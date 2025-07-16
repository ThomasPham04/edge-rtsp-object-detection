//
// Created by CuongTV on 13/06/2025.
// Copyright (c) GreenStreamVision
//
//      rtsp://admin:gsv@101Aa@192.168.50.4:554/Streaming/channels/101
//      rtsp://admin:gsv@101Aa@192.168.50.5:554/Streaming/channels/101
//      rtsp://admin:admin@116.193.72.50:8554:8554/ch1/1
//      rtsp://admin:gsv@101Aa@vtech.greenstreamvision.com:8556/Streaming/channels/101

#include "decoder/LibavDecoder.h"
#include <opencv2/highgui/highgui.hpp>	
int main (){
	auto rtsp_decoder = new gvision::LibavDecoder("rtsp://admin:gsv@101Aa@vtech.greenstreamvision.com:8556/Streaming/channels/101", true, true,15);
	rtsp_decoder->setNewFrameCb( [](cv::Mat cvMatFrame)->void {
		cv::imshow("frame",cvMatFrame);
		cv::waitKey(1);
	});
	rtsp_decoder->start();
	return 0;
}
