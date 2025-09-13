#include "sys_init.h"

bool SystemInit::init(int srcWidth, int srcHeight){
    CVI_S32 ret;
    ret = CVI_SYS_Exit();
    if (ret != CVI_SUCCESS){
        std::cerr << "CVI_SYS_Exit failed: " << ret << "\n";
        return false;
    }
    ret = CVI_VB_Exit();
    if (ret != CVI_SUCCESS) {
        std::cerr << "CVI_VB_Exit failed: " << ret << "\n";
        return false;
    }
    VB_CONFIG_S stVbConf = {};
    stVbConf.u32MaxPoolCnt = 3;
    stVbConf.astCommPool[0].u32BlkSize = srcWidth * srcHeight * 3 / 2;
    stVbConf.astCommPool[0].u32BlkCnt = 24;

    stVbConf.astCommPool[1].u32BlkSize = srcWidth * srcHeight * 3 / 2;
    stVbConf.astCommPool[1].u32BlkCnt = 24;

    // stVbConf.astCommPool[2].u32BlkSize = srcWidth * srcHeight * 3 / 2;
    // stVbConf.astCommPool[2].u32BlkCnt = 12;
    ret = CVI_VB_SetConfig(&stVbConf);
    if (ret != CVI_SUCCESS){
        std::cerr << "CVI_VB_Setconfig failed\n";
        return false;
    }
    CVI_VB_Init();
    ret = CVI_SYS_Init();

    if (ret != CVI_SUCCESS) {   
        std::cerr << "CVI_SYS_INIT failed" << ret << "\n";
        CVI_VB_Exit();
        return false;
    }

    return true;
}