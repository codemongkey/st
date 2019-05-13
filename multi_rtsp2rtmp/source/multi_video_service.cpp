//
//  multi_context_manager.cpp
//  multi_video_service
//
//  Created by mawei on 2019/2/21.
//  Copyright © 2019年 mawei. All rights reserved.
//
#include "multi_video_service.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <map>
#include "video_manager.h"
#include "public_function.h"

//初始化服务对象
multi_video_service_handle multi_video_service_create(service_param_t service_param) {
    VideoManager *video_manager = new VideoManager();
    if(video_manager->Init(service_param) != E_SUCCESS) {
        delete video_manager;
        return NULL;
    }

    return (multi_video_service_handle)video_manager;
}

//销毁服务对象
void multi_video_service_destory(multi_video_service_handle handle) {
    if(handle == NULL) {
        return;
    }

    VideoManager *video_manager = (VideoManager*)handle;

    delete video_manager;
    video_manager = nullptr;
}

//添加相机
int32_t multi_video_service_add_video(multi_video_service_handle handle, int32_t id, const char* source_url, \
                                      decode_param_t decode_param, char **output_play_address) {
    if (handle == NULL) {
        return E_INVALID_PARAM;
    }

    if (output_play_address == NULL) {
        return E_INVALID_PARAM;
    }

    VideoManager *video_manager = (VideoManager*)handle;
    std::string out_play_url;

    int32_t ret = video_manager->AddVideo(id, source_url, decode_param, out_play_url);
    if(ret == E_SUCCESS) {
        strcpy(*output_play_address, out_play_url.c_str());
    }

    return ret;
}

//移除相机
int32_t multi_video_service_remove_video(multi_video_service_handle handle, int32_t id) {
    if(handle == NULL) {
        return E_INVALID_PARAM;
    }

    VideoManager *video_manager = (VideoManager*)handle;
    return video_manager->RemoveVideo(id);
}

//获取所有相机状态
int32_t multi_video_service_get_videos_info(multi_video_service_handle handle, int32_t **ids, video_info_t **videos_info, int32_t *video_count) {
    if(handle == NULL) {
        return E_INVALID_PARAM;
    }

    if(ids == NULL) {
        return E_INVALID_PARAM;
    }

    if(videos_info == NULL) {
        return E_INVALID_PARAM;
    }

    VideoManager *video_manager = (VideoManager*)handle;

    std::map<int32_t, video_info_t> videos_info_tmp;
    int32_t ret = video_manager->GetVideosInfo(videos_info_tmp);

    *video_count = videos_info_tmp.size();
    if(videos_info_tmp.size() != 0) {
        *ids = (int32_t*)malloc(sizeof(int32_t) * videos_info_tmp.size());
        *videos_info = (video_info_t*)malloc(sizeof(video_frame_t) * videos_info_tmp.size());

        int index = 0;
        auto it = videos_info_tmp.begin();
        for(; it != videos_info_tmp.end(); ++it) {
            (*ids)[index] = it->first;
            (*videos_info)[index] = it->second;
            ++index;
        }

    }
    return ret;
}

//获取单个相机状态
int32_t multi_video_service_get_video_info(multi_video_service_handle handle, int32_t id, video_info_t *video_info) {
    if(handle == NULL) {
        return E_INVALID_PARAM;
    }
    if(video_info == NULL) {
        return E_INVALID_PARAM;
    }

    VideoManager *video_manager = (VideoManager*)handle;

    return video_manager->GetVideoInfo(id, *video_info);
}

//获取所有相机的一帧数据
int32_t multi_video_service_get_videos_one_frame(multi_video_service_handle handle, int32_t **ids, video_frame_t*** frames, int32_t *video_count) {
    if(handle == NULL) {
        return E_INVALID_PARAM;
    }
    VideoManager *video_manager = (VideoManager*)handle;

    std::map<int32_t, video_frame_t*> video_frames;

    int32_t ret = video_manager->GetVideosOneFrame(video_frames);

    *video_count = video_frames.size();
    if(video_frames.size() != 0) {
        *ids = (int32_t*)malloc(sizeof(int32_t) * video_frames.size());
        *frames = (video_frame**)malloc(sizeof(video_frame_t**)*video_frames.size());

        int index = 0;
        auto it = video_frames.begin();
        for(; it != video_frames.end(); ++it) {
            (*ids)[index] = it->first;
            (*frames)[index] = it->second;
            ++index;
        }
    }

    return ret;
}

int32_t multi_video_service_get_video_one_frame(multi_video_service_handle handle, const int32_t id, video_frame_t** frame) {
    if(handle == NULL) {
        return E_INVALID_PARAM;
    }
    VideoManager *video_manager = (VideoManager*)handle;

    std::map<int32_t, video_frame_t*> video_frames;

    int32_t ret = video_manager->GetVideosOneFrame(video_frames);

    if(video_frames.size() != 0) {
        *frame = (video_frame*)malloc(sizeof(video_frame_t*));

        int index = 0;
        auto it = video_frames.begin();
        for(; it != video_frames.end(); ++it) {
            if(it->first == id) {
                *frame = it->second;
                return E_SUCCESS;
            }
        }
    } else {
        return E_FAILED;
    }
    return E_ID_NOT_EXIST;
}

void multi_video_service_free_video_frame(video_frame_t *frame) {
    if(frame != nullptr) {
        if(frame->buffer != nullptr) {
            free(frame->buffer);
            frame->buffer = nullptr;
        }
        free(frame);
        frame = nullptr;
    }
}
//添加编码的视频数据
int32_t multi_video_service_add_encode_frame(multi_video_service_handle handle, int32_t id, video_frame_t* frame) {
    if(handle == NULL) {
        return E_INVALID_PARAM;
    }
    VideoManager *video_manager = (VideoManager*)handle;

    return video_manager->AddEncodeFrame(id, frame);
}

//开始编码
int32_t multi_video_service_start_encode(multi_video_service_handle handle, int32_t id, encode_param_t encode_param, char** play_back_url) {
    if(handle == NULL) {
        return E_INVALID_PARAM;
    }

    if(play_back_url == NULL || *play_back_url) {
        return E_INVALID_PARAM;
    }

    VideoManager *video_manager = (VideoManager*)handle;

    std::string url;

    int32_t ret = video_manager->StartEncode(id, encode_param, url);
    if(ret == E_SUCCESS) {
        strcpy(*play_back_url, url.c_str());
    }
    return ret;
}

//停止编码
int32_t multi_video_service_stop_encode(multi_video_service_handle handle, int32_t id) {
    return 0;
}

//将bgr文件转化为jepg格式的图片
int32_t multi_video_service_save_jpeg(video_frame_t* frame, const char* source_dir, char**file_name) {
    if(frame == nullptr)
        return E_INVALID_PARAM;
    if(access(source_dir, W_OK) != 0 || access(source_dir, X_OK) != 0 || access(source_dir, F_OK) != 0)
        return E_INVALID_PARAM;
    std::string cur_ms = std::to_string(get_millisecond());
    std::string source_dir_str(source_dir);
    if(source_dir_str[source_dir_str.size() - 1] != '/')
        source_dir_str += '/';
    std::string file_name_str = source_dir_str + (cur_ms) + ".jpeg";
    *file_name = (char *)malloc(file_name_str.size() + 1);
    memset(*file_name, sizeof((file_name_str.size()) + 1), 0);
    strcpy(*file_name, file_name_str.c_str());

    unsigned char* rgb = (unsigned char*)frame->buffer;
    int size = frame->stride;
    auto ret = bgr_to_jpgfile(rgb, size, frame->width, frame->height, *file_name);
    return ret;
}
