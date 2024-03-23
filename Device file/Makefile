obj-m += Device_file.o

PWD := $(shell pwd)
CROSS=/home/phamtuan/work/beagle_bone_black/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
KER_DIR=/home/phamtuan/work/beagle_bone_black/kernelbuildscripts/KERNEL

all:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KER_DIR) M=$(PWD) modules

clean:
	make -C $(KER_DIR) M=$(PWD) clean
