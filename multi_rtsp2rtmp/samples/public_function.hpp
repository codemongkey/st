#ifndef PUBLIC_FUNCTION_HPP
#define PUBLIC_FUNCTION_HPP
#include<stdint.h>
#include<map>
#include <sstream>
#include <fstream>
#include <chrono>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/schema.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>


#include "define.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include <SDL2/SDL.h>
#ifdef __cplusplus
}
#endif

//int64_t get_millisecond()
//{
//    auto time_now = std::chrono::system_clock::now();
//    auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());
//    return duration_in_ms.count();
//}
bool parse_devices_config(const std::string &config_file_path,SystemConfig &system_config)
{

    // 从文本中读取所有字符串
    std::ifstream file_reader(config_file_path);
    if (!file_reader.is_open()) {
        printf("%s:open file %s error\n",__FUNCTION__,config_file_path.c_str());
        return -1;
    }
    std::string dummy_str;
    std::string config_json;
    while (std::getline(file_reader, dummy_str)) {
        config_json += (dummy_str + "\n");
    }
    file_reader.close();


    rapidjson::Document json_doc;
    auto& json_cfg = json_doc.Parse(config_json.data());
    if (json_cfg.HasParseError()) {
        printf("%s: json_cfg HasParseError\n",__FUNCTION__);
        return false;
    }
    if(!json_cfg.IsObject())
    {
        printf("%s: json_cfg is not object\n",__FUNCTION__);
        return false;
    }

    if( json_cfg.HasMember("window_size") && json_cfg["window_size"].IsObject())
    {
        auto object = json_cfg["window_size"].GetObject();
        if( object.HasMember("width") && object["width"].IsInt())
        {
            system_config.window_size.width = object["width"].GetInt();
        }


        if( object.HasMember("height") && object["height"].IsInt())
        {
            system_config.window_size.height = object["height"].GetInt();
        }

    }

    if( json_cfg.HasMember("stream_server_info") && json_cfg["stream_server_info"].IsObject())
    {
        auto object = json_cfg["stream_server_info"].GetObject();
        if( object.HasMember("input_addr") && object["input_addr"].IsString())
        {
            system_config.stream_server_info.input_addr = object["input_addr"].GetString();
        }

        if( object.HasMember("input_port") && object["input_port"].IsInt())
        {
            system_config.stream_server_info.input_port = object["input_port"].GetInt();
        }

        if( object.HasMember("output_addr") && object["output_addr"].IsString())
        {
            system_config.stream_server_info.output_addr = object["output_addr"].GetString();
        }

        if( object.HasMember("output_port") && object["output_port"].IsInt())
        {
            system_config.stream_server_info.output_port = object["output_port"].GetInt();
        }


    }

    if( json_cfg.HasMember("devices") && json_cfg["devices"].IsArray())
    {
        auto config_devices = json_cfg["devices"].GetArray();

        auto iter = config_devices.begin();

        for (; iter < config_devices.end();++iter) {

            if(!(*iter).IsObject())
            {
                printf("%s: iter is not object\n",__FUNCTION__);
                return false;
            }

            if(!(*iter).HasMember("id") || !(*iter).HasMember("play_url"))
            {
                printf("%s: iter_roi no member x or y\n",__FUNCTION__);
                return false;
            }

            if(!(*iter)["id"].IsInt() || !(*iter)["play_url"].IsString())
            {
                printf("%s: iter_roi x y type is not int\n",__FUNCTION__);
                return false;
            }
            DeviceInfo device;
            device.id = (*iter)["id"].GetInt();
            device.play_url = (*iter)["play_url"].GetString();
            device.push_flag = (*iter)["push"].GetInt();
            device.play_flag = (*iter)["play_flag"].GetInt();
            device.time = (*iter)["time_limit"].GetInt();
            device.use_gpu = (*iter)["use_gpu"].GetInt();
            system_config.devices[device.id] = device;
        }

    }

    return true;
}
void CONVERT_24to32(unsigned char *image_in,unsigned char *image_out,int w,int h){
    for(int i =0;i<h;i++)
        for(int j=0;j<w;j++){
            //Big Endian or Small Endian?
            //"ARGB" order:high bit -> low bit.
            //ARGB Format Big Endian (low address save high MSB, here is A) in memory : A|R|G|B
            //ARGB Format Little Endian (low address save low MSB, here is B) in memory : B|G|R|A
            if(SDL_BYTEORDER==SDL_LIL_ENDIAN){
                //Little Endian (x86): R|G|B --> B|G|R|A
                image_out[(i*w+j)*4+0]=image_in[(i*w+j)*3+2];
                image_out[(i*w+j)*4+1]=image_in[(i*w+j)*3+1];
                image_out[(i*w+j)*4+2]=image_in[(i*w+j)*3];
                image_out[(i*w+j)*4+3]='0';
            }else{
                //Big Endian: R|G|B --> A|R|G|B
                image_out[(i*w+j)*4]='0';
                memcpy(image_out+(i*w+j)*4+1,image_in+(i*w+j)*3,3);
            }
        }
}

#endif // PUBLIC_FUNCTION_HPP
