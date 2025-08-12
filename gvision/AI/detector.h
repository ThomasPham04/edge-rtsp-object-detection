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
    int srcWidth;
    int srcHeight;
    cvitdl_handle_t handle;
    imgprocess_t img_handle;
    bool modelOpened = false;
    bool imageProcessorCreated = false;
    public:
    AIDetection();
    AIDetection(int srcWidth, int srcHeight);
    ~AIDetection(){
        if (imageProcessorCreated) {
            CVI_TDL_Destroy_ImageProcessor(this->img_handle);
            imageProcessorCreated = false;
        }
        if (handle) {
            CVI_TDL_DestroyHandle(this->handle);
            handle = nullptr;
        }
    }
    
    bool openModel ( const std::string& url,
                    CVI_TDL_SUPPORTED_MODEL_E modle);
    void setThresholds(CVI_TDL_SUPPORTED_MODEL_E model_id, float threshold, float nms_threshold);
    bool ensureImageProcessor();

    void objDectection ( const std::string& url,
                        VIDEO_FRAME_INFO_S *frame,
                        CVI_TDL_SUPPORTED_MODEL_E model_id,
                        cvtdl_object_t *obj,
                        float threshold
                    );
};