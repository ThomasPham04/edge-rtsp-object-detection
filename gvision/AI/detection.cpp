#include <detection.h>

bool AI_detection::getModel(const std:string& url, int index, const std::string& url){
    CVI_S32 ret = CVI_S32 CVI_TDL_OpenModel(this->handle, index, url.c_str());
    if (ret != CVI_SUCCESS){
        
    }
}

bool AI_detection::faceDetection (const cvitdl_handle_t handle, VIDEO_FRAME_INFO_S *frame, CVI_TDL_SUPPORTED_MODEL_E model_id, cvtdl_face_t *face_meta){
    CVI_S32 CVI_TDL_FaceDetection();
}