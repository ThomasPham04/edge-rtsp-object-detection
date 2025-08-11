#pragma once
#include "linux/cvi_comm_venc.h"
#include "cvi_venc.h"
#include <cstdio>
#include <string>
#include <iostream>
#include <unordered_map>
#include <climits>
#include "rtsp.h"
#include "middleware_utils.h"
#include <vector>
#include <memory>
#include <cstring>
#include <list>
// std::unordered_map<std::string, PAYLOAD_TYPE_E> encode_type = {
//         {"H.264", PT_H264},
//         {"H.265", PT_H265}
// };
class HardwareEncoder {
private:
    VENC_CHN veChn=0;
public:
    HardwareEncoder(int srcWidth, int srcHeight, PAYLOAD_TYPE_E encodeType);
    ~HardwareEncoder(){
        CVI_VENC_StopRecvFrame(this->veChn);
        CVI_VENC_DestroyChn(this->veChn);
    };
    bool sendFrame(const VIDEO_FRAME_INFO_S* frame);
    bool getStream(VENC_STREAM_S *stream);
    VENC_CHN getVencChn(){return veChn;}
};

class rtspSession {
private:
    CVI_RTSP_CTX* ctx = nullptr;
    CVI_RTSP_SESSION* session = nullptr;
    CVI_RTSP_TRACK track = nullptr; 

public:
    rtspSession() = default;
    
    rtspSession(CVI_RTSP_CTX* ctx, const std::string& name, CVI_RTSP_VIDEO_CODEC codec);
    ~rtspSession();

    
    bool writeFrame(const uint8_t* data, uint32_t len, uint64_t timestamp = 0);

    CVI_RTSP_CTX* getCtx() const {return ctx;}    
    CVI_RTSP_SESSION* getSession() const { return session; }
    CVI_RTSP_TRACK   getTrack()   const { return track; }
};

class rtspServer {
private:
    CVI_RTSP_CTX* rtspCtx = nullptr;
    std::list<rtspSession> sessions; 

public:
    rtspServer() = default;
    ~rtspServer();

    bool init(int port = 8554, int maxConn = 4);
    
    rtspSession* createSession(const std::string& name, CVI_RTSP_VIDEO_CODEC codec = RTSP_VIDEO_H265);

    CVI_RTSP_CTX* getCtx() const { return rtspCtx; }
};

