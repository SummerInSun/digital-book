CROSS_COMPILE=arm-linux-

AS 	= $(CROSS_COMPILE)as
LD 	= $(CROSS_COMPILE)ld
CC 	= $(CROSS_COMPILE)gcc
NM 	= $(CROSS_COMPILE)nm
#CPP	= $(CROSS_COMPILE)cpp

OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
STRIP	= $(CROSS_COMPILE)strip

#export ���´���
export AS LD CC NM
export OBJDUMP OBJCOPY STRIP

CFLAGS := -Wall -O2 -g
CFLAGS += -I $(shell pwd)/include
CFLAGS += -I $(shell pwd)/include/system
CFLAGS += -I /work/tools/opt/FriendlyARM/toolschain/4.4.3/arm-none-linux-gnueabi/include/freetype2
CFLAGS += -I /work/tools/usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/usr/include

LDFLAGS := -lm -lfreetype -lts -lpthread

export CFLAGS LDFLAGS

TOP_DIR := $(shell pwd)

TARGET := test

obj-y += main.o
obj-y += display/
obj-y += encoding/
obj-y += font/
obj-y += system/
obj-y += draw/
obj-y += input/
obj-y += netprint/

all :
	@ make -C ./ -f $(TOP_DIR)/Makefile.build
	@ $(CC) $(LDFLAGS) -o $(TARGET) built-in.o

clean :
	rm -rf $(shell find -name "*.o")

distclean :
	rm -rf $(shell find -name "*.o.d")
	rm -rf $(shell find -name "*.o")
