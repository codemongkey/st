//
//  video_decode.cpp
//  multi_video_service
//
//  Created by mawei on 2019/3/4.
//  Copyright © 2019年 mawei. All rights reserved.
//
#include <unistd.h>
#include "public_function.h"
#include "video_decode.h"
#include "multi_context_frames.h"
#include "source/video_manager.h"
extern std::map<int,bool> push_flag;
extern std::map<int,interface> video_config;
VideoDecode::VideoDecode(int32_t id,MultiContextFrames *multi_context_frames,service_type_t service_type):
    id_(id),
    multi_context_frames_(multi_context_frames),
    service_type_(service_type),
    source_url_(""),
    rtmp_client_(nullptr),
    input_play_address_(""),
    video_index_(-1),
    decode_flag_(false),
    decode_thread_(nullptr),
    decode_format_ctx_(nullptr),
    decode_codec_ctx_(nullptr)
{
    memset(&decode_param_,0,sizeof(decode_param_));
    memset(&video_info_,0,sizeof(video_info_));
    video_info_.status = E_WAITING;


    av_register_all();
    avformat_network_init();

    //av_log_set_level(AV_LOG_QUIET);
}
VideoDecode::~VideoDecode()
{

}
//对应到sense_decode中source_url->input_url(init function para), input_play_address->normal_address(Start function para)
int32_t VideoDecode::Start(const std::string &source_url,const decode_param_t &decode_param,const std::string &input_play_address)
{
    input_play_address_ = input_play_address;
    if(!decode_flag_)
    {

        AVCodec *pDecodeCodec;

        decode_format_ctx_ = avformat_alloc_context();
//        avformat_alloc_output_context2(&decode_format_ctx_,NULL,"flv","rtmp");
//        decode_format_ctx_->oformat->audio_codec =AV_CODEC_ID_AAC;
//        decode_format_ctx_->oformat->video_codec = AV_CODEC_ID_H264;
        if(decode_format_ctx_ == nullptr)
        {
            printf("%s:decode_format_ctx_ alloc failed(%s).\n",__FUNCTION__,source_url.c_str());
            video_info_.status = E_ALLOC_ERROR;
            return E_FAILED;
        }

        AVDictionary* options = NULL;
        const char *rtspHead = "rtsp://";
        if(strncmp(source_url.c_str(),rtspHead,strlen(rtspHead)) == 0)
        {
            av_dict_set(&options, "buffer_size", "2048000", 0);
            av_dict_set(&options, "stimeout", "2000000", 0);//设置超时断开连接时间
            av_dict_set(&options, "max_delay", "100000", 0);//设置最大延时
            //av_dict_set(&options, "fflags", "nobuffer", 0);//
            av_dict_set(&options, "rtsp_transport", "tcp", 0);  //以tcp方式打开，如果以tcp方式打开将udp替换为tcp
        }
        int ret = avformat_open_input(&decode_format_ctx_,source_url.c_str(),NULL,&options);

        if(ret != 0){
            printf("%s:could not open input stream ret (%d), camera url(%s).\n",__FUNCTION__,ret,source_url.c_str());

            video_info_.status = E_OPEN_STREAM_ERROR;
            return E_FAILED;
        }
        if(avformat_find_stream_info(decode_format_ctx_,NULL)<0){
            printf("%s:could not find stream information, camera url %s.\n",__FUNCTION__,source_url.c_str());

            video_info_.status = E_NO_STREAM_INFO;
            return E_FAILED;
        }

        //AV_CODEC_ID_AAC
        //avformat_write_header
        video_index_ = -1;
        for(int i = 0; i < decode_format_ctx_->nb_streams; i++)
        {
            AVStream* str = decode_format_ctx_->streams[i];
            if(decode_format_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                video_index_ = i;
                //break;
            }
        }
        printf("shichang:%lld\n",decode_format_ctx_->duration);
        printf("fengzhuanggeshi:%s\n",decode_format_ctx_->iformat->long_name);
        printf("kuangao:%d*%d\n",decode_format_ctx_->streams[video_index_]->codec->width,decode_format_ctx_->streams[video_index_]->codec->height);
        if(video_index_ == -1)
        {
            printf("%s:could not find a video stream, url %s.\n",__FUNCTION__,source_url.c_str());
            video_info_.status = E_NO_VIDEO_STREAM;
            return E_FAILED;
        }
        //decode_codec_ctx_ = m_pDecodeFormateCtx->streams[m_videoIndex]->codec;

        AVCodecParameters *codec_param;

        codec_param = decode_format_ctx_->streams[video_index_]->codecpar;
//        codec_param->width=1280;
//        codec_param->height=720;
        pDecodeCodec = avcodec_find_decoder(codec_param->codec_id);

        if(pDecodeCodec==NULL)
        {
            printf("%s:could not find codec, url %s.\n",__FUNCTION__,source_url.c_str());
            video_info_.status = E_NO_CODEC;
            return E_FAILED;
        }

        decode_codec_ctx_ = avcodec_alloc_context3(pDecodeCodec);
        if(decode_codec_ctx_ == nullptr)
        {
            printf("%s:avcodec_alloc_context3 failed, url %s.\n",__FUNCTION__,source_url.c_str());
            video_info_.status = E_ALLOC_ERROR;
            return E_FAILED;
        }

        if(avcodec_open2(decode_codec_ctx_, pDecodeCodec,NULL)<0)
        {
            printf("%s:could not open codec, url %s.\n",__FUNCTION__,source_url.c_str());
            video_info_.status = E_OPEN_CODEC_ERROR;
            return E_FAILED;
        }
        decode_codec_ctx_->width= codec_param->width;
        decode_codec_ctx_->height = codec_param->height;
//        decode_codec_ctx_->width= 1280;
//        decode_codec_ctx_->height = 720;
        decode_codec_ctx_->pix_fmt = (AVPixelFormat)codec_param->format;

        AVStream *stream = decode_format_ctx_->streams[video_index_];
        if(stream->avg_frame_rate.den != 0)
        {
            video_info_.fps = stream->avg_frame_rate.num/stream->avg_frame_rate.den;//每秒多少帧
        }

        //print video format
        av_dump_format(decode_format_ctx_,0,source_url.c_str(),0);

        source_url_ = source_url;
        decode_param_.fmt = decode_param.fmt;
        decode_param_.resize = decode_param.resize;
        decode_param_.resize_height = decode_param.resize_height;
        decode_param_.resize_width = decode_param.resize_width;
        decode_param_.resize_fun = decode_param.resize_fun;

        if(!decode_param_.resize)
        {
            video_info_.width = decode_codec_ctx_->width;
            video_info_.height = decode_codec_ctx_->height;
        } else
        {
            video_info_.width = decode_param_.resize_width;
            video_info_.height = decode_param_.resize_height;
        }

        if(service_type_ & E_STREAM_RELAY )
        {
            if(!init_srs_param(rtmp_client_,input_play_address))
            {
                printf("%s:init_srs_param error, input_play_address %s.\n",__FUNCTION__,\
                       input_play_address.c_str());
                return E_STREAM_SERVER_CONNECT_ERROR;
            }
        }

        decode_thread_ = new std::thread(&VideoDecode::Decode,this);
        if(decode_thread_ == nullptr)
        {
            printf("%s:%s create decode thread error\n",__FUNCTION__,source_url.c_str());

            if(service_type_ & E_STREAM_RELAY )
            {
                srs_rtmp_destroy(rtmp_client_);
                rtmp_client_ = nullptr;
            }

            return E_FAILED;
        }

        decode_flag_ = true;

        //ingore stride
        multi_context_frames_->AddContext(id_,video_info_);

    }
    printf("%s:%s success\n",__FUNCTION__,source_url.c_str());
    return E_SUCCESS;
}

