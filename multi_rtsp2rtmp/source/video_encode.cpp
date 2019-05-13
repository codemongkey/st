//
//  video_encode.cpp
//  multi_video_service
//
//  Created by mawei on 2019/3/4.
//  Copyright © 2019年 mawei. All rights reserved.
//

#include "video_encode.h"
VideoEncode::VideoEncode(ContextFrames *context_frames):
    context_frames_(context_frames)
{

}
VideoEncode::~VideoEncode()
{
}

//添加流媒体转发对象
int32_t VideoEncode::Start(encode_param_t encode_param,std::string &play_back_url)
{
    encode_param_ = encode_param;
    play_back_url_ = play_back_url;
    return 0;
}

void VideoEncode::Encode()
{
}

int32_t VideoEncode::Stop()
{
    return 0;
}

int32_t VideoEncode::Status()
{
    return 0;
}
