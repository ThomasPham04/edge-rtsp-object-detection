//
// Created by CuongTV on 26/12/2024.
// Copyright (c) GreenStreamVision
//

#ifndef G_VISION_DecoderAbstract_H
#define G_VISION_DecoderAbstract_H
#include <iostream>
#include <chrono> // !!
#include <callback/Callback.h>

namespace gvision {
 class DecoderAbstract {
  public:
	 DecoderAbstract(const std::string &uri, bool isKeyframe, bool isMotion, int fps);
	 DecoderAbstract(const std::string &id,const std::string &uri, bool isKeyframe, bool isMotion, int fps);
	
	 void setNewFrameCb(NewFrameCallback callback);
	
	 int calLastTimeObjectDetected();
	
	 virtual void start() = 0;
	
	 virtual void stop() = 0;
	
	 NewFrameCallback m_callback;
	
	 void log(std::string msg);
	 
	 int m_fps{5};
	
	 bool m_keyframe{false};
	 
	 bool m_isMotion{false};
	
	 std::string m_uri;
	
	 std::string m_id{"camera_id"};
	
	 std::chrono::time_point<std::chrono::system_clock> lastObjectDetected;
	 
 };
}

#endif //G_VISION_DecoderAbstract_H
