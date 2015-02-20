#include <iostream>
#include "filters.h"

void process(int* src, int* dst, int width, int height){
	std::cout << "COPYING" << std::endl;
	Filter::initCuda();
	SobelFilter sf(src,width,height);
	Filter& filter = sf;
	filter.filter(dst);  
}
