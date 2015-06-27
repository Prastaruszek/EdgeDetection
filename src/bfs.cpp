#include <cstdio>
#include <iostream>
#include "cuda.h"

void bfs(int* src, int* dst, int width, int height){
	int m = width;
	int n = height;
	int SIZE = n*m; 
	
	CUmodule cuModule = (CUmodule)0;
	if (cuModuleLoad(&cuModule, "obj/cudaStaff.ptx") != CUDA_SUCCESS) {
		printf("cannot load module\n");  
		exit(1);  
	} 
	
	CUfunction prepareBfs;
	CUfunction oneBfs;
	CUfunction final_battle;
	if (cuModuleGetFunction(&prepareBfs, cuModule, "prepareBfs") != CUDA_SUCCESS){
        exit(1);
    }
	if (cuModuleGetFunction(&oneBfs, cuModule, "oneBfs") != CUDA_SUCCESS){
        exit(1);
    }
    if (cuModuleGetFunction(&final_battle, cuModule, "final_battle") != CUDA_SUCCESS){
        exit(1);
    }
    cuMemHostRegister	((void*) src, SIZE*sizeof(int), CU_MEMHOSTREGISTER_PORTABLE);
    cuMemHostRegister	((void*) dst, SIZE*sizeof(int), CU_MEMHOSTREGISTER_PORTABLE);
    int changed = 1;
    CUdeviceptr cuSrc;
    CUdeviceptr cuChanged;
    if (cuMemAlloc(&cuSrc, SIZE * sizeof(int)) != CUDA_SUCCESS) { exit(-1); }
    if (cuMemAlloc(&cuChanged, sizeof(int)) != CUDA_SUCCESS) { exit(-1); }
    
    cuMemcpyHtoD(cuSrc, src ,SIZE*sizeof(int));
    cuMemcpyHtoD(cuChanged, &changed , sizeof(int));
    
    int BASIC_SIZE = 32,
    blocks_per_grid_x = (height/BASIC_SIZE),
    blocks_per_grid_y = (width/BASIC_SIZE),
    threads_per_block_x = BASIC_SIZE,
    threads_per_block_y = BASIC_SIZE;

	void* prepareArgs[1] = {&cuSrc};
	if(cuLaunchKernel(prepareBfs, blocks_per_grid_y, blocks_per_grid_x, 1,
		threads_per_block_y, threads_per_block_x, 1, 0, 0, prepareArgs, 0)!= CUDA_SUCCESS){
			std::cout << "ERROR" << std::endl;
			exit(-1);
	}
	
	cuCtxSynchronize();	

	int am=0;
    while(changed){
		++am;
		changed = 0;
		cuMemcpyHtoD(cuChanged, &changed , sizeof(int));	
		void* args[3] = {&cuSrc, &cuSrc, &cuChanged};
		if( (cuLaunchKernel(oneBfs, blocks_per_grid_y, blocks_per_grid_x, 1,
			threads_per_block_y, threads_per_block_x, 1, 0, 0, args, 0))!= CUDA_SUCCESS){
				std::cout << "ERROR" 
					<< std::endl;
				exit(-1);
		}
		cuCtxSynchronize();	
		cuMemcpyDtoH(&changed, cuChanged , sizeof(int));	
	}
	
	std::cout << "am=" << am << std::endl;
	
	void* args[3] = {&cuSrc};
	if(cuLaunchKernel(final_battle, blocks_per_grid_y, blocks_per_grid_x, 1,
			threads_per_block_y, threads_per_block_x, 1, 0, 0, args, 0)!= CUDA_SUCCESS){
				std::cout 
					<< "ERROR" << std::endl;
				exit(-1);
	}
	cuCtxSynchronize();	
	
    cuMemcpyDtoH(dst, cuSrc , SIZE*sizeof(int));
    
	cuMemHostUnregister	((void*) src);
	cuMemHostUnregister	((void*) dst);
	
	cuMemFree (cuChanged);	
	cuMemFree (cuSrc);		
}
