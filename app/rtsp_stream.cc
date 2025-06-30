//
// Created by CuongTV on 13/06/2025.
// Copyright (c) GreenStreamVision
//
#include "decoder/LibavDecoder.h"
#include <opencv2/highgui/highgui.hpp>
int main (){
	auto rtsp_decoder = new gvision::LibavDecoder("rtsp://admin:admin@116.193.72.50:8554:8554/ch1/1", true, true,15);
	rtsp_decoder->setNewFrameCb( [](cv::Mat cvMatFrame)->void {
		cv::imshow("frame",cvMatFrame);
		cv::waitKey(1);
		std::cout << cvMatFrame.cols << "\n";
	});
	rtsp_decoder->start();
	return 0;
}
