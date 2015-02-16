#include <iostream>
#include "filters.h"

void process(unsigned char* src, unsigned char* dst, int width, int height){
	std::cout << "COPYING" << std::endl;
	Filter::initCuda();
	SobelFilter sf(src,width,height);
	Filter& filter = sf;
	filter.filter(dst);  
}
