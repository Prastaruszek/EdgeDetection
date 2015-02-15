CUDA_INSTALL_PATH ?= /usr/local/cuda

CXX := g++
CC := gcc
LINK := g++ -fPIC
NVCC  := /usr/local/cuda/bin/nvcc

# Includes
INCLUDES = -I. -I$(CUDA_INSTALL_PATH)/include

# Libraries
LIB_CUDA := -lcuda


# Options
NVCCOPTIONS = -arch sm_20 -ptx

# Common flags
COMMONFLAGS += $(INCLUDES)
NVCCFLAGS += $(COMMONFLAGS) $(NVCCOPTIONS)
CXXFLAGS += $(COMMONFLAGS)
CFLAGS += $(COMMONFLAGS)

all:  detect

detect: obj/detect.o obj/processImg.o obj/SobelFilter.o obj/cudaStaff.o
	g++ obj/detect.o obj/processImg.o obj/SobelFilter.o  -o detect  -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -L/usr/local/cuda/lib64 -lcuda -lcudart

obj/detect.o: src/detect.cpp src/CImg.h
	g++ -c src/detect.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -o obj/detect.o

obj/processImg.o: src/processImg.cpp src/CImg.h
	g++ -c src/processImg.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -o obj/processImg.o

obj/SobelFilter.o: src/SobelFilter.cpp src/filters.h
	g++ -c src/SobelFilter.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -L/usr/local/cuda/lib64 -lcuda -lcudart -o obj/SobelFilter.o	-I. -I$(CUDA_INSTALL_PATH)/include 

obj/cudaStaff.o: src/cudaStaff.cu
	$(NVCC) -c -arch=sm_20 src/cudaStaff.cu 

example: obj/example.o
	g++ obj/example.o  -o example  -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11

obj/example.o: src/example/example.cpp src/CImg.h
	g++ -c src/example/example.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -o obj/example.o

clean: 
	-rm detect
	-rm example
	-rm obj/*
