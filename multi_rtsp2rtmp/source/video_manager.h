//
//  video_manager.h
//  multi_video_service
//
//  Created by mawei on 2019/3/4.
//  Copyright © 2019年 mawei. All rights reserved.
//

#ifndef video_manager_h
#define video_manager_h

#include <iostream>
#include <map>
#include <mutex>
#include <thread>

#include "multi_video_service_define.h"

class IPCamera;
class MultiContextFrames;
struct interface{
    bool push_flag;
    bool play_flag;
    bool use_gpu;
    int time_limit;//秒
    bool shake_again=false;
    int time_count=0;
    interface(bool push_flag=true,bool play_flag=true,bool use_gpu=true,int time_limit=20):
              push_flag(push_flag),play_flag(play_flag),use_gpu(use_gpu),time_limit(time_limit)
    {};
};
class VideoManager
{
public:
    VideoManager();
    ~VideoManager();

    int32_t Init(service_param_t service_param);

    //添加相机
    int32_t AddVideo(int32_t id,const std::string &source_url,decode_param_t decode_param,std::string &out_play_url);

    //移除相机
    int32_t RemoveVideo(int32_t id);

    //获取所有相机状态
    int32_t GetVideosInfo(std::map<int32_t,video_info_t> &videos_info);

    //获取单个相机状态
    int32_t GetVideoInfo(int32_t id,video_info_t &video_info);

    //获取所有相机的一帧数据
    int32_t GetVideosOneFrame(std::map<int32_t,video_frame_t*> &videos_frame);

    //回收视频资源
    int32_t RecycleFrame(int32_t id,video_frame_t* frame);

    //初始化编码参数
    int32_t InitEncodeParam(int32_t id,encode_param_t encode_param);

    //添加编码的视频数据
    int32_t AddEncodeFrame(int32_t id,video_frame_t* frame);

    //开始编码
    int32_t StartEncode(int32_t id,encode_param_t encode_param,std::string &play_back_url);

    //停止编码
    int32_t StopEncode(int32_t id);

    int32_t Uninit();
    //获取相机数组
    std::map<int32_t,IPCamera*>& getVideos(){
        return videos_;
    }


private:
    //相机重连函数
    void Reconnect();
    //获取所有相机状态
    int32_t GetVideosInfoCache(std::map<int32_t,video_info_t> &videos_info);
private:

    service_param_t service_param_;

    std::mutex videos_mutex_;
    std::map<int32_t,IPCamera*> videos_;

    MultiContextFrames *multi_context_frames_;

    std::thread *reconnect_thread_;
    bool reconnect_flag; //true 重连，falsec 重连结束

    std::mutex videos_info_mutex_;
    std::map<int32_t,video_info_t> videos_info_;
};




#endif /* video_manager_h */
