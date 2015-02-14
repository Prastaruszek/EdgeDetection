#include <iostream>

#define T 32


void process(unsigned char* src, unsigned char* dst, int width, int height){
	std::cout << "COPYING" << std::endl;
	int magX;
	int magY;
	//simple 1D mask
	for(int i=1; i<height-1; ++i){
		for(int j=1; j<width-1; ++j){
			magX = -src[(i)*width+j-1]+src[i*width+j+1];
			magY = src[(i-1)*width+j]-src[(i+1)*width+j];
			magX = magX>0? magX: -magX;
			magY = magY>0? magY: -magY;
			//printf("%d %d\n", magX, magY);
			dst[i*width+j] = magX+magY>=32?255:0;
		}
	}
}
