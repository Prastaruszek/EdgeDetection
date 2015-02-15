#include <iostream>
#include "filters.h"

void process(unsigned char* src, unsigned char* dst, int width, int height){
	std::cout << "COPYING" << std::endl;
	SobelFilter sf(src,width,height);
	Filter& filter = sf;
	filter.filter(dst);
}
