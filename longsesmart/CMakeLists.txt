cmake_minimum_required(VERSION 2.8)
project(longsesmart)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-a17.cortex-a7 -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-a17.cortex-a7 -std=c++11 -fPIC")
include_directories(
        thirdparty/include
        smartdata
)

if(DEFINED platform)
        if(${platform} STREQUAL "3516")   
                set(TOOLCHAIN "/opt/hisi-linux/x86-arm/arm-himix200-linux/bin/")
                set(CMAKE_C_COMPILER ${TOOLCHAIN}/arm-himix200-linux-gcc)
                set(CMAKE_CXX_COMPILER ${TOOLCHAIN}/arm-himix200-linux-g++)
                set(AR ${TOOLCHAIN}/arm-himix200-linux-ar)
                set(THIRD_PATH ${PROJECT_SOURCE_DIR}/thirdparty/lib/3516/)
                link_directories(${PROJECT_SOURCE_DIR}/thirdparty/lib/3516/)
        else()
                set(TOOLCHAIN "/opt/hisi-linux/x86-arm/arm-hisiv500-linux/target/bin/")
                set(CMAKE_C_COMPILER ${TOOLCHAIN}/arm-hisiv500-linux-gcc)
                set(CMAKE_CXX_COMPILER ${TOOLCHAIN}/arm-hisiv500-linux-g++)
                set(AR ${TOOLCHAIN}/arm-hisiv500-linux-ar)
                set(THIRD_PATH ${PROJECT_SOURCE_DIR}/thirdparty/lib/3519/)
                link_directories(${PROJECT_SOURCE_DIR}/thirdparty/lib/3519/)
        endif()
else()
        set(TOOLCHAIN "/opt/hisi-linux/x86-arm/arm-hisiv500-linux/target/bin/")
        set(CMAKE_C_COMPILER ${TOOLCHAIN}/arm-hisiv500-linux-gcc)
        set(CMAKE_CXX_COMPILER ${TOOLCHAIN}/arm-hisiv500-linux-g++)
        set(AR ${TOOLCHAIN}/arm-hisiv500-linux-ar)
        set(THIRD_PATH ${PROJECT_SOURCE_DIR}/thirdparty/lib/3519/)
        link_directories(${PROJECT_SOURCE_DIR}/thirdparty/lib/3519/)
endif()


set(THIRD_PARTY_SRC
        thirdparty/src/face.pb.cc
        thirdparty/src/ioAV.pb.cc
        thirdparty/src/base64.cpp
        thirdparty/src/jpegsize.cpp
)

set(LONGSE_SMART_SRC
        src/msgparse.cpp
        src/smartdatatrans.cpp
        src/smarttransimpl.cpp
        src/smartpack.cpp
        src/eventtrigger.cpp
)



if(${release} STREQUAL "static")  
        add_library(longsesmart STATIC ${THIRD_PARTY_SRC} ${LONGSE_SMART_SRC})
        target_link_libraries(longsesmart hobotmq protobuf pthread)
        add_custom_command(
                TARGET longsesmart POST_BUILD
                COMMAND rm ${PROJECT_SOURCE_DIR}/output -rf
                COMMAND mkdir ${PROJECT_SOURCE_DIR}/output
                COMMAND cp ${THIRD_PATH}/libhobotmq.a ${PROJECT_SOURCE_DIR}/output
                COMMAND cp ${THIRD_PATH}/libprotobuf.a ${PROJECT_SOURCE_DIR}/output
                COMMAND cp ./liblongsesmart.a ${PROJECT_SOURCE_DIR}/output
                COMMAND ${AR} -x ${PROJECT_SOURCE_DIR}/output/libhobotmq.a
                COMMAND ${AR} -x ${PROJECT_SOURCE_DIR}/output/libprotobuf.a
                COMMAND ${AR} -x ${PROJECT_SOURCE_DIR}/output/liblongsesmart.a
                COMMAND mv ./*.o ${PROJECT_SOURCE_DIR}/output/
                COMMAND rm ${PROJECT_SOURCE_DIR}/output/*.a
                COMMAND ${AR} rcs ${PROJECT_SOURCE_DIR}/output/liblongsesmart.a ${PROJECT_SOURCE_DIR}/output/*.o
        )
else()
        add_library(longsesmart SHARED ${THIRD_PARTY_SRC} ${LONGSE_SMART_SRC})
        target_link_libraries(longsesmart hobotmq protobuf pthread)

        add_custom_command(
                TARGET longsesmart POST_BUILD
                COMMAND mv liblongsesmart.so ${PROJECT_SOURCE_DIR}/output/liblongsesmart.so
        )
endif()

set(OUTPUT_ROOT ${PROJECT_SOURCE_DIR}/output)