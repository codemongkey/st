//
//  context_frames.cpp
//  multi_video_service
//
//  Created by mawei on 2019/2/21.
//  Copyright © 2019年 mawei. All rights reserved.
//

#include "context_frames.h"


ContextFrames::ContextFrames()
{}
ContextFrames::~ContextFrames()
{}
int32_t ContextFrames::Init(int32_t width,int32_t height,int32_t buffer_size)
{
    frame_width_ = width;
    frame_height_= height;
    buffer_size_ = buffer_size;

    AllocFrames();
    return 0;
}
int32_t ContextFrames::AddFrame(const video_frame_t* frame)
{
    return 0;
}
int32_t ContextFrames::GetFrame(video_frame_t* &frame)
{
    return 0;
}
int32_t ContextFrames::RecycleFrame(video_frame_t* frame)
{
    return 0;
}
int32_t ContextFrames::AllocFrames()
{
    for (int32_t i = 0; i<10; i++) {
        video_frame_t* frame = (video_frame_t*)malloc(sizeof(video_frame_t));
        if(frame != NULL)
        {
            frame->width = frame_width_;
            frame->height = frame_height_;
            frame->buffer = malloc(frame->stride*frame->height);
        }
    }

    return 0;
}

video_frame_t* ContextFrames::GetUnUsedFrame()
{
    video_frame_t* video_frame_t = nullptr;
    frame_pool_mutex_.lock();

    if(1)

    frame_pool_mutex_.unlock();

    return video_frame_t;

}
