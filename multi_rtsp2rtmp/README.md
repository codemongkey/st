多路视频流接入，解码，重编码，转发sdk
需要streamserver

mkdir build
cd build
#Debug version

cmake -DCMAKE_BUILD_TYPE=Debug ..

#Release version

cmake ..

./demo ../configs/config.json
