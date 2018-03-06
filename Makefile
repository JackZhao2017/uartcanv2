
# CROSS_COMPILE=/mnt/hgfs/android-ndk-r14b/my_toolchain/bin/aarch64-linux-android-

CROSS_COMPILE=/home/jack/Desktop/imax6/linux3.0.35/toolchain/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-

OBJDIR = .
SRCDIR = .

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPLUS	= $(CROSS_COMPILE)g++
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP	= $(CROSS_COMPILE)objdump
RM		= rm -f
MAKEDIR	= mkdir -p

CINC   :=-I./UartCanUtil
CINC   +=-I./UartCanSend
CINC   +=-I./UartCanReceive/
CINC   +=-I./UartCanReceive/UartCanRead
CINC   +=-I./UartCanReceive/UartCanParse
CINC   +=-I./UartCanSend/Encode



UartCan.a::UartCan.cpp UartCanUtil/*cpp UartCanSend/*cpp  UartCanSend/Encode/*cpp UartCanReceive/UartCanRead/*cpp   UartCanReceive/UartCanParse/*cpp   UartCanReceive/*cpp
	${CPLUS}  -fPIC  -O3 ${CINC}  -c  $^ 
	${AR} rv  UartCan.a *.o
	rm *.o


UartCanCpp.out:example.cpp  UartCan.cpp UartCanUtil/*cpp UartCanSend/*cpp  UartCanSend/Encode/*cpp UartCanReceive/UartCanRead/*cpp   UartCanReceive/UartCanParse/*cpp   UartCanReceive/*cpp 
			    
	${CPLUS}  -O3 ${CINC}  $^ -o $@ -lpthread


DEMO=UartCanCpp.out

.PHONY: all
all : UartCan.a ${DEMO}


.PHONY: clean
clean :
	rm -f  ${DEMO} UartCan.a
