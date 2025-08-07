#include "hw_encoder.h"
std::unordered_map<std::string, PAYLOAD_TYPE_E> encode_type = {
        {"H.264", PT_H264},
        {"H.265", PT_H265}
    };
HardwareEncoder::HardwareEncoder(int srcWidth, int srcHeight, std::string encodeType){
    CVI_S32 ret;
    VENC_CHN_ATTR_S chnAttr = {};
    chnAttr.stVencAttr.enType = encode_type[encodeType];
    chnAttr.stVencAttr.u32PicWidth = srcWidth;
    chnAttr.stVencAttr.u32PicHeight = srcHeight;
    chnAttr.stVencAttr.u32MaxPicWidth= srcWidth;
    chnAttr.stVencAttr.u32MaxPicHeight= srcHeight;
    chnAttr.stVencAttr.bByFrame = CVI_FALSE;
    chnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
    chnAttr.stRcAttr.stH264Cbr.u32BitRate = 8000;
    chnAttr.stRcAttr.stH264Cbr.u32SrcFrameRate = 20; 
    chnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate = 20;
    ret = CVI_VENC_CreateChn(this->veChn, &chnAttr);
    if (ret != CVI_SUCCESS){
        std::cerr << "CVI_VENC_CreateChn failed: " << ret << "\n";
    }

    VENC_RECV_PIC_PARAM_S recAttr = {};
    recAttr.s32RecvPicNum = INT_MAX;
    ret = CVI_VENC_StartRecvFrame(this->veChn, &recAttr);
    if (ret !=  CVI_SUCCESS){
        std::cerr << "CVI_VENC_StartRecvFrame failed: " << ret << "\n";
    }
    
}

bool HardwareEncoder::sendFrame(const VIDEO_FRAME_INFO_S* frame){
    CVI_S32 ret = CVI_VENC_SendFrame(this->veChn, frame, 2000);
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

CVI_S32 HardwareEncoder::Send_Frame_RTSP(VIDEO_FRAME_INFO_S *frame, SAMPLE_TDL_MW_CONTEXT *pstMWContext) {
  CVI_S32 ret = CVI_SUCCESS;

  CVI_S32 s32SetFrameMilliSec = 2000;
  VENC_STREAM_S stStream;
  VENC_CHN_ATTR_S stVencChnAttr;
  VENC_CHN_STATUS_S stStat;

  VENC_PACK_S *ppack;
  CVI_RTSP_DATA data = {0};
  memset(&data, 0, sizeof(CVI_RTSP_DATA));
  ret = CVI_VENC_SendFrame(this->veChn, frame, s32SetFrameMilliSec);
    if (ret != CVI_SUCCESS) {
        std::cerr << "CVI_VENC_SendFrame failed with error: " << ret << "\n";
        return ret;
    }

  ret = CVI_VENC_GetChnAttr(this->veChn, &stVencChnAttr);
  if (ret != CVI_SUCCESS) {
    printf("CVI_VENC_GetChnAttr, VencChn[%d], ret = %d\n", this->veChn, ret);
    return ret;
  }

  ret = CVI_VENC_QueryStatus(this->veChn, &stStat);
  if (ret != CVI_SUCCESS) {
    printf("CVI_VENC_QueryStatus failed with %#x!\n", ret);
    return ret;
  }

  if (!stStat.u32CurPacks) {
    printf("NOTE: Current frame is NULL!\n");
    return ret;
  }

  stStream.pstPack = (VENC_PACK_S *)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
  if (stStream.pstPack == NULL) {
    printf("malloc memory failed!\n");
    return ret;
  }

  ret = CVI_VENC_GetStream(this->veChn, &stStream, -1);
  if (ret != CVI_SUCCESS) {
    printf("CVI_VENC_GetStream failed with %#x!\n", ret);
    goto send_failed;
  }


  data.blockCnt = stStream.u32PackCount;
  for (unsigned int i = 0; i < stStream.u32PackCount; i++) {
    ppack = &stStream.pstPack[i];
    data.dataPtr[i] = ppack->pu8Addr + ppack->u32Offset;
    data.dataLen[i] = ppack->u32Len - ppack->u32Offset;
  }

  ret =
      CVI_RTSP_WriteFrame(pstMWContext->pstRtspContext, pstMWContext->pstSession->video, &data);
  if (ret != CVI_SUCCESS) {
    printf("CVI_RTSP_WriteFrame, ret = %d\n", ret);
    goto send_failed;
  }

send_failed:
  CVI_VENC_ReleaseStream(this->veChn, &stStream);
  free(stStream.pstPack);
  stStream.pstPack = NULL;
  return ret;
}
