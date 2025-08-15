#include "rtsp_reader/rtsp_reader.h"
#include "decoder/hw_decoder.h"
#include "encoder/hw_encoder.h"
#include "AI/detector.h"
#include "sys/sys_init.h"
#include <csignal>
#include <unistd.h>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include "stream.h"
#include "BYTETracker.h"
#include "cvi_draw_rect.h"

std::unordered_map<std::string, PAYLOAD_TYPE_E> decode_type = {
        {"H.264", PT_H264},
        {"H.265", PT_H265}
};
std::atomic<bool> stop(false);

// Thread-safe queue for frames
struct FrameData {
    VIDEO_FRAME_INFO_S frame;
    cvtdl_object_t obj;
    int64_t pts;
    bool operator<(const FrameData& other) const {
        return pts > other.pts; // Priority queue: lower PTS first
    }
};

class ThreadSafeQueue {
private:
    std::queue<FrameData> queue;
    mutable std::mutex mutex;
    std::condition_variable condition;
    size_t maxSize;

public:
    ThreadSafeQueue(size_t maxSize = 10) : maxSize(maxSize) {}
    
    void push(const FrameData& item) {
        std::unique_lock<std::mutex> lock(mutex);
        if (queue.size() >= maxSize) {
            queue.pop(); // Remove oldest frame if queue is full
        }
        queue.push(item);
        condition.notify_one();
    }
    
    bool pop(FrameData& item) {
        std::unique_lock<std::mutex> lock(mutex);
        if (queue.empty()) {
            return false;
        }
        item = queue.front();
        queue.pop();
        return true;
    }
    
    bool waitAndPop(FrameData& item) {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this] { return !queue.empty() || stop; });
        if (queue.empty()) return false;
        item = queue.front();
        queue.pop();
        return true;
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }
};

// Global queues
ThreadSafeQueue frameQueue(5);  // Decoder -> AI Processing
ThreadSafeQueue processedFrameQueue(5);  // AI Processing -> Encoder

// Thread synchronization
std::mutex printMutex;
std::atomic<int> frameCount{0};

void int_handler(int signal){
    std::cout << "\nReceived signal " << signal << ", shutting down gracefully...\n";
    stop = true;
}

// Thread function for AI processing
void aiProcessingThread(AIDetection& detector, 
                       byte_track::BYTETracker& tracker, int srcWidth, int srcHeight) {
    std::cout << "AI Processing thread started\n";
    
    while (1) {
        FrameData frameData;
        if (frameQueue.waitAndPop(frameData)) {
            // Debug: Check frame data
            static int aiFrameCount = 0;
            aiFrameCount++;
            if (aiFrameCount % 30 == 0) {
                std::cout << "AI thread: Processing frame " << aiFrameCount 
                         << ", PTS: " << frameData.pts << "\n";
            }
            
            // Process AI detection
            detector.objDectection(&frameData.frame, &frameData.obj);
            
            // Process detected objects and draw tracking boxes
            std::vector<byte_track::Object> detected_objects;
            for (uint32_t i = 0; i < frameData.obj.size; i++) {
                if (frameData.obj.info[i].classes == 0) {  // Only person
                    float x1 = frameData.obj.info[i].bbox.x1;
                    float y1 = frameData.obj.info[i].bbox.y1;
                    float x2 = frameData.obj.info[i].bbox.x2;
                    float y2 = frameData.obj.info[i].bbox.y2;

                    float width = x2 - x1;
                    float height = y2 - y1;

                    // Clamp coordinates
                    x1 = std::max(0.0f, std::min(x1, (float)(srcWidth - 1)));
                    y1 = std::max(0.0f, std::min(y1, (float)(srcHeight - 1)));
                    width = std::min(width, (float)(srcWidth - x1));
                    height = std::min(height, (float)(srcHeight - y1));

                    byte_track::Rect<float> rect(x1, y1, width, height);
                    detected_objects.emplace_back(rect, 0, frameData.obj.info[i].bbox.score);
                }
            }

            auto tracks = tracker.update(detected_objects);
            
            // Draw tracking boxes on frame
            for (const auto& track : tracks) {
                const auto& rect = track->getRect();
                
                cvtdl_object_t obj_meta;
                memset(&obj_meta, 0, sizeof(obj_meta));
                cvtdl_service_brush_t brushi;
                brushi.color.r = 255;
                brushi.color.g = 255;
                brushi.color.b = 255;
                brushi.size = 4;

                obj_meta.size = 1;
                obj_meta.rescale_type = meta_rescale_type_e::RESCALE_CENTER;

                obj_meta.info = (cvtdl_object_info_t *)malloc(sizeof(cvtdl_object_info_t) * obj_meta.size);
                if (!obj_meta.info) {
                    std::cerr << "Memory allocation failed for obj_meta.info\n";
                    continue;
                }

                obj_meta.info[0].bbox.x1 = rect.x();
                obj_meta.info[0].bbox.y1 = rect.y();
                obj_meta.info[0].bbox.x2 = rect.x() + rect.width();
                obj_meta.info[0].bbox.y2 = rect.y() + rect.height();

                CVI_TDL_ObjectDrawRect2(&obj_meta, &frameData.frame, false, &brushi);
                free(obj_meta.info);
            }
            
            // Push processed frame to encoder queue
            processedFrameQueue.push(frameData);
            
            // Print tracking info
            {
                std::lock_guard<std::mutex> lock(printMutex);
                for (const auto& track : tracks) {
                    const auto& rect = track->getRect();
                    std::cout << "ID: " << track->getTrackId()
                            << " Box: x=" << rect.x()
                            << ", y=" << rect.y()
                            << ", width=" << rect.width()
                            << ", height=" << rect.height() << "\n";
                }
            }
        }
        if (stop) break;
    }
    
    std::cout << "AI Processing thread stopped\n";
}

