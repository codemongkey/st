//
//  multi_video_service_define.h
//  multi_video_service
//
//  Created by mawei on 2019/2/21.
//  Copyright © 2019年 mawei. All rights reserved.
//

#ifndef define_h
#define define_h
#include <stdint.h>

/// @brief FOURCC maker.
#define MAKEFOURCC(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))

/// @brief format enum definition.
typedef enum video_format {
    VIDEO_NONE = -1, /**< Unknown pixel format type */
    VIDEO_GRAY = MAKEFOURCC('G', 'R', 'E', 'Y'), /**< Gray pixel format */
    VIDEO_BGR = MAKEFOURCC('B', 'G', 'R', 24), /**< BGR pixel format */
    VIDEO_BGRA = MAKEFOURCC('B', 'G', 'R', 'A'), /**< BGRA pixel format */
    VIDEO_RGB = MAKEFOURCC('R', 'G', 'B', 24), /**< RGB pixel format */
    VIDEO_ARGB = MAKEFOURCC('A', 'R', 'G', 'B'), /**< ARGB pixel format */
    VIDEO_YUV420P = MAKEFOURCC('Y', 'V', '1', '2'), /**< YUV pixel format */
    VIDEO_NV12 = MAKEFOURCC('N', 'V', '1', '2'), /**< NV12 pixel format */
    VIDEO_NV21 = MAKEFOURCC('N', 'V', '2', '1') /**< NV21 pixel format */
} video_format_t;

/// @brief Video parameter definition.
typedef struct video_frame {
    video_format_t fmt;   /**< Video frame format */
    int64_t pts;        /**< Presentation Time Stamp */
    int32_t width;      /**< Width of frame in pixel */
    int32_t height;      /**< Height of frame in pixel */
    int32_t stride;     /**< Stride of frame in byte */
    void *buffer;        /**< Pointer to somewhere in frame.data */
} video_frame_t;

typedef struct video_size{
    int32_t width;
    int32_t height;
}video_size_t;

//流媒体服务参数
typedef struct{
    char input_addr[64];  //流媒体转发服务器输入地址
    char output_addr[64]; //流媒体转发服务器输出地址
    uint16_t input_port;  //流媒体转发服务器输入端口
    uint16_t output_port; //流媒体转发服务器输出端口
}stream_server_param_t;

//解码参数
typedef struct {
    video_format_t fmt;
    bool resize;
    int32_t resize_width;
    int32_t resize_height;
    int32_t resize_fun;   //默认线性差值
}decode_param_t;

//编码参数
typedef struct {
    int32_t fps;            //帧率
    int32_t video_width;    //视频宽
    int32_t video_height;   //视频高
    int32_t stride;         //视频补偿
    int32_t format;         //编码格式
}encode_param_t;

typedef enum {
    E_STREAM_RELAY = 1,                             //仅用转发功能
    E_STREAM_DECODE = 1<<2,                         //仅用解码功能
    E_STREAM_DECODE_RELAY = (1<<2) + 1,               //解码+转发功能
    E_STREAM_ENCODE_RELAY= (1<<3) + 1,                //编码+转发功能
    E_STREAM_DECODE_ENCODE_RELAY = (1<<3) + (1<<2) + 1, //解码+编码+转发功能
}service_type_t;


typedef struct{
    service_type_t service_type;
    stream_server_param_t stream_server_param;
    bool video_auto_reconnect;             //是否相机自动重连

    //预留接口，暂不支持
    bool use_gpu;                           //是否使用gpu
    int32_t gpu_id;                         //gpu编号
}service_param_t;

//视频状态
typedef enum {
    E_NORMAL = 0,
    E_WAITING = 1,
    E_DECODE_ERROR = 2,
    E_OPEN_STREAM_ERROR = 3,
    E_NO_STREAM_INFO = 4,
    E_NO_VIDEO_STREAM = 5,
    E_NO_CODEC = 6,
    E_ALLOC_ERROR = 7,
    E_OPEN_CODEC_ERROR = 8,
    E_DECODE_EOF = 9,

    E_STREAM_SERVER_ERROR = 10,
}video_status_t;

//视频信息
typedef struct {
    video_status_t status;
    int32_t width;
    int32_t height;
    int32_t stride;
    int32_t fps;
}video_info_t;

//错误码
typedef enum {
    E_FAILED = -1,
    E_SUCCESS = 0,
    E_ID_EXIST = 1,
    E_ID_NOT_EXIST = 2,
    E_INVALID_PARAM,
    E_STREAM_SERVER_CONNECT_ERROR,
}error_code_t;


#endif /* define_h */
