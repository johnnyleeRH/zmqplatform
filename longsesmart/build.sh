#!/bin/bash

currentpath=${PWD}
echo ${currentpath}
rm ${currentpath}/build -rf
dist=${currentpath}/dist
rm ${dist} -rf
mkdir ${currentpath}/build
mkdir ${dist}
cd build
cmake .. -Dplatform:str=3519 -Drelease:str=static
make
mkdir ${dist}/lib
mkdir ${dist}/lib/3519
cp ${currentpath}/output/liblongsesmart.a ${dist}/lib/3519
cmake .. -Dplatform:str=3519 -Drelease:str=shared
make
cp ${currentpath}/output/liblongsesmart.so ${dist}/lib/3519
cmake .. -Dplatform:str=3516 -Drelease:str=static
make
mkdir ${dist}/lib/3516
cp ${currentpath}/output/liblongsesmart.a ${dist}/lib/3516
cmake .. -Dplatform:str=3516 -Drelease:str=shared
make
cp ${currentpath}/output/liblongsesmart.so ${dist}/lib/3516
cp ${currentpath}/demo/demo.cpp ${dist}/
cp ${currentpath}/demo/Makefile ${dist}/
mkdir ${dist}/include
cp ${currentpath}/smartdata/* ${dist}/include/
cp ${currentpath}/src/smartdatatrans.h ${dist}/include/