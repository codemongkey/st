//
//  ip_video.cpp
//  multi_video_service
//
//  Created by mawei on 2019/2/21.
//  Copyright © 2019年 mawei. All rights reserved.
//

#include "ip_camera.h"
#include "multi_context_frames.h"
#include "context_frames.h"
#include "video_decode.h"
#include "video_encode.h"


IPCamera::IPCamera(int32_t id, MultiContextFrames *multi_contex_frames,const service_type_t &service_type,\
                   const std::string &source_url,const decode_param_t &decode_param,\
                   const std::string &input_play_address) :
    id_(id),
    multi_contex_decode_frames_(multi_contex_frames),
    service_type_(service_type),
    video_decode_(nullptr),
    context_frames_encode_(nullptr),
    video_encode_(nullptr),
    source_url_(source_url),
    decode_param_(decode_param),
    input_play_address_(input_play_address)
{

}

IPCamera::~IPCamera()
{

}
int32_t IPCamera::Open()
{
    if(video_decode_ == nullptr)
    {
        video_decode_ = new VideoDecode(id_,multi_contex_decode_frames_,service_type_);
    }

    return video_decode_->Start(source_url_,decode_param_,input_play_address_);
}
int32_t IPCamera::Close()
{
    if(video_decode_ != nullptr)
    {
        video_decode_->Stop();
    }
    return 0;
}
int32_t IPCamera::GetvideoInfo(video_info_t &video_info)
{
    if(video_decode_ != nullptr)
    {
        video_info = video_decode_->Info();
    }
    return 0;
}
int32_t IPCamera::AddEncodeFrame(video_frame_t *video_frame)
{
    if(context_frames_encode_ != nullptr)
    {
        context_frames_encode_->AddFrame(video_frame);
    }
    return 0;
}
int32_t IPCamera::StartEncode(const encode_param_t &encode_param,std::string &play_back_url)
{
    context_frames_encode_ = new ContextFrames();
    video_encode_ = new VideoEncode(context_frames_encode_);
    video_encode_->Start(encode_param, play_back_url);

    return 0;
}
int32_t IPCamera::StopEncode()
{
    return 0;
}
