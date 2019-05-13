//
//  public_function.h
//  multi_video_service
//
//  Created by mawei on 2019/2/21.
//  Copyright © 2019年 mawei. All rights reserved.
//
#include "public_function.h"
#include <chrono>
#include <iostream>
#include "multi_video_service_define.h"
#include "srs_librtmp/srs_librtmp.h"

int64_t get_millisecond()
{
    auto time_now = std::chrono::system_clock::now();
    auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());
    return duration_in_ms.count();
}

bool init_srs_param(srs_rtmp_t &rtmp,const std::string &rtmp_url)
{

    printf("%s:(%s).\n",__FUNCTION__,rtmp_url.c_str());
    if(rtmp == nullptr) {
        rtmp = srs_rtmp_create(rtmp_url.c_str());
        if(rtmp == nullptr) {
            printf("%s: srs_rtmp_create failed\n",__FUNCTION__);
            return false;
        }
        //srs_rtmp_set_timeout(rtmp,800*1000,600*1000);
    }



    if (srs_rtmp_handshake(rtmp) != 0) {
        printf("%s: srs_rtmp_handshake failed\n",__FUNCTION__);
        return false;
    }

    if (srs_rtmp_connect_app(rtmp) != 0) {
        printf("%s: srs_rtmp_handshake failed\n",__FUNCTION__);
        return false;
    }

    if (srs_rtmp_publish_stream(rtmp) != 0) {
      printf("%s:(%s) publish stream faile.\n",__FUNCTION__,rtmp_url.c_str());
      return false;
    }

    printf("%s: (%s) success.\n",__FUNCTION__,rtmp_url.c_str());
    return true;
}

int32_t bgr_to_jpgfile(unsigned char *rgb, int size, int w, int h, char *jpgfile) {
    struct jpeg_error_mgr jerr;
    jpeg_std_error(&jerr);

    struct jpeg_compress_struct cinfo;
    cinfo.err = &jerr;
    jpeg_create_compress(&cinfo);

    FILE *fp = fopen(jpgfile, "wb+");
    if(fp == nullptr)
        return E_INVALID_PARAM;
    jpeg_stdio_dest(&cinfo, fp);
    //jpeg_mem_dest(&cinfo,jpg,(unsigned long*)j_size);

    cinfo.image_width = w;
    cinfo.image_height = h;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 100, TRUE);
    jpeg_start_compress(&cinfo, TRUE);
    int row_stride = cinfo.image_width * cinfo.input_components;

    JSAMPROW row_pointer[1];
    while(cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = & rgb[cinfo.next_scanline * row_stride];
        //row_pointer[0] = & rgb[(cinfo.image_height - cinfo.next_scanline - 1) * row_stride];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    fclose(fp);
    return E_SUCCESS;
}

