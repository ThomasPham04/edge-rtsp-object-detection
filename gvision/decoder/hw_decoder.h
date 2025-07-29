#pragma once
#include "cvi_vdec.h"
#include "cvi_comm_video.h"
#include "cvi_comm_vdec.h"
#include "cvi_sys.h"

#include <iostream>
#include <string>
class HardwareDecoder {
public:
    bool init(int width, int height);
    bool sendPacket(uint8_t *data, uint32_t size, int64_t pts);
    bool getFrame(VIDEO_FRAME_INFO_S *pFrame);
    void releaseFrame(VIDEO_FRAME_INFO_S *pFrame);
    void cleanup();
private:
    VDEC_CHN vdecChn = 0;
    // VB_POOL vbpool = VB_INVALID_POOLID;
};
