#ifndef DEFINE_H
#define DEFINE_H
#include <stdint.h>
#include <string>
#include <map>

typedef struct {
    int32_t id;
    std::string play_url;
    int time;
    bool push_flag;
    bool play_flag;
    bool use_gpu;
}DeviceInfo;

typedef struct {
    int32_t width;
    int32_t height;
}SyncSize;

typedef struct{
    std::string input_addr;
    int32_t input_port;
    std::string output_addr;
    int32_t output_port;
}StreamServerInfo;

typedef struct{
    SyncSize window_size;
    StreamServerInfo stream_server_info;
    std::map<int32_t,DeviceInfo> devices;
}SystemConfig;

#endif // DEFINE_H
