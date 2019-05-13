//
//  video_encode.h
//  multi_video_service
//  编码类，支持转发功能
//  Created by mawei on 2019/3/4.
//  Copyright © 2019年 mawei. All rights reserved.
//

#ifndef video_encode_h
#define video_encode_h

#include <iostream>
#include <thread>

#include "multi_video_service_define.h"

class ContextFrames;
class VideoEncode{
public:
    VideoEncode(ContextFrames *context_frames);
    ~VideoEncode();

    //添加流媒体转发对象
    int32_t Start(encode_param_t encode_param,std::string &play_back_url);

    int32_t Stop();

    int32_t Status();
private:

    void Encode();

private:
    ContextFrames *context_frames_;

    encode_param_t encode_param_;
    std::string play_back_url_;

    std::thread *encode_thread_;
    bool encode_flag_;
};

#endif /* video_encode_h */
