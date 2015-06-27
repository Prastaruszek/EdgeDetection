#include <iostream>
#include "filters.h"

void bfs(int* src, int* dst, int width, int height);

void initCuda(){
		cuInit(0);
		CUdevice cuDevice; 
		CUresult res = cuDeviceGet(&cuDevice, 0);
		if (res != CUDA_SUCCESS){ 
			exit(1);
		}

		CUcontext cuContext;
		res = cuCtxCreate(&cuContext,0, cuDevice);
		if (res != CUDA_SUCCESS){
			exit(1);
		}
};

void process(int* src , int* dst, int width, int height){
	initCuda();
	int middle[width*height];
	sobelFilter(src, middle, width, height);
	bfs(middle, dst, width, height);
}
