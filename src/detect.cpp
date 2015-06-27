#include <ctime>
#include "CImg.h"
using namespace cimg_library;

void process(int* src, int* dst, int width, int height);

template<typename T>
void displayMeOnTheScreen(CImg<T> image){
	CImgDisplay main_disp(image,"Click a point");
	while (!main_disp.is_closed());
	main_disp.wait();
}

void getGrayscaleImage(CImg<unsigned char>& image, int* grayscale){
	int size = image.size()/image.spectrum();
	unsigned char tempGrayscale[size];
	if(image.spectrum()==1){
		unsigned char* gray = image.data(0,0,0,0);
		for(int i=0; i<image.size(); ++i){
			tempGrayscale[i]= gray[i];
		}
	}
	else if(image.spectrum()==3){
		//to mozna zrobic w cudzie, jest to dosc trywialne, ale poki co zostawmy
		unsigned char* red = image.data(0,0,0,0);
		unsigned char* grey = image.data(0,0,0,1);
		unsigned char* blue = image.data(0,0,0,2);
		printf("colors have their own line in mem: %lu %lu %lu\n",
								red-red, grey-red, blue-red);
		for(int i=0; i<size; ++i){
			tempGrayscale[i] = round(0.299*((double)red[i]) 
			+ 0.587*((double)grey[i]) + 0.114*((double)blue[i]));
		}		
	}
	CImg<unsigned char> temp(tempGrayscale, image.height(), image.width(), 
												image.depth(), 1);
	temp.blur(1);
	unsigned char* gray = temp.data(0,0,0,0);
	for(int i=0; i<size; ++i){
		grayscale[i] = gray[i];
	}
	
}
void compress(unsigned char* dst_char, int* dst, int size){
	for(int i=0; i<size; ++i){
		dst_char[i]=dst[i];
		
	}
}
void finish(unsigned char * process, int size, int width, int height){
	for(int i=0; i<width; ++i){
		process[i] = 0;
		process[(height-1)*width+i] = 0;
	}
	for(int i=0; i<height; ++i){
		process[i*width] = 0;
		process[(i+1)*width-1] = 0;
	}
}
int main(int argc, char* argv[]) {
	char img[50]; 
	clock_t timer;
	if(argc>1){
		strncpy(img, "images/", 50);
		strncpy(img+7, argv[1], 43);
	} 
	else{
		strncpy(img, "images/lady_color.bmp", 50);
	}
	CImg<unsigned char> image(img);
	//image.blur(1);
	printf("width=%d hight=%d depth=%d size=%lu spectrum=%d\n",
				image.width(), image.height(), image.depth(),
								image.size(), image.spectrum());
	int size = image.size()/image.spectrum();
	int grayscale[size];
	int dst[size];
	unsigned char dst_char[size];
	
	timer=clock();
	
	getGrayscaleImage(image, grayscale);
	
	process(grayscale, dst, image.width(), image.height());
	
	timer=clock()-timer;
	
	compress(dst_char, dst, size);
	finish(dst_char, size, image.width(), image.height());
	printf("TIME PROCESSING IMAGE: %f\n sec", (float)timer/CLOCKS_PER_SEC);
	CImg<unsigned char> outImg(dst_char, image.width(), image.height(),
									image.depth(), 1);
	displayMeOnTheScreen(outImg);
	outImg.save("dst/lady.bmp");
	return 0;
}
