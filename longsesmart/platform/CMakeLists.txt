cmake_minimum_required(VERSION 2.8)
project(smartplatform)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-a17.cortex-a7 -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-a17.cortex-a7 -std=c++11 -fPIC")

set(TOOLCHAIN "/opt/hisi-linux/x86-arm/arm-hisiv500-linux/target/bin/")
set(CMAKE_C_COMPILER ${TOOLCHAIN}/arm-hisiv500-linux-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN}/arm-hisiv500-linux-g++)
set(AR ${TOOLCHAIN}/arm-hisiv500-linux-ar)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../)
set(THIRD_PATH ${PROJECT_SOURCE_DIR}/thirdparty/lib/3519/)
link_directories(${PROJECT_SOURCE_DIR}/thirdparty/lib/3519/)

#MESSAGE ("lrh" ${PROJECT_SOURCE_DIR})

include_directories(
        ${PROJECT_SOURCE_DIR}/thirdparty/include
        ${PROJECT_SOURCE_DIR}/smartdata
        ${PROJECT_SOURCE_DIR}/src
)

set(THIRD_PARTY_SRC
        ${PROJECT_SOURCE_DIR}/thirdparty/src/face.pb.cc
        ${PROJECT_SOURCE_DIR}/thirdparty/src/ioAV.pb.cc
        ${PROJECT_SOURCE_DIR}/thirdparty/src/base64.cpp
)

set(SMART_SRC
        ${PROJECT_SOURCE_DIR}/platform/smartparse.cpp
        ${PROJECT_SOURCE_DIR}/platform/smartrecv.cpp
        ${PROJECT_SOURCE_DIR}/platform/vcodec.cpp
        ${PROJECT_SOURCE_DIR}/platform/platform.cpp
)

add_executable(smartplatform ${THIRD_PARTY_SRC} ${SMART_SRC})
target_link_libraries(smartplatform hobotmq protobuf pthread)

set(OUTPUT_ROOT ${PROJECT_SOURCE_DIR}/output)