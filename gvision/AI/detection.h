#pragma once
// extern "C" {
//     #include "cvi_tdl.h"
//     // #include "cvi_comm.h"
// }
#include "cvi_tdl.h"
#include <iostream>
#include <string>

class AIDetection{
    private:
    cvitdl_handle_t handle;

    public:
    AIDetection(){
        CVI_S32 ret = CVI_TDL_CreateHandle(&handle);
        if (ret != CVI_SUCCESS){
            std::cerr << "Can't create handle";
        }
    }
    ~AIDetection(){
        CVI_TDL_DestroyHandle(handle); 
    }
    
    bool getModel ( const std::string& url,
                    CVI_TDL_SUPPORTED_MODEL_E module);

    bool faceDetection ( //const cvitdl_handle_t handle,
                        VIDEO_FRAME_INFO_S *frame,
                        CVI_TDL_SUPPORTED_MODEL_E model_id,
                        cvtdl_face_t *face_meta);
                        
    bool objDectection ( const std::string& url,
                        VIDEO_FRAME_INFO_S *frame,
                        CVI_TDL_SUPPORTED_MODEL_E model_id,
                        cvtdl_object_t *obj);
};