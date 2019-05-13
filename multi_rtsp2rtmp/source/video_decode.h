//
//  video_decode.h
//  multi_video_service
//  解码类，支持视频接入，转发，转码功能
//  Created by mawei on 2019/3/4.
//  Copyright © 2019年 mawei. All rights reserved.
//

#ifndef video_decode_h
#define video_decode_h

#include <iostream>
#include <thread>

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <SDL2/SDL.h>
#ifdef __cplusplus
}
#endif
#include "srs_librtmp/srs_librtmp.h"
#include "multi_video_service_define.h"

class MultiContextFrames;

class VideoDecode{
public:
    VideoDecode(int32_t id,MultiContextFrames *multi_context_frames,service_type_t service_type);
    ~VideoDecode();

    int32_t Start(const std::string &source_url,const decode_param_t &decode_param,const std::string &input_play_address);

    int32_t Stop();

    video_info_t Info();

private:
    void Decode();
    void VideoFormat2AvPixelFormat(video_format_t &vi_format, AVPixelFormat &av_format);
private:
    int32_t id_;
    MultiContextFrames *multi_context_frames_;
    service_type_t service_type_;
    std::string source_url_;

    decode_param_t decode_param_;
    srs_rtmp_t rtmp_client_;
    std::string input_play_address_;

    int32_t video_index_;

    bool decode_flag_;            //解码标志，true-进行中，false-退出
    std::thread *decode_thread_;

    AVFormatContext	*decode_format_ctx_;    //ffmpeg格式容器
    AVCodecContext	*decode_codec_ctx_;     //ffmpeg解码器

    video_info_t video_info_;
};

#endif /* video_decode_h */
