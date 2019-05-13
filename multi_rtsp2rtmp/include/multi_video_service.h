//
//  multi_video_service.h
//  multi_video_service
//
//  Created by mawei on 2019/2/21.
//  Copyright © 2019年 mawei. All rights reserved.
//

#ifndef multi_video_service_h
#define multi_video_service_h
#include <stdint.h>
#include "multi_video_service_define.h"

#define multi_video_service_handle void*
#ifdef __cplusplus
extern "C" {
#endif
    /// @brief 创建句柄
    /// @param[in] service_param 服务参数
    /// @return 调用成功则，返回正确的句柄，否则返回空指针
    multi_video_service_handle multi_video_service_create(service_param_t service_param);

    /// @brief 释放句柄
    /// @param[in] handle 服务句柄
    /// @return 调用成功则，返回正确的句柄，否则返回空指针
    void multi_video_service_destory(multi_video_service_handle handle);


    /// @brief 添加视频
    /// @param[in] handle 句柄
    /// @param[in] id 视频编号
    /// @param[in] source_url 视频源地址，rtsp地址
    /// @param[in] decode_param 编码参数
    /// @param[out] output_play_address 输出的播放地址
    /// @return 返回值
    int32_t multi_video_service_add_video(multi_video_service_handle handle,int32_t id,const char* source_url,\
                                          decode_param_t decode_param,char **output_play_address);

    /// @brief 移除视频
    /// @param[in] handle 句柄
    /// @param[in] id 视频编号
    /// @return 返回值
    int32_t multi_video_service_remove_video(multi_video_service_handle handle,int32_t id);

    /// @brief 获取所有视频信息
    /// @param[in] handle 句柄
    /// @param[out] ids 视频编号列表
    /// @param[out] video_infos 视频信息列表
    /// @param[out] video_count 视频个数
    /// @return 返回值
    int32_t multi_video_service_get_videos_info(multi_video_service_handle handle,int32_t **ids,video_info_t **videos_info,int32_t *video_count);

    /// @brief 获取单个视频信息
    /// @param[in] handle 句柄
    /// @param[in] id 视频编号
    /// @param[out] video_info 视频源信息
    /// @return 返回值
    int32_t multi_video_service_get_video_info(multi_video_service_handle handle,int32_t id,video_info_t *video_info);

    /// @brief 获取所有视频源的一帧数据
    /// @param[in] handle 句柄
    /// @param[out] ids 视频编号列表
    /// @param[out] frames 视频帧列表
    /// @param[out] video_count 视频个数
    /// @return 返回值
    int32_t multi_video_service_get_videos_one_frame(multi_video_service_handle handle,int32_t **ids,video_frame_t*** frames,int32_t *video_count);

    /// @brief 获取指定视频源的一帧数据
    /// @param[in] handle 句柄
    /// @param[out] id 视频编号
    /// @param[out] frames 视频帧列表
    /// @param[out] video_count 视频个数
    /// @return 返回值
    int32_t multi_video_service_get_video_one_frame(multi_video_service_handle handle,const int32_t ids,video_frame_t** frame);



    /// @brief 释放视频帧资源
    /// @param[in] frame 视频帧
    void multi_video_service_free_video_frame(video_frame_t *frame);


    /// @brief 添加编码的视频数据
    /// @param[in] handle 句柄
    /// @param[in] id 视频编号
    /// @param[out] frames 视频帧
    /// @return 返回值
    int32_t multi_video_service_add_encode_frame(multi_video_service_handle handle,int32_t id,video_frame_t* frame);

    /// @brief 启动编码
    /// @param[in] handle 句柄
    /// @param[in] id 视频编号
    /// @param[in] encode_param 编码参数
    /// @param[out] play_back_url 回放地址
    /// @return 返回值
    int32_t multi_video_service_start_encode(multi_video_service_handle handle,int32_t id,encode_param_t encode_param,char** play_back_url);

    /// @brief 停止编码
    /// @param[in] handle 句柄
    /// @param[in] id 视频编号
    /// @return 返回值
    int32_t multi_video_service_stop_encode(multi_video_service_handle handle,int32_t id);



    /// @brief 将一帧数据frame转换为jpeg格式的图片
    /// @param[in] frame 一帧数据
    /// @param[in] source_dir 存放的目录
    /// @param[out] file_name 回传的创建文件的名字
    /// @remark file_name 需要由调用者手动释放
    /// @return 返回值   参数有误,文件夹不存在,存储失败 .jpeg
    ///
    int32_t multi_video_service_save_jpeg(video_frame_t* frame,const char* source_dir,char**file_name);


#ifdef __cplusplus
}
#endif

#endif /* multi_context_manager_h */
