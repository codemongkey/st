//
//  multi_context_frames.h
//  multi_video_service
//  用于解码类，保存多个相机的视频帧
//  Created by mawei on 2019/2/21.
//  Copyright © 2019年 mawei. All rights reserved.
//

#ifndef multi_context_frames_h
#define multi_context_frames_h

#include <stdio.h>
#include <map>
#include <deque>
#include <mutex>

#include "multi_video_service_define.h"

class MultiContextFrames{
public:
    MultiContextFrames();
    ~MultiContextFrames();

    void AddContext(int32_t id,video_info_t &video_info);
    int32_t AddFrame(int32_t id,video_frame_t *frame);

    int32_t GetContextsOneFrame(std::map<int32_t,video_frame_t*> &contexts_frame);
    void RecycleFrame(video_frame_t *frame);

    int32_t RemoveContext(int32_t id);
private:
    void AllocFrames();

    std::mutex contexts_frames_mutex_;
    std::map<int32_t,std::deque<video_frame_t*>> contexts_frames_;

    std::deque<video_frame_t*> frame_pool_;

};

#endif /* multi_context_frames_h */
