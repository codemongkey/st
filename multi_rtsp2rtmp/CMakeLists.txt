CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

SET(LIB_NAME multi_video_service)

PROJECT(${LIB_NAME} C CXX)

FUNCTION (exclude OUTPUT INPUT)
SET(EXCLUDES ${ARGN})
FOREACH(EXCLUDE ${EXCLUDES})
list(REMOVE_ITEM INPUT "${EXCLUDE}")
ENDFOREACH()
SET(${OUTPUT} ${INPUT} PARENT_SCOPE)
ENDFUNCTION(exclude)

IF (CMAKE_BUILD_TYPE STREQUAL Debug)
message(STATUS "debug model")
add_definitions(-DDEBUG)
ELSE()
message(STATUS "release model")
ENDIF()
FIND_PACKAGE(Threads REQUIRED)

# Find required protobuf package



FILE(GLOB_RECURSE source ${CMAKE_CURRENT_SOURCE_DIR}/source/*.[ch]pp ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cc)
FILE(GLOB_RECURSE samples ${CMAKE_CURRENT_SOURCE_DIR}/samples/*.cpp)
#EXCLUDE(samples "${samples}" ${common_files})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/include/)


LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libs)
LINK_DIRECTORIES(/usr/local/lib)


#set(CMAKE_EXE_LINKER_FLAGS "-static")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++11 -Wl,-Bsymbolic")
add_compile_options(-fPIC)

SET(LIB_SRC ${source})
ADD_LIBRARY (${LIB_NAME} SHARED ${LIB_SRC})
TARGET_LINK_LIBRARIES(${LIB_NAME} ${CMAKE_THREAD_LIBS_INIT}
 avformat avcodec avutil swscale swresample x264
 ssl crypto srs_rtmp
 z dl pthread lzma stdc++ jpeg
)

SET(CMAKE_BUILD_TYPE DEBUG)

FOREACH(sample_src ${samples})
    GET_FILENAME_COMPONENT(sample_name ${sample_src} NAME_WE)
    ADD_EXECUTABLE(${sample_name} ${sample_src})
    TARGET_LINK_LIBRARIES(${sample_name} ${CMAKE_THREAD_LIBS_INIT}
    ${LIB_NAME}
    SDL2main SDL2 dl pthread lzma stdc++
	)
ENDFOREACH()
