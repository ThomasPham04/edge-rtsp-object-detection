#pragma once
#include <iostream>
extern "C"{
    #include "cvi_tdl.h"
}
#include <iostream>
#include <string>

class AI_detection{
    private:
    cvitdl_handle_t handle;

    public:
    AI_detection(){
        CVI_S32 CVI_TDL_CreateHandle(&handle);
    }
    ~AI_detection(){
        CVI_S32 CVI_TDL_DestroyHandle(&handle); 
    }
    
    bool getModel (const std:string& url, int index, const std::string& url);
    bool faceDetection (const cvitdl_handle_t handle, VIDEO_FRAME_INFO_S *frame, CVI_TDL_SUPPORTED_MODEL_E model_id, cvtdl_face_t *face_meta);
}