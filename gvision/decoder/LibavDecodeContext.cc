//
// Created by CuongTV on 18/01/2025.
// Copyright (c) GreenStreamVision
//

#include "LibavDecodeContext.h"
#include <iostream>
#include <opencv2/opencv.hpp>

namespace gvision {
    LibavDecodeContext::LibavDecodeContext(const std::string uri, bool enableDetectMotion) {
        mUri = uri;
        mEnableMotionDetection = enableDetectMotion;

    }

    LibavDecodeContext::~LibavDecodeContext() {
        avcodec_close(pCodecCtx);
        avformat_close_input(&pFormatCtx);
        sws_freeContext(conversion);
    }

    bool LibavDecodeContext::isVideoPkt(AVPacket *packet) {
        return packet->stream_index == videoStreamIndex;
    }

    bool LibavDecodeContext::init() {

        AVDictionary *m_opts{nullptr};
        av_dict_set(&m_opts, "rw_timeout", "3000000", 0);
        av_dict_set(&m_opts, "stimeout", "3000000", 0);
        av_dict_set(&m_opts, "rtsp_transport", "tcp", 0);
        pFormatCtx = avformat_alloc_context();

        if (mEnableMotionDetection) {
            av_dict_set(&m_opts, "flags2", "+export_mvs", 0);
        }
        if (avformat_open_input(&pFormatCtx, mUri.c_str(), nullptr, &m_opts) < 0) {
            avformat_close_input(&pFormatCtx);
            return false;
        }
        if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
            avformat_close_input(&pFormatCtx);
            return false;
        }
        for (int i = 0; i < pFormatCtx->nb_streams; i++) {
            if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoStreamIndex = i;
                break;
            }
        }
        pCodecCtx = avcodec_alloc_context3(nullptr);
        AVRational frameRate = av_guess_frame_rate(pFormatCtx, pFormatCtx->streams[videoStreamIndex], nullptr);

        fps = (int) (frameRate.num / frameRate.den);
        if (avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStreamIndex]->codecpar) < 0) {
            avformat_close_input(&pFormatCtx);
            avcodec_close(pCodecCtx);
            return false;
        }
        const AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
        if (pCodec == nullptr) {
            avformat_close_input(&pFormatCtx);
            avcodec_close(pCodecCtx);
            return false;
        }
        if (avcodec_open2(pCodecCtx, pCodec, &m_opts) < 0) {
            avcodec_close(pCodecCtx);
            avformat_close_input(&pFormatCtx);
            return false;
        }
        //
        codecName = pCodec->name;
        width = pCodecCtx->width;
        height = pCodecCtx->height;

        // Log or store the information
        std::cout << "Codec: " << codecName << std::endl;
        std::cout << "Resolution: " << width << "x" << height << std::endl;
        std::cout << "FPS: " << fps << std::endl;
        
		conversion = sws_getContext(width, height, AV_PIX_FMT_YUV420P,
                                    width, height, AV_PIX_FMT_BGR24,
                                    SWS_BILINEAR, nullptr, nullptr, nullptr);
        
		return true;
    }

    int LibavDecodeContext::sendPkt(AVPacket *p_packet) {
        return avcodec_send_packet(pCodecCtx, p_packet);
    }

    bool LibavDecodeContext::decodeAvFrame(AVFrame *p_frame) {
        return avcodec_receive_frame(pCodecCtx, p_frame) >= 0;
    }

    cv::Mat LibavDecodeContext::avFrame2cvMat(AVFrame *frame) {
        cv::Mat cvFrame(frame->height,
                        frame->width,
                        CV_8UC3,
                        cv::Scalar(0, 0, 0));
        int cvSizes[1];
        cvSizes[0] = (int) cvFrame.step1();
        sws_scale(conversion, frame->data, frame->linesize, 0, frame->height, &cvFrame.data, cvSizes);
        return cvFrame;
    }
}
