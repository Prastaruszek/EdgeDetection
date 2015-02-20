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
	
	CUfunction transpose_char;      
    if (cuModuleGetFunction(&transpose_char, cuModule, "transpose_char") != CUDA_SUCCESS){
        exit(1);
    }
    
    CUfunction transpose_short;
    if (cuModuleGetFunction(&transpose_short, cuModule, "transpose_short") != CUDA_SUCCESS){
        exit(1);
    }
    
	CUfunction oneDimSobel;
	if (cuModuleGetFunction(&oneDimSobel, cuModule, "oneDimSobel") != CUDA_SUCCESS){
        exit(1);
    }
    
    CUfunction sobelPlus;
    if (cuModuleGetFunction(&sobelPlus, cuModule, "sobelPlus") != CUDA_SUCCESS){
        exit(1);
    }
    
    cuMemHostRegister	((void*) img, SIZE*sizeof(int), CU_MEMHOSTREGISTER_PORTABLE);
    cuMemHostRegister	((void*) dst, SIZE*sizeof(int), CU_MEMHOSTREGISTER_PORTABLE);
    
    int N[2]{width, height};
    int revN[2]{height,width};
    
    CUdeviceptr cuMagX;
    CUdeviceptr cuRevMagY;
    CUdeviceptr cuMagY;
    CUdeviceptr cuImg;
    CUdeviceptr cuTransImg;
    CUdeviceptr cuN;
    CUdeviceptr cuRevN;

    if (cuMemAlloc(&cuMagX, SIZE * sizeof(int)) != CUDA_SUCCESS) { exit(-1); }
    if (cuMemAlloc(&cuMagY, SIZE * sizeof(int)) != CUDA_SUCCESS) { exit(-1); }
    if (cuMemAlloc(&cuRevMagY, SIZE * sizeof(int)) != CUDA_SUCCESS) { exit(-1); }
    if (cuMemAlloc(&cuImg, SIZE * sizeof(int)) != CUDA_SUCCESS) { exit(-1); }
    if (cuMemAlloc(&cuTransImg, SIZE * sizeof(int)) != CUDA_SUCCESS) { exit(-1); }
    if (cuMemAlloc(&cuN, 2* sizeof(int)) != CUDA_SUCCESS) { exit(-1); }
    if (cuMemAlloc(&cuRevN, 2* sizeof(int)) != CUDA_SUCCESS) { exit(-1); }
    cuMemcpyHtoD(cuImg, img ,SIZE*sizeof(int));
    cuMemcpyHtoD(cuN, N ,2*sizeof(int));
    cuMemcpyHtoD(cuRevN, revN ,2*sizeof(int));
        
    int BASIC_SIZE = 32;
   	int blocks_per_grid_x = (height/BASIC_SIZE);
	int blocks_per_grid_y = (width/BASIC_SIZE);
	int threads_per_block_x = BASIC_SIZE;
	int threads_per_block_y = BASIC_SIZE;
	
	int shDebug[SIZE];
	int chDebug[SIZE];
	
    void* argsFirst[3] = {&cuImg, &cuMagY, &cuN};
    if(cuLaunchKernel(oneDimSobel, (width+1023)/1024, 1, 1, 1024, 1,1,0,0,argsFirst, 0)
										!= CUDA_SUCCESS){
		exit(-1);
	}
    cuCtxSynchronize();	
    
    void* argsTrans[2] = {&cuImg, &cuTransImg };
    if( cuLaunchKernel(transpose_char, blocks_per_grid_y, blocks_per_grid_x, 1,
		threads_per_block_y, threads_per_block_x, 1, 0, 0, argsTrans, 0)!= CUDA_SUCCESS){
			exit(-1);
	}
	
	cuCtxSynchronize();	
	
	void* argsSec[3] = {&cuTransImg, &cuRevMagY, &cuRevN};
	if(cuLaunchKernel(oneDimSobel, (height+1023)/1024, 1, 1, 1024, 1,1,0,0,argsSec, 0)
								!= CUDA_SUCCESS){
		exit(-1);
	}
	
	cuCtxSynchronize();	
	void* argsTransSec[2] = {&cuRevMagY, &cuMagX};
	int res;
	if((res=cuLaunchKernel(transpose_short, blocks_per_grid_x, blocks_per_grid_y, 1,
		threads_per_block_x, threads_per_block_y, 1, 0, 0, argsTransSec, 0))!= CUDA_SUCCESS){
			printf("%d trasnspose_short failed\n", res);
			exit(-1);		
	}
	
	cuCtxSynchronize();	
	void* sobelPlusArgs[3] = {&cuMagX, &cuMagY, &cuTransImg};
	if(cuLaunchKernel(sobelPlus, blocks_per_grid_y, blocks_per_grid_x, 1,
		threads_per_block_y, threads_per_block_x, 1, 0, 0, sobelPlusArgs, 0)!= CUDA_SUCCESS){
			exit(-1);
	}
	cuCtxSynchronize();	
	
	cuMemcpyDtoH(dst, cuTransImg , SIZE*sizeof(int));	
	
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
