//
//  sample.cpp
//  multi_video_service
//
//  Created by mawei on 2019/2/21.
//  Copyright © 2019年 mawei. All rights reserved.
//

#include <iostream>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

#include "multi_video_service.h"
#include "public_function.hpp"

void InitSignalHandler()
{
    //signal(SIGINT,&handle);
    //signal(SIGSEGV,&handle);
    signal(SIGPIPE,SIG_IGN);
    //signal(SIGPIPE,handle);
    //signal(SIGABRT,&handle);
}



int main(int argc, const char * argv[]) {
    // insert code here...

    if(argc < 2)
    {
        printf("miss config file\n");
        return 0;
    }
    InitSignalHandler();
    //std::map<int32_t,DeviceInfo> devices;
    SystemConfig system_config;
    if(!parse_devices_config(argv[1],system_config))
    {
        printf("parse config error\n");
        return 0;
    }


 /////////////////////////////////////////////////////////////////////////////
    service_param_t service_param;
    memset(&service_param,0,sizeof(service_param));
    service_param.service_type = E_STREAM_DECODE_ENCODE_RELAY;//all
    strcpy(service_param.stream_server_param.input_addr,system_config.stream_server_info.input_addr.c_str());

    service_param.stream_server_param.input_port = system_config.stream_server_info.input_port;

    strcpy(service_param.stream_server_param.output_addr,system_config.stream_server_info.output_addr.c_str());

    service_param.stream_server_param.output_port = system_config.stream_server_info.output_port;

    service_param.video_auto_reconnect = true;

    multi_video_service_handle handle = multi_video_service_create(service_param);


    const int32_t address_len = 256;

    char *output_play_address = (char*)malloc(sizeof(char)*address_len);


    for(auto it = system_config.devices.begin();it != system_config.devices.end();++it)
    {
        decode_param_t decode_param;
        decode_param.resize = true;
        //decode_param.resize = true;
        decode_param.resize_height =system_config.window_size.height;
        decode_param.resize_width =system_config.window_size.width;
        //decode_param.fmt = VIDEO_YUV420P;
        //decode_param.fmt = VIDEO_RGB;
        decode_param.fmt = VIDEO_BGR;

        DeviceInfo device_info = it->second;
        memset(output_play_address,0,address_len);
        multi_video_service_add_video(handle,device_info.id, device_info.play_url.c_str(),decode_param,(char**)&output_play_address);

        printf("output_play_address:%s\n",output_play_address);
    }

    int fps = 0;
    while(1)//用于释放缓存？
    {
        int32_t *ids = NULL;
        video_frame_t* *frames = NULL;
        int32_t video_count = 0;

        multi_video_service_get_videos_one_frame(handle,&ids,&frames,&video_count);

        if(video_count == 0)
        {
            usleep(10*1000);
            continue;
        }
        printf("video_count:%d\n",video_count);

        for(int i = 0; i < video_count; ++i)
        {
              video_frame_t *video_frame = frames[i];

              if(video_frame != nullptr)
              {
                  printf("id:%d,pts:%lld,video_frame:%p,(%d,%d)\n",ids[i],video_frame->pts,video_frame,\
                         video_frame->width,video_frame->height);

                  multi_video_service_free_video_frame(video_frame);
              }
        }
        if(ids != NULL)
        {
            free(ids);
            ids = NULL;
        }

        if(frames != NULL)
        {
            free(frames);
            frames = NULL;
        }

        video_count = 0;
        video_info_t *videos_info = nullptr;
        multi_video_service_get_videos_info(handle,&ids,&videos_info,&video_count);
        for(int i = 0; i < video_count; ++i)
        {
              video_info_t video_info = videos_info[i];

              printf("id:%d,status:%d,fps:%lld,(%d,%d,%d)\n",ids[i],video_info.status,video_info.fps,video_info.width,\
                     video_info.height,video_info.stride);

              fps = video_info.fps;
        }

        if(ids != NULL)
        {
            free(ids);
            ids = NULL;
        }

        if (videos_info != nullptr) {
            free(videos_info);
            videos_info = nullptr;
        }
        if (fps != 0) {
            usleep(1000/fps*1000);
        } else {
            usleep(40*1000);
        }
    }

    getchar();

    free(output_play_address);
    multi_video_service_destory(handle);

    return 0;
}
