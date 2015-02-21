#include<cstdio>
#define S 64
#define ZERO 0

extern "C" {



__device__
bool btwn(int a, int x, int y){
	return (a>=x && a<y);
}

__device__
void load_to_shared(int* src, int cache[][S], int th_x, int th_y, int n, int m){
	int val, pos, ind_x, ind_y;
	if(threadIdx.x==0 && threadIdx.y==0){
		ind_x = th_x-1; ind_y = th_y-1;
		if(btwn(ind_x, 0, m) && btwn(ind_y, 0, n)){
			pos = ind_y*m + ind_x;
			val = src[pos];
		}
		cache[threadIdx.y][threadIdx.x] = val;
	}
	
	if(threadIdx.x==0 && threadIdx.y==31){
		ind_x = th_x-1; ind_y = th_y+1;
		if(btwn(ind_x, 0, m) && btwn(ind_y, 0, n)){
			pos = ind_y*m + ind_x;
			val = src[pos];
		}
		cache[threadIdx.y+2][threadIdx.x] = val;
	}
	
	if(threadIdx.x==31 && threadIdx.y==0){
		ind_x = th_x+1; ind_y = th_y-1;
		if(btwn(ind_x, 0, m) && btwn(ind_y, 0, n)){
			pos = ind_y*m + ind_x;
			val = src[pos];
		}
		cache[threadIdx.y][threadIdx.x+2] = val;
	}
	
	if(threadIdx.x==31 && threadIdx.y==31){
		ind_x = th_x+1; ind_y = th_y-1;
		if(btwn(ind_x, 0, m) && btwn(ind_y, 0, n)){
			pos = ind_y*m + ind_x;
			val = src[pos];
		}
		cache[threadIdx.y+2][threadIdx.x+2] = val;
	}

	if(threadIdx.y==0){
		ind_x = th_x; ind_y = th_y-1; val=ZERO;
		if(btwn(ind_x, 0, m) && btwn(ind_y, 0, n)){
			pos = ind_y*m + ind_x;
			val = src[pos];
		}
		cache[threadIdx.y][threadIdx.x+1] = val;
	}

	if(threadIdx.y==31){
		ind_x = th_x; ind_y = th_y+1; val=ZERO;
		if(btwn(ind_x, 0, m) && btwn(ind_y, 0, n)){
			pos = ind_y*m + ind_x;
			val = src[pos];
		}
		cache[threadIdx.y+2][threadIdx.x+1] = val;
	}
	
	if(threadIdx.x==0){
		ind_x = th_x-1; ind_y = th_y; val=ZERO;
		if(btwn(ind_x, 0, m) && btwn(ind_y, 0, n)){
			pos = ind_y*m + ind_x;
			val = src[pos];
		}
	 	cache[threadIdx.y+1][threadIdx.x] = val;
	}

	if(threadIdx.x==31){
		ind_x = th_x+1; ind_y = th_y; val=ZERO;
		if(btwn(ind_x, 0, m) && btwn(ind_y, 0, n)){
			pos = ind_y*m + ind_x;
			val = src[pos];
		}
	 	cache[threadIdx.y+1][threadIdx.x+2] = val;
	}
}

__global__
void sobelAndSuppression(int* src, int* dst_magn){
	__shared__ int cache[34][S];
	int m = gridDim.x*32;
	int n = gridDim.y*32;
    int th_x = blockIdx.x * 32 + threadIdx.x;
	int th_y = blockIdx.y * 32 + threadIdx.y;
	int i_src = th_y*m + th_x;
	int ind_x, ind_y;

	/*now we load to share with a frame of thickness eq 1*/
	cache[threadIdx.y+1][threadIdx.x+1] = src[i_src];
	load_to_shared(src, cache, th_x, th_y, n, m);

	ind_y = threadIdx.y+1; ind_x = threadIdx.x+1; //it's correct position
	int mag_x;
	int mag_y;
	__syncthreads();
	mag_x = cache[ind_y][ind_x-1] - cache[ind_y][ind_x+1];
	int magAbs_x = ((mag_x>0)?mag_x:-mag_x);
	mag_y = cache[ind_y+1][ind_x] - cache[ind_y-1][ind_x];
	int magAbs_y = ((mag_y>0)?mag_y:-mag_y);
	dst_magn[i_src] = magAbs_x+magAbs_y;
	if(threadIdx.x==0 && threadIdx.y==0){
		printf("%d\n", dst_magn[i_src]);
	
	}
}

}


