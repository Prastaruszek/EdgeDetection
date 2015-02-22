#include <cstdio>
#include "filters.h"

#define T 32


SobelFilter::SobelFilter(int* img, int width, int height):
						Filter(img,width,height,nullptr){}
						
SobelFilter::SobelFilter(Filter* another): Filter(another->getImg(),
				another->getWidth(),another->getHeight(),another){}
				
void SobelFilter::filter(int* dst){
	if(another!=nullptr){
		another->filter(dst);
	}
	int SIZE = height*width;
	CUmodule cuModule = (CUmodule)0;
	if (cuModuleLoad(&cuModule, "obj/cudaStaff.ptx") != CUDA_SUCCESS) {
		printf("cannot load module\n");  
		exit(1); 
	} 
	
	CUfunction sobelAndSuppression, nonMaximalSupression;
	if (cuModuleGetFunction(&sobelAndSuppression, cuModule, "sobelAndSuppression") != CUDA_SUCCESS){
        exit(1);
    }
    
    if (cuModuleGetFunction(&nonMaximalSupression, cuModule, "nonMaximalSupression") != CUDA_SUCCESS){
        exit(1);
    }
    
    cuMemHostRegister	((void*) img, SIZE*sizeof(int), CU_MEMHOSTREGISTER_PORTABLE);
    cuMemHostRegister	((void*) dst, SIZE*sizeof(int), CU_MEMHOSTREGISTER_PORTABLE);
    
    CUdeviceptr cuSrc;
    CUdeviceptr cuDst;
    CUdeviceptr cuTanges;
    CUdeviceptr cuDstMaximalSupression;
    if (cuMemAlloc(&cuSrc, SIZE * sizeof(int)) != CUDA_SUCCESS) { exit(-1); }
    if (cuMemAlloc(&cuDst, SIZE * sizeof(int)) != CUDA_SUCCESS) { exit(-1); }
    if (cuMemAlloc(&cuTanges, SIZE * sizeof(float)) != CUDA_SUCCESS) { exit(-1); }
    if (cuMemAlloc(&cuDstMaximalSupression, SIZE*sizeof(int)) != CUDA_SUCCESS) {exit(-1);}
    cuMemcpyHtoD(cuSrc, img ,SIZE*sizeof(int));

    int BASIC_SIZE = 32;
    int blocks_per_grid_x = (height/BASIC_SIZE);
	int blocks_per_grid_y = (width/BASIC_SIZE);
	int threads_per_block_x = BASIC_SIZE;
	int threads_per_block_y = BASIC_SIZE;
	
	void* args[3] = {&cuSrc, &cuDst, &cuTanges};
    if (cuLaunchKernel(sobelAndSuppression, blocks_per_grid_y, blocks_per_grid_x, 1,
		threads_per_block_y, threads_per_block_x, 1, 0, 0, args, 0)!= CUDA_SUCCESS) {
			exit(-1);
	}
	cuCtxSynchronize();	
	void* args1[3] = {&cuDst, &cuTanges, &cuDstMaximalSupression};
	if (cuLaunchKernel(nonMaximalSupression, blocks_per_grid_y, blocks_per_grid_x, 1,
		threads_per_block_y, threads_per_block_x, 1, 0, 0, args1, 0)!= CUDA_SUCCESS) {
			exit(-1);
	}
	cuCtxSynchronize();
	cuMemcpyDtoH(dst, cuDstMaximalSupression , SIZE*sizeof(int));	
	
	cuMemHostUnregister	((void*) img);
	cuMemHostUnregister	((void*) dst);	
    
	/*int magX;
	int magY;int bugs=0;
	for(int i=0; i<height-1; ++i){
		for(int j=0; j<width-1; ++j){
			magX = -img[(i+1)*width+j]+img[(i+1)*width+j+1];
			magY = img[(i)*width+j+1]-img[(i+1)*width+j+1];
			magX = magX>0? magX: -magX;
			magY = magY>0? magY: -magY;
			dst[(i+1)*width+(j+1)]=(magX+magY>=T?255:0);
			if(dst[(i+1)*width+(j+1)] != (magX+magY>=T?255:0)){
				printf("%d %d\n", i, j);
				++bugs;
			}
		}
	}*/
	//printf("bugs=%d\n", bugs);
}
