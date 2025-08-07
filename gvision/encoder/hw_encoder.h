#pragma once
#include "linux/cvi_comm_venc.h"
#include "cvi_venc.h"
#include <cstdio>
#include <string>
#include <iostream>
#include <unordered_map>
#include <climits>
#include "rtsp.h"
#include "middleware_utils.h"
class HardwareEncoder {
private:
    VENC_CHN veChn=0;
public:
    HardwareEncoder(int srcWidth, int srcHeight, std::string encodeType);
    ~HardwareEncoder(){
        CVI_VENC_StopRecvFrame(this->veChn);
        CVI_VENC_DestroyChn(this->veChn);
    };
    bool sendFrame(const VIDEO_FRAME_INFO_S* frame);
    bool getStream(VENC_STREAM_S *stream);
    CVI_S32 Send_Frame_RTSP(VIDEO_FRAME_INFO_S *frame, SAMPLE_TDL_MW_CONTEXT *pstMWContext);
};

