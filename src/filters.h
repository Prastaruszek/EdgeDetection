#ifndef _FILTER
#define _FILTER
#include "cuda.h"

class Filter
{
public:
	Filter(int* img, int width, int height, Filter* another):img(img), 
							width(width), height(height), 
							another(another){}
	virtual void filter(int* dst) = 0;  
	int* getImg(){return img;}
	int getWidth(){return width;}
	int getHeight(){return height;}
	static void initCuda(){
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
	}
protected:
	Filter* another;
	int* img;
	int width;      
	int height; 
};

class SobelFilter : public Filter{
public:
	SobelFilter(int* img, int width, int height);
	SobelFilter(Filter* another);
	void filter(int* dst);
};

#endif
