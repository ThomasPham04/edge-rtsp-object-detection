#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "core/cvi_tdl_types_mem_internal.h"
#include "core/utils/vpss_helper.h"
#include "cvi_tdl.h"
#include "cvi_tdl_media.h"

class AIDetection{
    private:
    cvitdl_handle_t handle;

    public:
    AIDetection();
    AIDetection(int srcWidth, int srcHeight);
    ~AIDetection(){
        CVI_TDL_DestroyHandle(handle); 
    }
    
    bool getModel ( const std::string& url,
                    CVI_TDL_SUPPORTED_MODEL_E module);

    bool faceDetection ( //const cvitdl_handle_t handle,
                        VIDEO_FRAME_INFO_S *frame,
                        CVI_TDL_SUPPORTED_MODEL_E model_id,
                        cvtdl_face_t *face_meta);
                        
    void objDectection ( const std::string& url,
                        VIDEO_FRAME_INFO_S *frame,
                        CVI_TDL_SUPPORTED_MODEL_E model_id,
                        cvtdl_object_t *obj);
};