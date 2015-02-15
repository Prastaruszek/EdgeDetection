#include "filters.h"

#define T 32


SobelFilter::SobelFilter(unsigned char* img, int width, int height):
						Filter(img,width,height,nullptr){}
						
SobelFilter::SobelFilter(Filter* another): Filter(another->getImg(),
				another->getWidth(),another->getHeight(),another){}
				
void SobelFilter::filter(unsigned char* dst){
	int magX;
	int magY;
	for(int i=1; i<height-1; ++i){
		for(int j=1; j<width-1; ++j){
			magX = -img[(i)*width+j-1]+img[i*width+j+1];
			magY = img[(i-1)*width+j]-img[(i+1)*width+j];
			magX = magX>0? magX: -magX;
			magY = magY>0? magY: -magY;
			dst[i*width+j] = magX+magY>=T?255:0;
		}
	}
}
