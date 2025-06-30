//
// Created by CuongTV on 26/12/2024.
// Copyright (c) GreenStreamVision
//

#include "LibavDecoder.h"
#include <thread>
#include "libav_utils.h"

namespace gvision {
 
 void LibavDecoder::start() {
	 m_running = true;
	 decodeAV();
 }
 void LibavDecoder::decodeAV() {
	 
	 int cnt{0};
	 
	 while (m_running) {
		 AVDictionary *m_opts{nullptr};
		 av_dict_set(&m_opts, "rw_timeout", "3000000", 0);
		 av_dict_set(&m_opts, "stimeout", "3000000", 0);
		 av_dict_set(&m_opts, "rtsp_transport", "tcp", 0);
		 if (m_isMotion) {
			 av_dict_set(&m_opts, "flags2", "+export_mvs", 0);
		 }
		 int retry = 0;
		 
		 while (m_running) {
			 AVFormatContext *pFormatCtx = avformat_alloc_context();
			 if (avformat_open_input(&pFormatCtx, this->m_uri.c_str(), nullptr, &m_opts) < 0) {
				 avformat_close_input(&pFormatCtx);
				 retry++;
				 continue;
			 }
			 if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
				 avformat_close_input(&pFormatCtx);
				 retry++;
				 continue;
			 }
			
			 int videoStreamIndex = -1;
			 for (int i = 0; i < pFormatCtx->nb_streams; i++) {
				 if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
					 videoStreamIndex = i;
					 break;
				 }
			 }
			 //
			 AVRational frameRate = av_guess_frame_rate(pFormatCtx, pFormatCtx->streams[videoStreamIndex], nullptr);
			
			 int fps = (int) (frameRate.num / frameRate.den);
			 if (fps > 30) fps = 30;
			 int numPerFrames = (int) fps / fps;
			 if (fps < 10) {
				 numPerFrames = 1;
			 }
	
			 AVCodecContext *pCodecCtx = avcodec_alloc_context3(nullptr);
			 if (avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStreamIndex]->codecpar) < 0) {
				 avformat_close_input(&pFormatCtx);
				 avcodec_close(pCodecCtx);
				 retry++;
				 continue;
			 }
			
			 const AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
			 if (pCodec == nullptr) {
				 avformat_close_input(&pFormatCtx);
				 avcodec_close(pCodecCtx);
				 retry++;
				 continue;
			 }
			 if (avcodec_open2(pCodecCtx, pCodec, &m_opts) < 0) {
				 avcodec_close(pCodecCtx);
				 avformat_close_input(&pFormatCtx);
				 retry++;
				 continue;
			 }
			 int file_cnt = 0;
			 FILE *ts_file;
			 auto start_time = std::chrono::high_resolution_clock::now();
			 lastReceivedAvFrame = std::chrono::system_clock::now();
			 AVPacket packet;
			 m_restart = false;
			 while (av_read_frame(pFormatCtx, &packet) >= 0 && m_running || m_restart) {
				
				 if (packet.stream_index == videoStreamIndex) {
					 AVFrame *frame = av_frame_alloc();
					 avcodec_send_packet(pCodecCtx, &packet);
					 retry = 0;
					 while (avcodec_receive_frame(pCodecCtx, frame) >= 0) {
						 avFrameCounter++;
						 processAvFrame(frame, &packet, avFrameCounter, numPerFrames);
						 lastReceivedAvFrame = std::chrono::system_clock::now();
					 }
					 if (avFrameCounter % 150 == 0) {
						 std::cout << "------------- decode ---------------------------------\n";
						 std::cout << "Av Frames:" << avFrameCounter << std::endl;
						 std::cout << "------------- decode ---------------------------------\n";
						
					 }
					 av_freep(&frame->data);
					 av_frame_free(&frame);
				 } // packet.stream_index == videoStreamIndex
				 av_packet_unref(&packet);
				 if (!m_running) {
					 break;
				 }
				 
			 } // end (av_read_frame(pFormatCtx, &packet) >= 0)
			 //
			 
			 //
			 av_packet_unref(&packet);
			 avcodec_close(pCodecCtx);
			 avformat_close_input(&pFormatCtx);
			 retry++;
			 
		 }
	 }
	 std::cout << cnt << "stop  decode \n";
 }
 void LibavDecoder::stop() {
	 m_running = false;
 }
 
 LibavDecoder::LibavDecoder(const std::string &uri, bool isKeyframe, bool isMotion, int fps)
	 : DecoderAbstract(uri, isKeyframe, isMotion, fps) {
 }
 LibavDecoder::LibavDecoder(const std::string &id, const std::string &uri, bool isKeyframe, bool isMotion, int fps)
	 : DecoderAbstract(id, uri, isKeyframe, isMotion, fps) {
 }
 void LibavDecoder::processAvFrame(AVFrame *av_frame, AVPacket *packet, int frame_cnt, int gapFrames) {
	 auto isMotion = detectMotion(av_frame, packet, frame_cnt, gapFrames);
	 if (isMotion) {
		 this->log("motion_detected");
	 }
	 if (isKeyframe(packet) || isMotion) {
		 cv::Mat image(av_frame->height, av_frame->width, CV_8UC3, cv::Scalar(0, 0, 0));
		 if (avFrame2cvMat(av_frame, image)) {
			 this->m_callback(image);
		 }
	 }
	 
 }
 bool LibavDecoder::detectMotion(AVFrame *av_frame, AVPacket *packet, int frame_cnt, int gapFrames) {
	 bool isMotionDetected = false;
	 int i;
	 AVFrameSideData *sd;
	 int width = av_frame->width;
	 int height = av_frame->height;
//	 if (calLastTimeObjectDetected() > 3000)
	 {
		 sd = av_frame_get_side_data(av_frame, AV_FRAME_DATA_MOTION_VECTORS);
		 isMotionDetected = false;
		 if (sd) {
			 cv::Mat image(height, width, CV_8UC3, cv::Scalar(0, 0, 0));
			 const auto mvs = (const AVMotionVector *) sd->data;
			 int mvAcc = 0;
			 int block_cnt = 0;
			 for (i = 0; i < sd->size / sizeof(*mvs); i++) {
				 const AVMotionVector *mv = &mvs[i];
				 auto x_c = float(mv->src_x + (float) mv->w / 2) / width;
				 auto y_c = float(mv->src_y + (float) mv->h / 2) / height;
				 cv::Point2f p(x_c, y_c);
//				 if (m_streamInfo->isPoint2fInRegion(p))
				 {
					 int motion_x = mv->motion_x;
					 int motion_y = mv->motion_y;
					 if (motion_x != 0 || motion_y != 0) {
						 double magnitude = sqrt(motion_x * motion_x + motion_y * motion_y);
						 if (magnitude > motionMagnitudeThreshold) {
							 int area = mv->w * mv->h;
							 mvAcc += area;
							 block_cnt++;
						 }
					 }
					 
				 }
			 }
			 
//			 float percent = (float) mvAcc * 100 / (av_frame->height * av_frame->width);
			 if (block_cnt > motionBlockThreshold) {
				 isMotionDetected = true;
			 }
			 
		 }
	 }
	 return isMotionDetected;
 }
 void LibavDecoder::init() {
	
 }
	
}