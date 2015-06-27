#include <iostream>
#include "filters.h"

#define T 32

void sobelFilter(int* src, int* dst, int width, int height){
	
	int SIZE = height*width;
	CUmodule cuModule = (CUmodule)0;
	if (cuModuleLoad(&cuModule, "obj/cudaStaff.ptx") != CUDA_SUCCESS) {
		std::cout << "cannot load module" << std::endl;  
		exit(1); 
	} 
	
	CUfunction sobel, nonMaximalSupression;
	if (cuModuleGetFunction(&sobel, cuModule, "sobel") != CUDA_SUCCESS){
        exit(1);
    }
    
    if (cuModuleGetFunction(&nonMaximalSupression, cuModule, "nonMaximalSupression") != CUDA_SUCCESS){
        exit(1);
    }
    
    cuMemHostRegister	((void*) src, SIZE*sizeof(int), CU_MEMHOSTREGISTER_PORTABLE);
    cuMemHostRegister	((void*) dst, SIZE*sizeof(int), CU_MEMHOSTREGISTER_PORTABLE);
    
    CUdeviceptr cuSrc;
    CUdeviceptr cuDst;
    CUdeviceptr cuTangens;
    CUdeviceptr cuDstMaximalSupression;
    if (cuMemAlloc(&cuSrc, SIZE * sizeof(int)) != CUDA_SUCCESS) { exit(-1); }
    if (cuMemAlloc(&cuDst, SIZE * sizeof(int)) != CUDA_SUCCESS) { exit(-1); }
    if (cuMemAlloc(&cuTangens, SIZE * sizeof(float)) != CUDA_SUCCESS) { exit(-1); }
    if (cuMemAlloc(&cuDstMaximalSupression, SIZE*sizeof(int)) != CUDA_SUCCESS) {exit(-1);}
    cuMemcpyHtoD(cuSrc, src ,SIZE*sizeof(int));

    
    int blocks_per_grid_x = (height/32),
    blocks_per_grid_y = (width/32),
    threads_per_block_x = 32,
    threads_per_block_y = 32;
	
	void* args[3] = {&cuSrc, &cuDst, &cuTangens};
    if (cuLaunchKernel(sobel, blocks_per_grid_y, blocks_per_grid_x, 1,
		threads_per_block_y, threads_per_block_x, 1, 0, 0, args, 0)!= CUDA_SUCCESS) {
			std::cout << "ERROR OCCURED" << std::endl;
			exit(-1);
	}
	
	cuCtxSynchronize();	
	void* args1[3] = {&cuDst, &cuTangens, &cuDstMaximalSupression};
	if (cuLaunchKernel(nonMaximalSupression, blocks_per_grid_y, blocks_per_grid_x, 1,
		threads_per_block_y, threads_per_block_x, 1, 0, 0, args1, 0)!= CUDA_SUCCESS) {
			exit(-1);
	}
	cuCtxSynchronize();
	cuMemcpyDtoH(dst, cuDstMaximalSupression , SIZE*sizeof(int));	
	
	cuMemHostUnregister	((void*) src);
	cuMemHostUnregister	((void*) dst);	
}

    
	/*int magX;
	int magY;int bugs=0;
	for(int i=0; i<height-1; ++i){
		for(int j=0; j<width-1; ++j){
			magX = -src[(i+1)*width+j]+src[(i+1)*width+j+1];
			magY = src[(i)*width+j+1]-src[(i+1)*width+j+1];
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
	
