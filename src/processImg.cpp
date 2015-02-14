#include <iostream>

void process(unsigned char* src, unsigned char* dst, int width, int height){
	std::cout << "COPYING" << std::endl;
	for(int i=0; i<width*height; ++i){
		dst[i]=src[i];
	}
}
