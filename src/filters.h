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
protected:
	Filter* another;
	int* img;
	int width;      
	int height; 
};

void sobelFilter(int* src, int* dst, int width, int height);

#endif
