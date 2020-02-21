# example demo
CC = /opt/hisi-linux/x86-arm/arm-himix200-linux/bin/arm-himix200-linux-g++
CURRENT_PATH = $(shell pwd)
INCLUDE = ${CURRENT_PATH}/include
LIBPATH = ${CURRENT_PATH}/lib
LIBS = -llongsesmart -lpthread

all: demo
demo: demo.cpp
	${CC} $< -I${INCLUDE} -L${LIBPATH}/3516/ ${LIBS} -o $@
clean:
	-rm -f *.o demo
