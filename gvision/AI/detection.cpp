#include <detection.h>

bool AIDetection::getModel(const std:string& url, int index, const std::string& url){
    CVI_S32 ret = CVI_S32 CVI_TDL_OpenModel(this->handle, index, url.c_str());
    if (ret == CVI_TDL_ERR_INVALID_MODEL_PATH){
        std::cerr << "Incorrect model path" << std::endl;
        return -1 
    }
    if (ret == CVI_TDL_ERR_OPEN_MODEL){
        std::cerr << "Failed to start model" << std::endl;
    }
}

bool AIDetection::faceDetection (  const cvitdl_handle_t handle,
                                    VIDEO_FRAME_INFO_S *frame,
                                    CVI_TDL_SUPPORTED_MODEL_E model_id,
                                    cvtdl_face_t *face_meta ) {
    CVI_S32 CVI_TDL_FaceDetection();
    if (ret == CVI_TDL_ERR_OPEN_MODEL){
        std::cerr << "Failed to start model" << std::endl;
    }
}
bool AIDetection::objDectection (   const std::string& url,
                                    VIDEO_FRAME_INFO_S *frame,
                                    CVI_TDL_SUPPORTED_MODEL_E model_id,
                                    cvtdl_object_t *obj ){
    this->getModel(url.c_str(), frame,  )
    CVI_S32 ret = CVI_TDL_Detection(this->handle, frame, model_id, obj)
}