//
//  multi_context_frames.cpp
//  multi_video_service
//
//  Created by mawei on 2019/2/21.
//  Copyright © 2019年 mawei. All rights reserved.
//

#include "multi_context_frames.h"

const int context_max_frames = 10;

MultiContextFrames::MultiContextFrames()
{

}

MultiContextFrames::~MultiContextFrames()
{

}

void MultiContextFrames::AddContext(int32_t id,video_info_t &video_info)
{
    std::deque<video_frame_t*> frames;

    contexts_frames_mutex_.lock();
    if(contexts_frames_.find(id) == contexts_frames_.end())
        contexts_frames_[id] = frames;
    contexts_frames_mutex_.unlock();
}

int32_t MultiContextFrames::AddFrame(int32_t id,video_frame_t *frame)
{
    //std::lock_guard<std::mutex> lock(contexts_frames_mutex_);

    contexts_frames_mutex_.lock();
    auto it = contexts_frames_.find(id);
    if( it != contexts_frames_.end())
    {
        if(it->second.size() > context_max_frames)
        {
            //remove old frame
            RecycleFrame(it->second.front());
            it->second.pop_front();
            //printf("id:%d,count(%lu),drop one frame\n",id,it->second.size());
        }
        it->second.push_back(frame);;
    }
    contexts_frames_mutex_.unlock();

    return 0;
}

int32_t MultiContextFrames::GetContextsOneFrame(std::map<int32_t,video_frame_t*> &contexts_frame)
{
    contexts_frame.clear();

    contexts_frames_mutex_.lock();
    auto it = contexts_frames_.begin();
    for(;it != contexts_frames_.end();++it)
    {
        //printf("id:%d,framesize:%lu\n",it->first,it->second.size());
        if(it->second.size() != 0)
        {
            contexts_frame[it->first] = it->second.front();
            it->second.pop_front();
        }
    }

    contexts_frames_mutex_.unlock();

    return 0;
}

void MultiContextFrames::RecycleFrame(video_frame_t *frame)
{
    if(frame != nullptr)
    {
        if(frame->buffer != nullptr)
        {
            free(frame->buffer);
            frame->buffer = nullptr;
        }
    }
}

int32_t MultiContextFrames::RemoveContext(int32_t id)
{
    contexts_frames_mutex_.lock();
    std::deque<video_frame_t*> frames = contexts_frames_[id];
    while(frames.size() != 0)
    {
        video_frame_t *frame = frames.front();
        RecycleFrame(frame);
        delete frame;
        frames.pop_front();
    }
    contexts_frames_.erase(id);
    contexts_frames_mutex_.unlock();
}

void MultiContextFrames::AllocFrames()
{

}
