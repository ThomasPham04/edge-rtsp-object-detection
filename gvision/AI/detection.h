#pragma once
#include <iostream>
extern "C"{
    #include "cvi_tdl.h"
}
#include <iostream>
#include <string>

class AIDetection{
    private:
    cvitdl_handle_t handle;

    public:
    AIDetection(){
        CVI_S32 CVI_TDL_CreateHandle(&handle);
    }
    ~AIDetection(){
        CVI_S32 CVI_TDL_DestroyHandle(&handle); 
    }
    
    bool getModel ( const std:string& url,
                    int index, 
                    const std::string& url);

    bool faceDetection ( //const cvitdl_handle_t handle,
                        VIDEO_FRAME_INFO_S *frame,
                        CVI_TDL_SUPPORTED_MODEL_E model_id,
                        cvtdl_face_t *face_meta);
                        
    bool objDectection ( const std::string url,
                        VIDEO_FRAME_INFO_S *frame,
                        CVI_TDL_SUPPORTED_MODEL_E model_id,
                        cvtdl_object_t *obj)
}