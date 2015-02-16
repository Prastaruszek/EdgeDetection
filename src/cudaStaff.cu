#include<cstdio>

extern "C" {
__global__
void transpose_char(unsigned char* src, unsigned char* dst){

	__shared__ unsigned char cache[32][33];
	int m = gridDim.x*32;
    int th_x = blockIdx.x * 32 + threadIdx.x;
	int th_y = blockIdx.y * 32 + threadIdx.y;
	int i_src = th_y*m + th_x;
	cache[threadIdx.y][threadIdx.x] = src[i_src];
    __syncthreads();
    th_x = blockIdx.y * 32 + threadIdx.x;
	th_y = blockIdx.x * 32 + threadIdx.y;
    m = gridDim.y*32;
    int i_dst = th_y*m+th_x;
    dst[i_dst] = cache[threadIdx.x][threadIdx.y]; 
 
}
__global__
void transpose_short(short* src, short* dst){
	//printf("WE R IN)");
	__shared__ short cache[32][33];
	int m = gridDim.x*32;
    int th_x = blockIdx.x * 32 + threadIdx.x;
	int th_y = blockIdx.y * 32 + threadIdx.y;
	int i_src = th_y*m + th_x;
	cache[threadIdx.y][threadIdx.x] = src[i_src];
    __syncthreads();
    th_x = blockIdx.y * 32 + threadIdx.x;
	th_y = blockIdx.x * 32 + threadIdx.y;
    m = gridDim.y*32;
    int i_dst = th_y*m+th_x;
    dst[i_dst] = cache[threadIdx.x][threadIdx.y]; 
 
}

__global__
void oneDimSobel(unsigned char *src, short *dst, int*N){
	int n = N[0]; // width
	int m = N[1]; // height
	int SIZE = n*m;
	int th_x = blockIdx.x*1024+threadIdx.x;
	if(th_x < n){
		int prev = src[th_x];
		int next=-1;
		for(int i=th_x+n; i<SIZE; i+=n){
			next = src[i];
			int temp = next-prev;
			temp = temp>=0?temp:-temp;
			dst[i] = temp;
			prev = next; 
		}
	}
}
__global__
void sobelPlus(short* A, short* B, unsigned char* dst){
	int m = gridDim.x*32;
    int th_x = blockIdx.x * 32 + threadIdx.x;
	int th_y = blockIdx.y * 32 + threadIdx.y;
	//TU MOZE BYC WOLNO
	int pos = th_y*m+th_x;
	dst[pos] = A[pos]+B[pos]>32?255:0;
}
/*__global__
void columning(int* cc, int* bitmap, int* N, int* myBool){
	int n =  *N;
	int SIZE =  n * n;
    int th_x = blockIdx.x * 1024 + threadIdx.x;
	//printf("thx = %d\n", th_x);
	if(th_x < n){
		int i = th_x+n;
		//printf("nums %d %d\n", i, i-n);
		for(i = th_x+n; i<SIZE; i+=n){
			if(bitmap[i]==bitmap[i-n] && cc[i-n]<cc[i]){
				cc[i] = cc[i-n];
				*myBool = 1;
			}
		}
		i-=n;
		for(i = i-n ;i>=0; i-=n){
			if(bitmap[i]==bitmap[i+n] && cc[i+n]<cc[i]){
				cc[i]=cc[i+n];
				*myBool = 1;
			}
		}
	}
}*/
}