// Thread function for encoding and RTSP output
void encodingThread(HardwareEncoder& encoder, rtspSession* session, 
                   HardwareDecoder& decoder) {
    std::cout << "Encoding thread started\n";
    
    while (!stop) {
        FrameData frameData;
        if (processedFrameQueue.waitAndPop(frameData)) {
            // Check if encoder is ready
            if (!encoder.isStarted()) {
                std::cerr << "Encoder not started; skipping frame\n";
                decoder.releaseFrame(&frameData.frame);
                CVI_TDL_Free(&frameData.obj);
                continue;
            }
            
            // Try to send frame to encoder with retry logic
            bool frameSent = false;
            int retryCount = 0;
            const int maxRetries = 3;
            
            static int encoderFrameCount = 0;
            encoderFrameCount++;
            
            while (!frameSent && retryCount < maxRetries && !stop) {
                frameSent = encoder.sendFrame(&frameData.frame);
                if (!frameSent) {
                    retryCount++;
                    std::cerr << "Failed to send frame " << encoderFrameCount << " to encoder, retry " << retryCount << "/" << maxRetries << "\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
            
            if (!frameSent) {
                std::cerr << "Failed to send frame " << encoderFrameCount << " after " << maxRetries << " retries, skipping\n";
                decoder.releaseFrame(&frameData.frame);
                CVI_TDL_Free(&frameData.obj);
                continue;
            }
            
            // Wait a bit for encoder to process the frame
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            
            // Query encoder status
            VENC_CHN_STATUS_S stStat;
            if (CVI_VENC_QueryStatus(encoder.getVencChn(), &stStat) != CVI_SUCCESS) {
                std::cerr << "CVI_VENC_QueryStatus failed for frame " << encoderFrameCount << "\n";
                decoder.releaseFrame(&frameData.frame);
                CVI_TDL_Free(&frameData.obj);
                continue;
            }

            if (stStat.u32CurPacks == 0) {
                std::cerr << "No encoded data available for frame " << encoderFrameCount << "\n";
                decoder.releaseFrame(&frameData.frame);
                CVI_TDL_Free(&frameData.obj);
                continue;
            }

            // Get encoded stream
            VENC_STREAM_S stStream;
            memset(&stStream, 0, sizeof(stStream));
            stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
            if (!stStream.pstPack) {
                std::cerr << "malloc failed for pstPack on frame " << encoderFrameCount << "\n";
                decoder.releaseFrame(&frameData.frame);
                CVI_TDL_Free(&frameData.obj);
                continue;
            }

            if (encoder.getStream(&stStream)) {
                // Send each packet to RTSP
                for (uint32_t i = 0; i < stStream.u32PackCount; i++) {
                    VENC_PACK_S *ppack = &stStream.pstPack[i];
                    const uint8_t* sendPtr = ppack->pu8Addr + ppack->u32Offset;
                    uint32_t sendLen = ppack->u32Len - ppack->u32Offset;
                    uint64_t pts = ppack->u64PTS;

                    if (!session->writeFrame(sendPtr, sendLen, pts)) {
                        std::cerr << "Failed to write frame " << encoderFrameCount << " to RTSP\n";
                    } else {
                        // Successfully sent to RTSP
                        frameCount++;
                        if (encoderFrameCount % 30 == 0) {
                            std::cout << "Encoder thread: Successfully encoded and sent frame " << encoderFrameCount << "\n";
                        }
                    }
                }
                encoder.releaseStream(&stStream);
            } else {
                std::cerr << "Failed to get stream from encoder for frame " << encoderFrameCount << "\n";
            }

            free(stStream.pstPack);
            stStream.pstPack = NULL;
            
            // Clean up frame resources
            decoder.releaseFrame(&frameData.frame);
            CVI_TDL_Free(&frameData.obj);
        }
    }
    
    std::cout << "Encoding thread stopped\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: app_AI_decode <model_path> [rtsp_url]\n";
        return -1;
    }
    
    byte_track::BYTETracker tracker(
        30,     // frame_rate
        30,     // track_buffer
        0.5,    // track_thresh
        0.6,    // high_thresh
        0.8     // match_thresh
    );
    
    RtspReader reader;
    signal(SIGINT, int_handler);
    std::cout << "-------------------Multithreaded Hardware Decoder-------------------\n";
    
    std::string rtspUrl = (argc >= 3) ? std::string(argv[2]) : ip;
    if (!reader.open(rtspUrl.c_str())){
        std::cerr << "Failed to open input RTSP stream\n";
        return -1;
    } else {
        std::cout << "Input RTSP stream opened successfully\n";
    }

    int srcWidth = reader.getVideoWidth();
    int srcHeight = reader.getVideoHeight();
    std::string codecType = reader.getCodecType();
    std::cout << "Video Width: " << srcWidth << ", Height: " << srcHeight << ", Type: " << codecType << "\n";

    SystemInit::init(srcWidth,srcHeight);
    rtspServer ser;
    ser.init(8854, 4);
    rtspSession *session = ser.createSession("cam1", RTSP_VIDEO_H264);

    PAYLOAD_TYPE_E type = decode_type[codecType];

    HardwareDecoder decoder(srcWidth, srcHeight, type);
    HardwareEncoder encoder(srcWidth, srcHeight, type);
    AIDetection detector(srcWidth, srcHeight);
    CVI_TDL_SUPPORTED_MODEL_E model = CVI_TDL_SUPPORTED_MODEL_YOLOV8_DETECTION;
    if (!detector.openModel(argv[1], model)) {
        std::cerr << "Failed to open model: " << argv[1] << "\n";
        reader.close();
        return -1;
    }
    detector.setThresholds(model, 0.5f, 0.5f);
    detector.ensureImageProcessor();
    
    // Start worker threads
    std::cout << "Starting worker threads...\n";
    
    std::thread aiThread(aiProcessingThread, std::ref(detector), 
                        std::ref(tracker), srcWidth, srcHeight);
    std::thread encoderThread(encodingThread, std::ref(encoder), std::ref(session), 
                            std::ref(decoder));
    
    // Main thread: Read RTSP and decode frames
    std::cout << "Main thread: Reading RTSP stream and decoding frames...\n";
    
    AVPacket pkt;
    int frameCounter = 0;
    
    while (reader.readPacket(pkt)) {
        frameCounter++;
        
        if (!decoder.sendPacket(pkt.data, pkt.size, pkt.pts)){
            std::cerr << "Failed to send packet to decoder\n";
            av_packet_unref(&pkt);
            continue;
        }
        
        VIDEO_FRAME_INFO_S frame;
        if (decoder.getFrame(&frame)) {
            // Debug: Check frame data
            if (frameCounter % 30 == 0) {
                std::cout << "Main thread: Got frame " << frameCounter 
                         << ", PTS: " << pkt.pts 
                         << ", Frame size: " << (frame.stVFrame.u32Length ? frame.stVFrame.u32Length : 0) << "\n";
                frameCounter = 0;
            }
            
            // Create frame data for AI processing
            FrameData frameData;
            frameData.frame = frame;
            frameData.pts = pkt.pts;
            
            // Initialize obj structure
            memset(&frameData.obj, 0, sizeof(cvtdl_object_t));
            
            // Push frame to AI processing queue
            frameQueue.push(frameData);
            
            // Print progress every 30 frames
            if (frameCounter % 30 == 0) {
                std::lock_guard<std::mutex> lock(printMutex);
                std::cout << "Main thread: Processed " << frameCounter 
                         << " frames, AI queue: " << frameQueue.size() 
                         << ", Encoder queue: " << processedFrameQueue.size() 
                         << ", Encoded: " << frameCount << "\n";
                frameCounter = 0;
            }
        } else {
            std::cerr << "Main thread: Failed to get frame from decoder\n";
        }
        
        av_packet_unref(&pkt);
        
        // Check stop signal
        if (stop) break;
    }
    
    std::cout << "Main thread: Stopping, waiting for worker threads...\n";
    
    // Wait for worker threads to finish
    if (aiThread.joinable()) {
        aiThread.join();
    }
    if (encoderThread.joinable()) {
        encoderThread.join();
    }
    
    std::cout << "All threads stopped. Cleanup...\n";
    
    // Cleanup
    reader.close();
    
    std::cout << "Multithreaded processing complete. Total frames encoded: " << frameCount << "\n";
    return 0;
}
