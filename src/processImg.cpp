#include <iostream>
#include "filters.h"

void bfs(int* src, int* dst, int width, int height);

void process(int* src, int* dst, int width, int height){
	std::cout << "COPYING" << std::endl;
	Filter::initCuda();
	SobelFilter sf(src,width,height);
	Filter& filter = sf;
	int middle[width*height];
	filter.filter(middle);
	bfs(middle, dst, width, height);
}
