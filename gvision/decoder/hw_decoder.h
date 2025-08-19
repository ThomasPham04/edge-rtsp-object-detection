#pragma once
#include "cvi_vdec.h"
#include "cvi_comm_video.h"
#include "cvi_comm_vdec.h"
#include "cvi_sys.h"
#include "cvi_vb.h"
#include "cvi_comm_vb.h" 
#include <iostream>
#include <string>
#include <unordered_map>
// std::unordered_map<std::string, PAYLOAD_TYPE_E> decode_type = {
//         {"H.264", PT_H264},
//         {"H.265", PT_H265}
// };

class HardwareDecoder {
public:
    ~HardwareDecoder(){
        CVI_VDEC_StopRecvStream(vdecChn);
        CVI_VDEC_DestroyChn(vdecChn);
       
    }
    HardwareDecoder(int width, int height, PAYLOAD_TYPE_E codecType);
    bool sendPacket(uint8_t *data, uint32_t size, int64_t pts);
    bool getFrame(VIDEO_FRAME_INFO_S *pFrame);
    void releaseFrame(VIDEO_FRAME_INFO_S *pFrame);
private:
    VDEC_CHN vdecChn = 0;
    
};
