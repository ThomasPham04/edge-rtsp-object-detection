#include "sys_init.h"

static inline uint32_t AlignUp(uint32_t x, uint32_t align) {
    return (x + align - 1) & ~(align - 1);
}

bool SystemInit::init(int srcWidth, int srcHeight) {
    CVI_VB_Exit();
    CVI_SYS_Exit();

    VB_CONFIG_S stVbConf;
    memset(&stVbConf, 0, sizeof(stVbConf));
    stVbConf.u32MaxPoolCnt = 2;

    uint32_t alignW = AlignUp(srcWidth, 16);
    uint32_t alignH = AlignUp(srcHeight, 2);
    // Pool 0: YUV420 frame buffers (decoder/output frames)
    stVbConf.astCommPool[0].u32BlkSize = alignW * alignH * 3 / 2;
    stVbConf.astCommPool[0].u32BlkCnt = 6;

    // Pool 1: Encoder stream buffers (bitstream). Size is conservative.
    // Adjust if needed based on actual bitrate/resolution.
    stVbConf.astCommPool[1].u32BlkSize = alignW * alignH;
    stVbConf.astCommPool[1].u32BlkCnt = 4;

    CVI_VB_SetConfig(&stVbConf);
    CVI_VB_Init();

    if (CVI_SYS_Init() != CVI_SUCCESS) {
        std::cerr << "CVI_SYS_Init failed\n";
        CVI_VB_Exit();
        return false;
    }

    return true;
}
