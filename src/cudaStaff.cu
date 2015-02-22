#include<cstdio>
#define S 64
#define ZERO 0
#define PI 3.14159265
#define LOW 16
#define HIGH 32
#define QUEUE_SIZE 128
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
void sobelAndSuppression(int* src, int* dst_magni, float * tangesOut){
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
	dst_magni[i_src] = magAbs_x+magAbs_y;
	//dst_magni[i_src] = (magAbs_x+magAbs_y)>32?255:0;
	tangesOut[i_src] = atan2((float) mag_y,(float) mag_x)*180/PI;
	if(threadIdx.x==0 && threadIdx.y==0){
		printf("Magnitude=%d x=%f angle=%f\n", dst_magni[i_src], ((float) mag_x)/((float) mag_y), tangesOut[i_src]);
	}
}

__global__
void nonMaximalSupression(int * magn, float * tanges, int * dest) {
	__shared__ int cacheMagn[34][S], cacheTanges[34][S];
	int m = gridDim.x*32;
	int n = gridDim.y*32;
    int th_x = blockIdx.x * 32 + threadIdx.x;
	int th_y = blockIdx.y * 32 + threadIdx.y;
	int i_src = th_y*m + th_x;
	int ind_x, ind_y;

	cacheMagn[threadIdx.y+1][threadIdx.x+1] = magn[i_src];
	load_to_shared(magn, cacheMagn, th_x, th_y, n, m);
    
    //cacheTanges[threadIdx.y+1][threadIdx.x+1] = tanges[i_src];
    //load_to_shared(tanges, cacheTanges, th_x, th_y, n, m);

	ind_y = threadIdx.y+1; ind_x = threadIdx.x+1; 
	__syncthreads();
    float angle = tanges[i_src];
    if (angle < 0) angle = 360 + angle;
    //north && south
    int centerCell = cacheMagn[ind_y][ind_x];
    dest[i_src] = centerCell;
    if ((337.5 <= angle || angle < 22.5) || 
            (157.25 <= angle && angle < 202.5)) {
        if (cacheMagn[ind_y][ind_x+1] > centerCell ||
                cacheMagn[ind_y][ind_x-1] > centerCell) 
                    dest[i_src] = 0;
    } // north-east && south-west 
    else if ((22.5 <= angle && angle < 67.5) ||
            (202.5 <= angle && angle < 247.5)) {
        if (cacheMagn[ind_y-1][ind_x+1] > centerCell || 
                cacheMagn[ind_y+1][ind_x-1] > centerCell) 
                    dest[i_src] = 0;
    } // west && east
    else if ((67.5 <= angle && angle < 112.5) ||
                (247.5 <= angle && angle < 292.5)) {
        if (cacheMagn[ind_y+1][ind_x] > centerCell ||
                cacheMagn[ind_y-1][ind_x] > centerCell) 
                    dest[i_src] = 0;
    } // west-north && east-south
    else if ((112.5 <= angle && angle < 157.5) ||
        (292.5 <= angle || angle < 337.5)) {
        if (cacheMagn[ind_y-1][ind_x-1] > centerCell || 
                cacheMagn[ind_y+1][ind_x+1] > centerCell) 
                    dest[i_src] = 0;
    }

}	

__global__
void oneBfs(int* src, int* dst, int* changed){
	__shared__ int cache[34][S];
	int m = gridDim.x*32;
	int n = gridDim.y*32;
    int th_x = blockIdx.x * 32 + threadIdx.x;
	int th_y = blockIdx.y * 32 + threadIdx.y;
	int i_src = th_y*m + th_x;
	cache[threadIdx.y+1][threadIdx.x+1] = src[i_src];
	load_to_shared(src, cache, th_x, th_y, n, m);
	//we can proceed
	int ind_y = threadIdx.y+1;
	int ind_x = threadIdx.x+1; //it's correct position
	//tu moze dochodzic do malych runtime errorow, jesli rozmiar bedzie za maly
	int queue[QUEUE_SIZE];
	int beg=0, end=0;
	int val = cache[ind_y][ind_x];
	if(val!=0 && val!=-1 && val!=-2){
		if(val < LOW){
			val=(cache[ind_y][ind_x]=0);
		}
		else if(val >= HIGH){
			val=(cache[ind_y][ind_x]=-2);
		}
		else{
			val=(cache[ind_y][ind_x]=-1);
		}
		*changed=1;
	}
	__syncthreads();
	int procInd_x;
	int procInd_y;
	if(val==-1){
		for(int i=-1; i<2; ++i){
			for(int j=-1; j<2; ++j){
				procInd_x = ind_x+i;
				procInd_y = ind_y+j;
				if(cache[procInd_y][procInd_x]==-2){
					//two values are near
					queue[end++] = ind_y;
					queue[end++] = ind_x;
					cache[ind_y][ind_x]=-2;
					*changed=1;
				}
			}
		}
	}
	int x_new;
	int y_new;
	
	
	while(beg!=end){
		procInd_y = queue[beg++];
		procInd_x = queue[beg++];
		for(int i=-1; i<2; ++i){
			for(int j=-1; j<2; ++j){
				x_new = procInd_x+i;
				y_new = procInd_y+j;
				if(cache[y_new][x_new]==-1 && btwn(y_new, 1, 33) 
											&& btwn(x_new, 1, 33)){
					//two values are near
					queue[end++] = y_new;
					queue[end++] = x_new;
					cache[y_new][x_new]=-2;
				}
			}
		}
	}
	__syncthreads();
	dst[i_src] = cache[ind_y][ind_x];
	
}
__global__ 
void final_battle(int* src){
	int m = gridDim.x*32;
    int th_x = blockIdx.x * 32 + threadIdx.x;
	int th_y = blockIdx.y * 32 + threadIdx.y;
	int i_src = th_y*m + th_x;
	int val = src[i_src];
	if(val==-2){
		src[i_src] = 255;
	}
	else{
		src[i_src] = 0;
	}
}

__global__
void gaussianFilter(int * src, int * dest) {
	int m = gridDim.x*32;
    int th_x = blockIdx.x * 32 + threadIdx.x;
	int th_y = blockIdx.y * 32 + threadIdx.y;
	int i_src = th_y*m + th_x;
	
}

}
