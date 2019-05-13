//
//  video_manager.cpp
//  multi_video_service
//
//  Created by mawei on 2019/3/4.
//  Copyright © 2019年 mawei. All rights reserved.
//
#include <unistd.h>
#include <string.h>
#include "video_manager.h"
#include "multi_context_frames.h"
#include "ip_camera.h"
#include "public_function.h"

VideoManager::VideoManager():
    multi_context_frames_(nullptr),
    reconnect_thread_(nullptr),
    reconnect_flag(false)
{
    memset(&service_param_,0,sizeof(service_param_));
    videos_.clear();
    videos_info_.clear();
}

VideoManager::~VideoManager()
{
    Uninit();
}
int32_t VideoManager::Init(service_param_t service_param)
{

    //创建断线重连线程
    reconnect_thread_ = new std::thread(&VideoManager::Reconnect,this);
    if(reconnect_thread_ != nullptr)
    {
        reconnect_flag = true;
    }

    if(service_param.service_type & E_STREAM_DECODE )
    {
        multi_context_frames_ = new MultiContextFrames();
    }

    service_param_ = service_param;

    return 0;
}
//添加相机
int32_t VideoManager::AddVideo(int32_t id,const std::string &play_url,decode_param_t decode_param,std::string &output_play_address)
{
    videos_mutex_.lock();
    auto iter = videos_.find(id);
    if(iter != videos_.end())
    {
        videos_mutex_.unlock();
        return E_ID_EXIST;
    }

    std::string input_play_address;
    if(service_param_.service_type & E_STREAM_RELAY)
    {
        int64_t millisecond = get_millisecond();

        output_play_address ="rtmp://" + std::string(service_param_.stream_server_param.output_addr) + \
                ":"+ std::to_string(service_param_.stream_server_param.output_port) + "/" + \
                std::to_string(id) + "/normal/" + std::to_string(millisecond);
        output_play_address ="rtmp://localhost/live/test";
        input_play_address = "rtmp://" + std::string(service_param_.stream_server_param.input_addr) + \
                ":"+ std::to_string(service_param_.stream_server_param.input_port) + "/" + \
                std::to_string(id) + "/normal/" + std::to_string(millisecond);
        input_play_address = "rtmp://localhost/live/test";
    }


    IPCamera *ip_camera = new IPCamera(id,multi_context_frames_,service_param_.service_type,play_url,\
                                       decode_param,input_play_address);

    videos_[id] = ip_camera;
    videos_mutex_.unlock();
    video_info_t video_info;

    //ip_camera->GetvideoInfo(video_info);


    return ip_camera->Open();
}

//移除相机
int32_t VideoManager::RemoveVideo(int32_t id)
{
    IPCamera *ip_video = nullptr;

    videos_mutex_.lock();
    auto iter = videos_.find(id);
    if(iter != videos_.end())
    {
        ip_video = iter->second;
        videos_.erase(iter);
    }
    videos_mutex_.unlock();

    if(ip_video != nullptr)
    {
        delete ip_video;
    }

    return E_SUCCESS;
}

//获取所有相机状态
int32_t VideoManager::GetVideosInfo(std::map<int32_t,video_info_t> &videos_info)
{
    videos_mutex_.lock();
    videos_info = videos_info_;
    videos_mutex_.unlock();

    return E_SUCCESS;
}

//获取单个相机状态
int32_t VideoManager::GetVideoInfo(int32_t id,video_info_t& video_info)
{
    videos_mutex_.lock();
    auto iter = videos_.find(id);
    if (iter!= videos_.end()) {
        IPCamera *ip_video = iter->second;
        ip_video->GetvideoInfo(video_info);
    }
    videos_mutex_.unlock();

    return E_SUCCESS;
}

//获取所有相机的一帧数据
int32_t VideoManager::GetVideosOneFrame(std::map<int32_t,video_frame_t*> &videos_frame)
{
    multi_context_frames_->GetContextsOneFrame(videos_frame);
    return E_SUCCESS;
}

//添加编码的视频数据
int32_t VideoManager::AddEncodeFrame(int32_t id,video_frame_t* frame)
{
    videos_mutex_.lock();
    auto iter = videos_.find(id);
    if (iter!= videos_.end()) {
        IPCamera *ip_video = iter->second;
        ip_video->AddEncodeFrame(frame);
    }
    videos_mutex_.unlock();

    return E_SUCCESS;
}

//开始编码
int32_t VideoManager::StartEncode(int32_t id,encode_param_t encode_param,std::string &play_back_url)
{
    videos_mutex_.lock();
    auto iter = videos_.find(id);
    if (iter!= videos_.end()) {
        IPCamera *ip_video = iter->second;





        ip_video->StartEncode(encode_param,play_back_url);
    }
    videos_mutex_.unlock();
    return 0;
}

//停止编码
int32_t VideoManager::StopEncode(int32_t id)
{
    return 0;
}


int32_t VideoManager::Uninit()
{
    videos_mutex_.lock();
    auto iter = videos_.begin();
    for (; iter != videos_.end(); ++iter) {
        IPCamera *ip_video = iter->second;
        ip_video->Close();
        delete ip_video;
    }
    videos_.clear();
    videos_mutex_.unlock();

    if(reconnect_thread_ != nullptr)
    {
        reconnect_flag = false;
        reconnect_thread_->join();
        delete reconnect_thread_;
        reconnect_thread_ = nullptr;
    }

    if(multi_context_frames_ != nullptr)
    {
        delete multi_context_frames_;
        multi_context_frames_ = nullptr;
    }

    return 0;
}


void VideoManager::Reconnect()
{
    while (reconnect_flag) {

        std::map<int32_t,video_info_t> videos_info;
        GetVideosInfoCache(videos_info);

        videos_info_mutex_.lock();
        videos_info_ = videos_info;
        videos_info_mutex_.unlock();

        if(service_param_.video_auto_reconnect)
        {
            auto it = videos_info.begin();
            for(;it != videos_info.end();++it)
            {
                video_info_t video_info = it->second;
                if(video_info.status != E_NORMAL && video_info.status != E_WAITING)
                {
                    printf("id:%d,not normal.\n",it->first);
                    videos_mutex_.lock();
                    IPCamera *ip_video = videos_[it->first];
                    videos_mutex_.unlock();

                    ip_video->Close();
                    ip_video->Open();
                }
            }
        }

        //1s
        sleep(1);

    }
}


//获取所有相机状态
int32_t VideoManager::GetVideosInfoCache(std::map<int32_t,video_info_t> &videos_info)
{
    videos_mutex_.lock();
    auto iter = videos_.begin();
    for (; iter != videos_.end(); ++iter) {
        IPCamera *ip_video = iter->second;
        video_info_t video_info;
        ip_video->GetvideoInfo(video_info);
        videos_info[iter->first] = video_info;
    }
    videos_mutex_.unlock();

    return E_SUCCESS;
}
