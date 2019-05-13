//
//  ip_video.h
//  multi_video_service
//
//  Created by mawei on 2019/2/21.
//  Copyright © 2019年 mawei. All rights reserved.
//

#ifndef IP_video_H
#define IP_video_H

#include <iostream>
#include "multi_video_service_define.h"

class VideoDecode;
class VideoEncode;

class MultiContextFrames;
class ContextFrames;
class  IPCamera
{
public:
    IPCamera(int32_t id,MultiContextFrames *multi_contex_frames,const service_type_t &service_type,\
             const std::string &source_url,const decode_param_t &decode_param,\
             const std::string &input_play_address);
    ~IPCamera();
    int32_t Open();
    int32_t Close();
    int32_t GetvideoInfo(video_info_t &video_info);
    int32_t AddEncodeFrame(video_frame_t *video_frame);
    int32_t StartEncode(const encode_param_t &encode_param,std::string &play_back_url);
    int32_t StopEncode();
public:
    service_type_t service_type_;
private:
    int32_t id_;
    MultiContextFrames *multi_contex_decode_frames_;//解码后的数据
 //    service_type_t service_type_;

    decode_param_t decode_param_;                   //解码参数
    VideoDecode *video_decode_;                     //解码类

    ContextFrames *context_frames_encode_;          //用于编码的数据
    VideoEncode *video_encode_;                     //编码类

    std::string source_url_;
    std::string input_play_address_;


};
#endif /* IP_video_H */
