//
// Created by CuongTV on 18/01/2025.
// Copyright (c) GreenStreamVision
//

#include "libav_utils.h"
#include  <opencv2/opencv.hpp>

namespace gvision {
 bool isKeyframe(AVPacket *pkt) {
	 return pkt->flags & AV_PKT_FLAG_KEY;
 }
 
 bool avFrame2cvMat(AVFrame *frame, cv::Mat &dest) {
	 int cvSizes[1];
	 cvSizes[0] = (int) dest.step1();
	 SwsContext *conversion = sws_getContext(frame->width, frame->height, AV_PIX_FMT_YUV420P,
	                                         dest.cols, dest.rows, AV_PIX_FMT_BGR24,
	                                         SWS_BILINEAR, nullptr, nullptr, nullptr);
	 sws_scale(conversion, frame->data, frame->linesize, 0, frame->height, &dest.data, cvSizes);
	 sws_freeContext(conversion);
	 return true;
 }
 long int detectMotion(AVFrame *av_frame, int blockThreshold, int magnitudeThreshold) {
	 AVFrameSideData *sd;
	 sd = av_frame_get_side_data(av_frame, AV_FRAME_DATA_MOTION_VECTORS);
	 if (sd) {
		 const auto mvs = (const AVMotionVector *) sd->data;
		 int mvAcc = 0;
		 int block_cnt = 0;
		 for (int i = 0; i < sd->size / sizeof(*mvs); i++) {
			 const AVMotionVector *mv = &mvs[i];
			 // to check point in region
//			 auto x_c = float(mv->src_x + (float) mv->w / 2) / width;
//			 auto y_c = float(mv->src_y + (float) mv->h / 2) / height;
			 int motion_x = mv->motion_x;
			 int motion_y = mv->motion_y;
			 if (motion_x != 0 || motion_y != 0) {
				 double magnitude = sqrt(motion_x * motion_x + motion_y * motion_y);
				 if (magnitude > magnitudeThreshold) {
					 int area = mv->w * mv->h;
					 mvAcc += area;
					 block_cnt++;
				 }
			 }
		 }
		 return block_cnt;
	 } else {
		 return -1;
	 }
 }
	
}