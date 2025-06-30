//
// Created by CuongTV on 26/12/2024.
// Copyright (c) GreenStreamVision
//

#ifndef G_VISION_LibavDecoder_H
#define G_VISION_LibavDecoder_H
#include "DecoderAbstract.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/motion_vector.h>
#include <unistd.h>
}

namespace gvision {
 class LibavDecoder : public DecoderAbstract {
  public:
	 LibavDecoder(const std::string &uri, bool isKeyframe, bool isMotion, int fps);
	 
	 LibavDecoder(const std::string &id, const std::string &uri, bool isKeyframe, bool isMotion, int fps);
	 
	 void start() override;
	 
	 void decodeAV();
	 
	 void stop() override;
  
  private:
	 void init();
	 void processAvFrame(AVFrame *av_frame, AVPacket *packet, int frame_cnt, int gapFrames);
	 
	 bool detectMotion(AVFrame *av_frame, AVPacket *packet, int frame_cnt, int gapFrames);
	 
	 bool m_running{false};
	 
	 std::chrono::time_point<std::chrono::system_clock> lastReceivedAvFrame;
	 
	 bool m_restart{false};
	 
	 int avFrameCounter{1};
	 
	 int motionMagnitudeThreshold{10};
	 
	 int motionBlockThreshold{25};
 };
}

#endif //G_VISION_LibavDecoder_H
