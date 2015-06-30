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
CXXFLAGS += $(COMMONFLAGS) -O2 `pkg-config opencv --cflags --libs` -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -lcuda
CFLAGS += $(COMMONFLAGS) -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -L/usr/local/cuda/lib64 -lcuda -lcudart

all:  detect

detect: obj/detect.o obj/processImg.o obj/bfs.o obj/SobelFilter.o obj/cudaStaff.ptx
	$(LINK) obj/detect.o obj/processImg.o obj/SobelFilter.o obj/bfs.o  -o detect  $(CXXFLAGS)

obj/detect.o: src/detect.cpp src/CImg.h
	$(CXX) -c src/detect.cpp $(CFLAGS) -o obj/detect.o

obj/processImg.o: src/processImg.cpp src/CImg.h
	$(CXX) -c src/processImg.cpp $(CFLAGS) -o obj/processImg.o

obj/SobelFilter.o: src/SobelFilter.cpp src/filters.h
	$(CXX) -c src/SobelFilter.cpp  $(CFLAGS) -o obj/SobelFilter.o
	
obj/bfs.o: src/bfs.cpp
	$(CXX) -c src/bfs.cpp  $(CFLAGS) -o obj/bfs.o

obj/cudaStaff.ptx: src/cudaStaff.cu
	$(NVCC) $(NVCCFLAGS) src/cudaStaff.cu -o obj/cudaStaff.ptx
	#$(NVCC) -c $(NVCC_FLAGS) src/cudaStaff.cu -o obj/cudaStaff.o

example: obj/example.o
	g++ obj/example.o  -o example  -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11

obj/example.o: src/example/example.cpp src/CImg.h
	g++ -c src/example/example.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -o obj/example.o

clean: 
	-rm detect
	-rm example
	-rm obj/*
