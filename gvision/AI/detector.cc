#include "detector.h"

AIDetection::AIDetection(){  
    CVI_S32 ret = CVI_TDL_CreateHandle(&handle);
    if (ret != CVI_SUCCESS){
        std::cerr << "Can't create handle";
    }
}
AIDetection::AIDetection (int srcWidth, int srcHeight){
    this->srcWidth = srcWidth;
    this->srcHeight = srcHeight;
    CVI_S32 ret = CVI_TDL_CreateHandle(&handle);
    if (ret != CVI_SUCCESS){
        std::cerr << "Can't create handle";
    }
    ret = MMF_INIT_HELPER2(srcWidth, srcHeight, PIXEL_FORMAT_YUV_PLANAR_420, 1,
                                 srcWidth, srcHeight, PIXEL_FORMAT_YUV_PLANAR_420, 1);

    if (ret != CVI_SUCCESS){
        std::cerr << "Init sys failed";
    }
}

bool AIDetection::openModel(const std::string& url, CVI_TDL_SUPPORTED_MODEL_E model){
    CVI_S32 ret = CVI_TDL_OpenModel(this->handle, model, url.c_str());
    if (ret == CVI_TDL_ERR_INVALID_MODEL_PATH){
        std::cerr << "Incorrect model path\n";
        return false;
    }
    if (ret == CVI_TDL_ERR_OPEN_MODEL){
        std::cerr << "Failed to start model\n";
        return false;
    }
    modelOpened = true;
    return true;
}

void AIDetection::setThresholds(CVI_TDL_SUPPORTED_MODEL_E model, float threshold, float nms_threshold) {
    CVI_TDL_SetModelThreshold(this->handle, model, threshold);
    CVI_TDL_SetModelNmsThreshold(this->handle, model, nms_threshold);
}

bool AIDetection::ensureImageProcessor() {
    if (!imageProcessorCreated) {
        if (CVI_TDL_Create_ImageProcessor(&(this->img_handle)) == CVI_SUCCESS) {
            imageProcessorCreated = true;
        } else {
            std::cerr << "Failed to create image processor\n";
            return false;
        }
    }
    return true;
}

void AIDetection::objDectection (VIDEO_FRAME_INFO_S *frame, cvtdl_object_t *obj){
    if (!modelOpened) {
        std::cerr << "Model not opened. Call openModel() before detection.\n";
        return;
    }
    if (!ensureImageProcessor()) {
        return;
    }
    CVI_S32 ret = CVI_TDL_YOLOV8_Detection(this->handle, frame, obj);
    if (ret != CVI_SUCCESS) {
        std::cerr << "Detection failed with code: " << ret << "\n";
    }

    // int eval_perf = 0;
    // if (eval_perf) {
    //     for (int i = 0; i < 101; i++) {
    //     cvtdl_object_t obj = {0};
    //     CVI_TDL_PersonPet_Detection(this->handle, frame, &obj);
    //     CVI_TDL_Free(&obj);
    //     }
    // }
}