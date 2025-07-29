// hw_decoder.cpp
#include "hw_decoder.h"

bool HardwareDecoder::init(int width, int height) {
    // CVI_VB_Init();

    CVI_S32 ret = CVI_SYS_Init();
    if (ret != CVI_SUCCESS) {
        std::cerr << "CVI_SYS_Init failed: " << ret << "\n";
        return false;
    }

    VDEC_CHN_ATTR_S attr = {};
    attr.enType = PT_H264;
    attr.enMode = VIDEO_MODE_FRAME;
    attr.u32PicWidth = width;
    attr.u32PicHeight = height;
    attr.u32StreamBufSize = width * height;
    attr.u32FrameBufCnt = 4;
    // attr.u32FrameBufSize = width * height * 3 / 2;
    attr.u32FrameBufSize = width * height;

    ret = CVI_VDEC_CreateChn(vdecChn, &attr);
    if (ret != CVI_SUCCESS) {
        std::cerr << "CVI_VDEC_CreateChn failed: " << ret << "\n";
        return false;
    }

    ret = CVI_VDEC_StartRecvStream(vdecChn);
    if (ret != CVI_SUCCESS) {
        std::cerr << "CVI_VDEC_StartRecvStream failed: " << ret << "\n";
        return false;
    }

    std::cout << "Hardware decoder initialized successfully\n";
    return true;
}

bool HardwareDecoder::sendPacket(uint8_t *data, uint32_t size, int64_t pts) {
    VDEC_STREAM_S stream;
    memset(&stream, 0, sizeof(stream));
    stream.pu8Addr = data;
    stream.u32Len = size;
    stream.bEndOfFrame = CVI_TRUE;
    stream.bEndOfStream = CVI_FALSE;
    stream.u64PTS = pts;

    CVI_S32 ret = CVI_VDEC_SendStream(vdecChn, &stream, -1);
    if (ret != CVI_SUCCESS) {
        std::cerr << "SendStream failed: " << ret << "\n";
        return false;
    }
    return true;
}

bool HardwareDecoder::getFrame(VIDEO_FRAME_INFO_S *pFrame) {
    int ret = CVI_VDEC_GetFrame(vdecChn, pFrame, -1);
    if (ret != CVI_SUCCESS) {
        std::cerr << "CVI_VDEC_GetFrame failed with error: " << ret << "\n";
        return false;
    }
    return true;
}

void HardwareDecoder::releaseFrame(VIDEO_FRAME_INFO_S *pFrame) {
    CVI_VDEC_ReleaseFrame(vdecChn, pFrame);
}

void HardwareDecoder::cleanup() {
    CVI_VDEC_StopRecvStream(vdecChn);
    CVI_VDEC_DestroyChn(vdecChn);
    CVI_SYS_Exit();
}
