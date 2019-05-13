//
//  sample.cpp
//  multi_video_service
//
//  Created by mawei on 2019/2/21.
//  Copyright © 2019年 mawei. All rights reserved.
//
#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include<thread>
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <SDL2/SDL.h>
#ifdef __cplusplus
}
#endif

#include "source/video_manager.h"
#include "multi_video_service.h"
#include "public_function.hpp"
#include <sys/time.h>


#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)

#define SFM_BREAK_EVENT  (SDL_USEREVENT + 2)

int thread_exit = 0;
int thread_pause = 0;

std::map<int,interface> video_config;
int sfp_refresh_thread (void *opaque) {//刷新
    thread_exit = 0;
    thread_pause = 0;
    while (!thread_exit) {
        if (!thread_pause) {
            SDL_Event event;
            event.type = SFM_REFRESH_EVENT;
            SDL_PushEvent(&event);
        }
        SDL_Delay(40);
    }
    thread_exit = 0;
    thread_pause = 0;
    //Break
    SDL_Event event;
    event.type = SFM_BREAK_EVENT;
    SDL_PushEvent(&event);
    return 0;
}
void sigalrm_fn(int sig)
{
    for (auto it = video_config.begin(); it != video_config.end() ; ++it) {
        it->second.time_count+=1;
        if(it->second.time_limit<=it->second.time_count)
        {
            it->second.time_count  = 0;
            it->second.push_flag   = !it->second.push_flag;
            if(it->second.time_limit>4)//5秒不publish srs服务器会clean，断开连接
                it->second.shake_again = true;
            std::cout<<"-----------------------------------change"<<std::endl;
            it->second.time_limit += 1;
        }
    }
    alarm(1);
    return;
}
void InitSignalHandler() {
    //signal(SIGINT,&handle);
    //signal(SIGSEGV,&handle);
    signal(SIGPIPE, SIG_IGN);
    //signal(SIGPIPE,handle);
    //signal(SIGABRT,&handle);
}
using namespace std;
void func1(std::map<int32_t,DeviceInfo>::iterator it){

    usleep(6000 * 1000);
    while(1)
    {
        if(it->second.push_flag==0){

        }
    }
}

