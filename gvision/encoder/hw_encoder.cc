#include "hw_encoder.h"
HardwareEncoder::HardwareEncoder(int srcWidth, int srcHeight, PAYLOAD_TYPE_E encodeType) {
    CVI_S32 ret;
    VENC_CHN_ATTR_S chnAttr;
    memset(&chnAttr, 0, sizeof(chnAttr));
    
    chnAttr.stVencAttr.enType          = encodeType;
    chnAttr.stVencAttr.u32PicWidth     = srcWidth;
    chnAttr.stVencAttr.u32PicHeight    = srcHeight;
    chnAttr.stVencAttr.u32MaxPicWidth  = srcWidth;
    chnAttr.stVencAttr.u32MaxPicHeight = srcHeight;
    
    chnAttr.stVencAttr.u32BufSize = ALIGN(srcWidth, 16) * ALIGN(srcHeight, 16) * 3 / 2;
    // chnAttr.stVencAttr.u32BufSize = srcWidth*srcHeight*1.5;
    
    if (encodeType == PT_H264) {
        
        chnAttr.stVencAttr.u32Profile = 1;
    } else if (encodeType == PT_H265) {
        
        chnAttr.stVencAttr.u32Profile = 0;
    } else {
        chnAttr.stVencAttr.u32Profile = 1;
    }

    chnAttr.stVencAttr.bByFrame        = CVI_TRUE;
    chnAttr.stVencAttr.bSingleCore     = CVI_FALSE;
    chnAttr.stVencAttr.bEsBufQueueEn   = CVI_FALSE;
    chnAttr.stVencAttr.bIsoSendFrmEn   = CVI_FALSE;

    

    if (encodeType == PT_H264) {
        chnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
        chnAttr.stRcAttr.stH264Cbr.u32Gop = 20;
        chnAttr.stRcAttr.stH264Cbr.u32BitRate = 409600; 
        chnAttr.stRcAttr.stH264Cbr.u32SrcFrameRate = 20;
        chnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate = 20;
        chnAttr.stRcAttr.stH264Cbr.bVariFpsEn = 0;
        chnAttr.stRcAttr.stH264Cbr.u32StatTime = 1;
    } else if (encodeType == PT_H265) {
        chnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
        chnAttr.stRcAttr.stH265Cbr.u32Gop = 40;
        chnAttr.stRcAttr.stH265Cbr.u32BitRate = 3000000;
        chnAttr.stRcAttr.stH265Cbr.u32SrcFrameRate = 20;
        chnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate = 20;
    } else {
        chnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
        chnAttr.stRcAttr.stH264Vbr.u32Gop = 40;
        chnAttr.stRcAttr.stH264Vbr.u32MaxBitRate = 4000000;
        chnAttr.stRcAttr.stH264Vbr.u32SrcFrameRate = 20;
        chnAttr.stRcAttr.stH264Vbr.fr32DstFrameRate = 20;
    }

    chnAttr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
    
    ret = CVI_VENC_CreateChn(this->veChn, &chnAttr);
    if (ret != CVI_SUCCESS) {
        std::cerr << "\nCVI_VENC_CreateChn failed: " << ret << "\n";
        return;
    } else {
        std::cout << "HardwareEncoder create channel successfully\n";
    }

    
    VENC_RECV_PIC_PARAM_S recAttr;
    memset(&recAttr, 0, sizeof(recAttr));
    
    recAttr.s32RecvPicNum = -1;
    ret = CVI_VENC_StartRecvFrame(this->veChn, &recAttr);
    if (ret != CVI_SUCCESS) {
        std::cerr << "\nCVI_VENC_StartRecvFrame failed: " << ret << "\n";
    } else {
        std::cout << "Encoder init successfully\n";
        this->started = true;
    }

}


