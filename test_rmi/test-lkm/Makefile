# 指定编译的目标文件
# obj-m += hello_gpt.o
obj-m += old_lkm.o

# 指定目标架构和交叉编译工具链前缀
ARCH := arm64
KDIR := ~/workspace/linux
CROSS_COMPILE = /opt/toolchains/gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-

# 编译选项
#EXTRA_CFLAGS := -I/path/to/include
#EXTRA_CFLAGS := -mabi=lp64
EXTRA_CFLAGS=-g -O0

# 定义默认目标
all:
	make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

# 清理编译生成的文件
clean:
	make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean