//
//  public_function.h
//  multi_video_service
//  通用函数库
//  Created by mawei on 2019/2/21.
//  Copyright © 2019年 mawei. All rights reserved.
//

//获取毫秒时间戳
#include <iostream>
#include "srs_librtmp/srs_librtmp.h"
#include "libjpeg/jpeglib.h"
#include "libjpeg/jerror.h"

int64_t get_millisecond();

bool init_srs_param(srs_rtmp_t &rtmp,const std::string &rtmp_url);

int32_t bgr_to_jpgfile(unsigned char *rgb, int size, int w, int h, char *jpgfile) ;