bool HardwareEncoder::sendFrame(const VIDEO_FRAME_INFO_S* frame){
    CVI_S32 ret = CVI_VENC_SendFrame(this->veChn, frame, -1);
    if (ret != CVI_SUCCESS) {
        std::cerr << "CVI_VENC_SendFrame failed with error: " << ret << "\n";
        return false;
    }
    return true;
}

bool HardwareEncoder::getStream(VENC_STREAM_S *stream){
    CVI_S32 ret = CVI_VENC_GetStream(this->veChn, stream, -1);
    if (ret != CVI_SUCCESS){
        std::cerr << "Failed to get stream";
        return false;
    }    
    return true;
}

void HardwareEncoder::releaseStream(VENC_STREAM_S *stStream) {
    CVI_VENC_ReleaseStream(this->veChn, stStream);
}

rtspServer::~rtspServer() {
    
    sessions.clear();

    if (rtspCtx) {
        CVI_RTSP_Stop(rtspCtx);
        CVI_RTSP_Destroy(&rtspCtx);
        rtspCtx = nullptr;
    }
}

bool rtspServer::init(int port, int maxConn) {
    CVI_RTSP_CONFIG config;
    memset(&config, 0, sizeof(config));
    config.port = port;
    config.timeout = 5;
    config.maxConnNum = maxConn;
    config.tcpBufSize = 1024 * 1024;

    if (CVI_RTSP_Create(&rtspCtx, &config) != 0) {
        std::cerr << "CVI_RTSP_Create failed\n";
        rtspCtx = nullptr;
        return false;
    }

    if (CVI_RTSP_Start(rtspCtx) != 0) {
        std::cerr << "CVI_RTSP_Start failed\n";
        CVI_RTSP_Destroy(&rtspCtx);
        rtspCtx = nullptr;
        return false;
    }

    return true;
}

rtspSession* rtspServer::createSession(const std::string& name, CVI_RTSP_VIDEO_CODEC codec) {
    if (!rtspCtx) {
        std::cerr << "rtspCtx is null. Call init() first.\n";
        return nullptr;
    }

    
    sessions.emplace_back(rtspCtx, name, codec);
    return &sessions.back();
}

/* ---------------- rtspSession implementation ---------------- */

rtspSession::rtspSession(CVI_RTSP_CTX* ctx_, const std::string& name, CVI_RTSP_VIDEO_CODEC codec)
    : ctx(ctx_), session(nullptr), track(nullptr)
{
    CVI_RTSP_SESSION_ATTR attr;
    memset(&attr, 0, sizeof(attr));
    
    std::strncpy(attr.name, name.c_str(), sizeof(attr.name) - 1);
    attr.name[sizeof(attr.name) - 1] = '\0';

    attr.duration = 0.0f;            
    attr.reuseFirstSource = 1;
    attr.maxConnNum = 1;
    attr.video.bitrate = 0;          
    attr.video.codec = codec;

    if (CVI_RTSP_CreateSession(ctx, &attr, &session) != 0) {
        std::cerr << "CVI_RTSP_CreateSession failed for " << name << "\n";
        session = nullptr;
        track = nullptr;
        return;
    }

    
    
    if (session) {
        track = session->video;
    }
}

rtspSession::~rtspSession() {
    if (session && ctx) {
        CVI_RTSP_DestroySession(ctx, session);
        session = nullptr;
        track = nullptr;
    }
}

bool rtspSession::writeFrame(const uint8_t* data, uint32_t len, uint64_t timestamp) {
    if (!ctx || !session || !track || !data || len == 0) return false;

    CVI_RTSP_DATA rtspData;
    memset(&rtspData, 0, sizeof(rtspData));

    
    rtspData.blockCnt = 1;
    rtspData.dataPtr[0] = const_cast<uint8_t*>(data);
    rtspData.dataLen[0] = len;
    rtspData.timestamp = timestamp;

    int ret = CVI_RTSP_WriteFrame(ctx, track, &rtspData);
    if (ret != 0) {
        std::cerr << "CVI_RTSP_WriteFrame failed: " << ret << "\n";
        return false;
    }
    return true;
}