void PushThread(multi_video_service_handle handle,SystemConfig& system_config) {
    std::cout<<endl;
}
int main(int argc,  char * argv[]) {
    // insert code here...    
    if (argc < 2) {
        printf("miss config file\n");
        return 0;
    }

    //std::map<int32_t,DeviceInfo> devices;
    SystemConfig system_config;
    if (!parse_devices_config(argv[1], system_config)) {
        printf("parse config error\n");
        return 0;
    }

    //初始化接口配置
    for(auto it=system_config.devices.begin();it!=system_config.devices.end();++it){
        video_config[it->first]=interface(it->second.push_flag,it->second.play_flag,it->second.use_gpu,it->second.time);
    }
    /////////////////////////////////////////////////////////////////////////////
    int screen_w, screen_h;
    if (system_config.window_size.width > 0 && system_config.window_size.height > 0) {
        screen_w = system_config.window_size.width;
        screen_h = system_config.window_size.height;
    } else {
        screen_w = 1920;
        screen_h = 1080;
    }
    video_size_t video_size;

    //video_format_t format = VIDEO_RGB;
    //video_format_t format = VIDEO_YUV420P;

    SDL_Window *screen;
    SDL_Renderer* sdlRenderer;

    SDL_Thread *video_tid;
    SDL_Event event;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        printf( "Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }
    screen = SDL_CreateWindow("sample", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              screen_w, screen_h, SDL_WINDOW_OPENGL);

    if (!screen) {
        printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
        return -1;
    }
    sdlRenderer = SDL_CreateRenderer(screen, -1, 0);

    std::map<int32_t, SDL_Rect> sdl_rects;
    std::map<int32_t, SDL_Texture*> sdlTextures;



/////////////////////////////////////////////////////////////////////////////
    service_param_t service_param;
    memset(&service_param, 0, sizeof(service_param));
    service_param.service_type = E_STREAM_DECODE_ENCODE_RELAY;//all
    strcpy(service_param.stream_server_param.input_addr, system_config.stream_server_info.input_addr.c_str());

    service_param.stream_server_param.input_port = system_config.stream_server_info.input_port;

    strcpy(service_param.stream_server_param.output_addr, system_config.stream_server_info.output_addr.c_str());

    service_param.stream_server_param.output_port = system_config.stream_server_info.output_port;

    service_param.video_auto_reconnect = true;

    multi_video_service_handle handle = multi_video_service_create(service_param);

    const int32_t address_len = 256;

    char *output_play_address = (char*)malloc(sizeof(char) * address_len);

    int i = 0;

    size_t width_count = sqrt( system_config.devices.size() - 1) + 1;

    for(auto it = system_config.devices.begin(); it != system_config.devices.end(); ++it) {
        SDL_Rect sdlRect;
        if (system_config.devices.size() <= 1) {
            sdlRect.x = 0;
            sdlRect.y = 0;
            sdlRect.w = screen_w;
            sdlRect.h = screen_h;

            video_size.width = screen_w;
            video_size.height = screen_h;
        }
        else {
            sdlRect.x = (i % width_count) * screen_w / width_count;
            sdlRect.y = (i / width_count) * screen_h / width_count;
            sdlRect.w = screen_w / width_count;
            sdlRect.h = screen_h / width_count;

            video_size.width = screen_w / width_count;
            video_size.height = screen_h / width_count;
        }

        sdl_rects[it->first] = sdlRect;

        SDL_Texture* sdlTexture;

        decode_param_t decode_param;
        decode_param.resize = true;
        decode_param.resize_height = video_size.height;
        decode_param.resize_width = video_size.width;
        //decode_param.fmt = VIDEO_YUV420P;
        //decode_param.fmt = VIDEO_RGB;

        //"rtsp://admin:admin123@10.5.4.171"
        decode_param.fmt = VIDEO_BGR;

        if (decode_param.fmt == VIDEO_RGB) {
            sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, video_size.width, video_size.height);
        }
        if (decode_param.fmt == VIDEO_BGR) {
            sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_BGR888, SDL_TEXTUREACCESS_STREAMING, video_size.width, video_size.height);
        }
        if (decode_param.fmt == VIDEO_YUV420P) {
            sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, video_size.width, video_size.height);
        }

        sdlTextures[it->first] = sdlTexture;

        i++;

        DeviceInfo device_info = it->second;
        memset(output_play_address, 0, address_len);
        multi_video_service_add_video(handle, device_info.id, device_info.play_url.c_str(), decode_param, (char**)&output_play_address);
        printf("\033[40;31moutput_play_address:%s\n\033[0m", output_play_address);
    }

    signal(SIGALRM, sigalrm_fn);
    alarm(1);

    //IYUV: Y + U + V  (3 planes)
    //YV12: Y + V + U  (3 planes)
    //PushThread(handle,system_config);
    video_tid = SDL_CreateThread(sfp_refresh_thread, NULL, NULL);

    int32_t count = 0;
    int32_t count1 = 0;
    while(1){};
    while(1) {
        SDL_WaitEvent(&event);
        if(event.type == SFM_REFRESH_EVENT) {
            int32_t *ids = NULL;
            video_frame_t * *frames = NULL;
            int32_t video_count = 0;

            multi_video_service_get_videos_one_frame(handle, &ids, &frames, &video_count);

            if(video_count == 0) {
                usleep(10 * 1000);
                continue;
            }
           // printf("video_count:%d\n", video_count);

            //add test
            ++count;

            if (count == 100) {                
                count = 0;
                video_frame_t *frame = nullptr;
                char *file_name = nullptr;
                do {
                    int32_t ret = multi_video_service_get_video_one_frame(handle, ids[0], &frame);
                    if(ret != 0) {
                        printf("multi_video_service_get_video_one_frame error\n");
                        break;
                    }
                    ret = multi_video_service_save_jpeg(frame, "tmp", &file_name);
                    if(ret != 0) {
                        printf("multi_video_service_save_jpeg error\n");
                        break;
                    }
                    printf("save jpeg file %s\n",file_name);
                } while(0);
                free(file_name);
                free(frame);
            }

            SDL_RenderClear( sdlRenderer );

            for(int i = 0; i < video_count; ++i) {
                video_frame_t *video_frame = frames[i];

                if(video_frame != nullptr) {

                    /*printf("id:%d,pts:%lld,video_frame:%p,(%d,%d)\n", ids[i], video_frame->pts, video_frame, \
                         video_frame->width, video_frame->height);*/
                    //if yuv 420
                    if(video_frame->fmt == VIDEO_YUV420P) {
                        SDL_UpdateTexture( sdlTextures[ids[i]], NULL, video_frame->buffer, video_frame->stride );
                    }
                    if(video_frame->fmt == VIDEO_BGR || video_frame->fmt == VIDEO_RGB) {
                        unsigned char buffer_convert[video_frame->width * video_frame->height * 4];
                        CONVERT_24to32((unsigned char*)video_frame->buffer, buffer_convert, video_frame->width, video_frame->height);
                        SDL_UpdateTexture( sdlTextures[ids[i]], NULL, buffer_convert, video_frame->width * 4 );

                    }

                    SDL_RenderCopy(sdlRenderer, sdlTextures[ids[i]], NULL, &sdl_rects[ids[i]]);

                    multi_video_service_free_video_frame(video_frame);
                }

                //multi_video_service_get_videos_info(&video_frame);
            }

            if(ids != NULL) {
                free(ids);
                ids = NULL;
            }

            if(frames != NULL) {
                free(frames);
                frames = NULL;
            }

            video_count = 0;
            video_info_t *videos_info = nullptr;
            multi_video_service_get_videos_info(handle, &ids, &videos_info, &video_count);
            for (int i = 0; i < video_count; ++i) {
                video_info_t video_info = videos_info[i];

                /*printf("id:%d,status:%d,fps:%lld,(%d,%d,%d)\n", ids[i], video_info.status, video_info.fps, video_info.width, \
                       video_info.height, video_info.stride);*/
            }

            if(ids != NULL) {
                free(ids);
                ids = NULL;
            }

            if(videos_info != nullptr) {
                free(videos_info);
                videos_info = nullptr;
            }

            SDL_RenderPresent( sdlRenderer );


        } else if(event.type == SDL_QUIT) {
            cout<<"quit1"<<endl;
            thread_exit = 1;
        } else if(event.type == SFM_BREAK_EVENT) {
            cout<<"quit2"<<endl;
            break;
        }
    }
    SDL_Quit();
    cout<<"quit2"<<endl;
    getchar();

    free(output_play_address);
    multi_video_service_destory(handle);

    return 0;
}
