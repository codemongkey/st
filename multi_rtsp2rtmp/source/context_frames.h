//
//  context_frames.hpp
//  multi_video_service
//  用于编码类，保存单个相机的视频帧
//  Created by mawei on 2019/2/21.
//  Copyright © 2019年 mawei. All rights reserved.
//

#ifndef context_frames_h
#define context_frames_h

#include <stdio.h>
#include <map>
#include <deque>
#include <mutex>

#include "multi_video_service_define.h"

class ContextFrames{

public:
    ContextFrames();
    ~ContextFrames();

    int32_t Init(int32_t width,int32_t height,int32_t buffer_size);

    int32_t AddFrame(const video_frame_t* frame);
    int32_t GetFrame(video_frame_t* &frame);
    int32_t RecycleFrame(video_frame_t* frame);
private:
    int32_t AllocFrames();
    video_frame_t* GetUnUsedFrame();

private:

    std::mutex frame_pool_mutex_;
    //存储预先分配的资源
    std::deque<video_frame_t*> frame_pool_;

    std::mutex frames_mutex_;
    std::deque<video_frame_t*> frames_;


    int32_t frame_width_;
    int32_t frame_height_;
    int32_t buffer_size_;
};

#endif /* context_frames_h */