int32_t VideoDecode::Stop()
{
    if(decode_flag_)
    {
        decode_flag_ = false;
        decode_thread_->join();
        delete decode_thread_;
        decode_thread_ = nullptr;

        if(service_type_ & E_STREAM_RELAY )
        {
            srs_rtmp_destroy(rtmp_client_);
            rtmp_client_ = nullptr;
        }


        if(decode_format_ctx_ != nullptr)
        {
            avformat_close_input(&decode_format_ctx_);
        }

        if(decode_codec_ctx_ != nullptr)
        {
            avcodec_close(decode_codec_ctx_);
        }

        multi_context_frames_->RemoveContext(id_);
    }

    return E_SUCCESS;
}

video_info_t VideoDecode::Info()
{
    return video_info_;
}
void VideoDecode::Decode()
{
    AVFrame	*pFrame;
    AVFrame	*pOutFrame;
    struct SwsContext *img_convert_ctx;
    int ret = 0;

    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));

    pFrame = av_frame_alloc();
    pOutFrame = av_frame_alloc();
    if(pFrame == nullptr)
    {
        printf("%s:pFrame av_frame_alloc failed.\n",__FUNCTION__);
        video_info_.status = E_DECODE_ERROR;
        return;
    }

    AVPixelFormat av_format;
    VideoFormat2AvPixelFormat(decode_param_.fmt,av_format);//获得当前需要解码视频的格式

    img_convert_ctx = sws_getContext(decode_codec_ctx_->width, decode_codec_ctx_->height, decode_codec_ctx_->pix_fmt,
        video_info_.width, video_info_.height, av_format, SWS_FAST_BILINEAR, NULL, NULL, NULL);

    video_info_.status = E_NORMAL;
    while(decode_flag_)
    {
        //Wait one frame //10.11.196.87
        //while(!video_config[id_].push_flag);

        ret = av_read_frame(decode_format_ctx_, packet);

        if(ret == AVERROR_EOF)
        {
            printf("%s: read file end (%d), url %s.\n",__FUNCTION__,ret,source_url_.c_str());
            video_info_.status = E_DECODE_EOF;
            break;
        }

        if(ret == AVERROR(EAGAIN))
        {
            continue;
        }

        if(ret < 0)
        {
            printf("%s: read frame error(%d), url %s.\n",__FUNCTION__,ret,source_url_.c_str());
            video_info_.status = E_DECODE_ERROR;
            break;
        }

        if(packet->stream_index != video_index_)
        {
            printf("not video stream_index:%d\n",packet->stream_index);
            continue;
        } else {
            printf("isvideo stream_index:%d\n",packet->stream_index);
        }

        if((service_type_ & E_STREAM_RELAY)&&video_config[id_].push_flag)//
        {
            // std::cout<<"===================================decoding packet id:"<<id_<<std::endl;
            if(video_config[id_].shake_again==true) {//publish outtime后要重连
                printf("id:%d,rtmp_srs_shake_again\n",id_);
                video_config[id_].shake_again=false;
                srs_rtmp_destroy(rtmp_client_);
                rtmp_client_=nullptr;
                init_srs_param(rtmp_client_,this->input_play_address_);
            }
            //send to srs
//            if(packet->stream_index != video_index_){
//                bool res = srs_aac_is_adts((char*)packet->data, packet->size);
//                if(res)
//                 {
//                    if ((ret = srs_audio_write_raw_frame(rtmp_client_,
//                        10, 2, 1, 1,
//                        (char*)packet->data, packet->size, 45)) != 0
//                    ) {
//                        srs_human_trace("send audio raw data failed. ret=%d", ret);
//                        //goto rtmp_destroy;
//                        if(rtmp_client_ != nullptr)
//                        {
//                            srs_rtmp_destroy(rtmp_client_);
//                            rtmp_client_ = nullptr;
//                        }
//                        video_info_.status = E_STREAM_SERVER_ERROR;


//                        break;
//                    }}

//            } else
            {
                ret = srs_h264_write_raw_frames(rtmp_client_, (char*)packet->data, packet->size, \
                                                packet->pts/100, packet->dts/100);
                if(ret != 0)//发送失败
                {
                    if (srs_h264_is_dvbsp_error(ret)) {
                        printf("ignore drop video error, code=%d\n", ret);
                    } else if (srs_h264_is_duplicated_sps_error(ret)) {
                        printf("ignore duplicated sps, code=%d\n", ret);
                    } else if (srs_h264_is_duplicated_pps_error(ret)) {
                        printf("ignore duplicated pps, code=%d\n", ret);
                    } else {

                        printf("====================================================================\n");
                        printf("%s: error(%d) send h264 raw data failed,will be reconnect,input_play_address_:%s.\n",\
                               __FUNCTION__,ret,input_play_address_.c_str());

                        if(rtmp_client_ != nullptr)
                        {
                            srs_rtmp_destroy(rtmp_client_);
                            rtmp_client_ = nullptr;
                        }
                        video_info_.status = E_STREAM_SERVER_ERROR;


                        break;
                    }
                }

            }

        }
        if((service_type_ & E_STREAM_DECODE)&&0)//&&video_config[id_].push_flag
        {
            //be decode
            ret = avcodec_send_packet(decode_codec_ctx_,packet);

            if (ret == AVERROR(EAGAIN)) {
                continue;
            } else if (ret == AVERROR_INVALIDDATA) {
                printf("%s:avcodec_send_packet failed(%d),packet->size(%d) continue.\n",__FUNCTION__,ret,packet->size);
                av_packet_unref(packet);
                continue;
            } else if (ret < 0) {
                printf("%s:avcodec_send_packet failed(%d),packet->size(%d) break.\n",__FUNCTION__,ret,packet->size);
                video_info_.status = E_DECODE_ERROR;
                break;
            }

            while (ret >= 0) {

                ret = avcodec_receive_frame(decode_codec_ctx_,pFrame);
                if (ret == AVERROR(EAGAIN))// seems to have problems
                    continue;
                else if (ret < 0) {
                    printf("%s:avcodec_receive_frame failed(%d).\n",__FUNCTION__,ret);
                    video_info_.status = E_DECODE_ERROR;
                    break;
                }

                if(pFrame->pts == AV_NOPTS_VALUE )
                {
                    continue;
                }

                if(multi_context_frames_ != nullptr)
                {
                    //resize frame
                    video_frame_t *vi_frame = (video_frame_t *)malloc(sizeof(video_frame_t));
                    vi_frame->buffer = malloc(av_image_get_buffer_size(av_format, video_info_.width, video_info_.height,1));

                    if (decode_param_.fmt == VIDEO_NONE)
                    {
                        vi_frame->fmt = VIDEO_YUV420P;
                    } else
                    {
                        vi_frame->fmt = decode_param_.fmt;
                    }

                    vi_frame->height = video_info_.height;
                    vi_frame->width = video_info_.width;
                    vi_frame->pts = pFrame->pts;

                    av_image_fill_arrays(pOutFrame->data, pOutFrame->linesize,(const uint8_t*)vi_frame->buffer,
                         av_format,video_info_.width, video_info_.height,1);

                    sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pFrame->height, pOutFrame->data,pOutFrame->linesize);
                    vi_frame->stride = pOutFrame->linesize[0];

                    video_info_.stride = vi_frame->stride;

                    multi_context_frames_->AddFrame(id_,vi_frame);
                }
            }

            if(video_info_.status == E_DECODE_ERROR)
            {
                break;
            }
        }

        av_packet_unref(packet);
    }

    av_frame_free(&pFrame);

    av_frame_free(&pOutFrame);

    sws_freeContext(img_convert_ctx);

    printf("%s: end, url %s.\n",__FUNCTION__,source_url_.c_str());

}

void VideoDecode::VideoFormat2AvPixelFormat(video_format_t &vi_format, AVPixelFormat &av_format)
{
    switch(vi_format)
    {
    case VIDEO_GRAY:
        av_format = AV_PIX_FMT_GRAY8;
        break;
    case VIDEO_BGR:
        av_format = AV_PIX_FMT_BGR24;
        break;
    case VIDEO_BGRA:
        av_format = AV_PIX_FMT_BGRA;
        break;
    case VIDEO_RGB:
        av_format = AV_PIX_FMT_RGB24;
        break;
    case VIDEO_ARGB:
        av_format = AV_PIX_FMT_ARGB;
        break;
    case VIDEO_YUV420P:
        av_format = AV_PIX_FMT_YUV420P;
        break;
    case VIDEO_NV12:
        av_format = AV_PIX_FMT_NV12;
        break;
    case VIDEO_NV21:
        av_format = AV_PIX_FMT_NV21;
        break;
    default:
        av_format = AV_PIX_FMT_YUV420P;
    }
}
