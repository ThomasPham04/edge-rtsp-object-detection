//
// Created by CuongTV on 26/12/2024.
// Copyright (c) GreenStreamVision
//

#include "DecoderAbstract.h"
#include <chrono> // !!
namespace gvision {
 DecoderAbstract::DecoderAbstract(const std::string &uri, bool isKeyframe, bool isMotion, int fps) {
	 m_uri = uri;
	 m_keyframe = isKeyframe;
	 m_fps = fps;
	 m_isMotion = isMotion;
 }
 
 void DecoderAbstract::setNewFrameCb(NewFrameCallback callback) {
	 m_callback = std::move(callback);
 }
 int DecoderAbstract::calLastTimeObjectDetected() {
	 auto current_time = std::chrono::system_clock::now();
	 auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		 current_time - lastObjectDetected).count();
	 return int(elapsed_ms);
 }
 DecoderAbstract::DecoderAbstract(const std::string &id,
                                  const std::string &uri,
                                  bool isKeyframe,
                                  bool isMotion,
                                  int fps) {
	 m_uri = uri;
	 m_keyframe = isKeyframe;
	 m_fps = fps;
	 m_isMotion = isMotion;
	 m_id = id;
 }
 void DecoderAbstract::log(std::string msg) {
	 std::stringstream ss;
	 ss << "[" << m_id << "] " << msg << std::endl;
	 std::cout << ss.str();
	 
 }
}