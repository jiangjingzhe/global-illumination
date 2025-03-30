# 定义编译器
CC = gcc

CFLAGS = -Wall -O3 -D_USE_MATH_DEFINES -I. \
		 -I"D:/Coding/NVIDIA GPU Computing Toolkit/CUDA/VV11.8/include" \
		 -L"D:/Coding/NVIDIA GPU Computing Toolkit/CUDA/VV11.8/lib/x64"

LIBS = -lopengl32 -lglu32 -lfreeglut -lOpenCL

# 源文件列表
SRCS = smallptGPU.c displayfunc.c

# 目标可执行文件
TARGET = smallptGPU.exe

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	del $(TARGET)
