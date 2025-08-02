// hw_decoder.cpp
#include "hw_decoder.h"


HardwareDecoder::HardwareDecoder(int width, int height, std::string codecType) {
    CVI_S32 ret = CVI_SYS_Init();
    if (ret != CVI_SUCCESS) {
        std::cerr << "CVI_SYS_Init failed: " << ret << "\n";
    }
    VB_CONFIG_S stVbConf = {};
    stVbConf.u32MaxPoolCnt = 1;
    stVbConf.astCommPool[0].u32BlkSize = width * height * 3 / 2;
    stVbConf.astCommPool[0].u32BlkCnt = 4;
    ret = CVI_VB_SetConfig(&stVbConf);
    if (ret != CVI_SUCCESS){
        std::cerr << "CVI_VB_Setconfig failed\n";
    }
    CVI_VB_Init();
    std::unordered_map<std::string, PAYLOAD_TYPE_E> codec_type = {
        {"H.264", PT_H264},
        {"H.265", PT_H265}
    };

    VDEC_CHN_ATTR_S attr = {};
    attr.enType = codec_type[codecType];
    attr.enMode = VIDEO_MODE_FRAME;
    attr.u32PicWidth = width;
    attr.u32PicHeight = height;
    attr.u32StreamBufSize = width * height;
    attr.u32FrameBufCnt = 4;
    attr.u32FrameBufSize = width * height * 3 / 2;
    // attr.u32FrameBufSize = width * height;

    ret = CVI_VDEC_CreateChn(vdecChn, &attr);
    if (ret != CVI_SUCCESS) {
        std::cerr << "CVI_VDEC_CreateChn failed: " << ret << "\n";
    }

    ret = CVI_VDEC_StartRecvStream(vdecChn);
    if (ret != CVI_SUCCESS) {
        std::cerr << "CVI_VDEC_StartRecvStream failed: " << ret << "\n";
    }

    std::cout << "Hardware decoder initialized successfully\n";
}

bool HardwareDecoder::sendPacket(uint8_t *data, uint32_t size, int64_t pts) {
    VDEC_STREAM_S stream;
    memset(&stream, 0, sizeof(stream));
    stream.pu8Addr = data;
    stream.u32Len = size;
    stream.bEndOfFrame = CVI_TRUE;
    stream.bEndOfStream = CVI_FALSE;
    stream.u64PTS = pts;
    
    CVI_S32 ret = CVI_VDEC_SendStream(this->vdecChn, &stream, -1);
    if (ret != CVI_SUCCESS) {
        std::cerr << "SendStream failed: " << ret << "\n";
        return false;
    }
    return true;
}

bool HardwareDecoder::getFrame(VIDEO_FRAME_INFO_S *pFrame) {
    int ret = CVI_VDEC_GetFrame(this->vdecChn, pFrame, 2000);
    if (ret != CVI_SUCCESS) {
        std::cerr << "CVI_VDEC_GetFrame failed with error: " << ret << "\n";
        return false;
    }
    return true;
}

void HardwareDecoder::releaseFrame(VIDEO_FRAME_INFO_S *pFrame) {
    CVI_VDEC_ReleaseFrame(this->vdecChn, pFrame);
}