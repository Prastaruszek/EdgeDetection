#include "CImg.h"

using namespace cimg_library;

void process(unsigned char* src, unsigned char* dst, int width, int height);

template<typename T>
void displayMeOnTheScreen(CImg<T> image){
	CImgDisplay main_disp(image,"Click a point");
	while (!main_disp.is_closed());
	main_disp.wait();
}

void getGrayscaleImage(CImg<unsigned char>& image, unsigned char* grayscale){
	int size = image.size()/image.spectrum();
	if(image.spectrum()==1){
		unsigned char* gray = image.data(0,0,0,0);
		for(int i=0; i<image.size(); ++i){
			grayscale[i]= gray[i];
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
			grayscale[i] = round(0.299*((double)red[i]) 
			+ 0.587*((double)grey[i]) + 0.114*((double)blue[i]));
		}		
	}
}
int main(int argc, char* argv[]) {
	char img[50];
	if(argc>1){
		strncpy(img, "images/", 50);
		strncpy(img+7, argv[1], 43);
	}
	else{
		strncpy(img, "images/lady_color.bmp", 50);
	}
	CImg<unsigned char> image(img);
	printf("width=%d hight=%d depth=%d size=%lu spectrum=%d\n",
				image.width(), image.height(), image.depth(),
								image.size(), image.spectrum());
	int size = image.size()/image.spectrum();
	unsigned char grayscale[size];
	unsigned char dst[size];
	getGrayscaleImage(image, grayscale);
	process(grayscale, dst, image.width(), image.height());
	CImg<unsigned char> outImg(dst, image.width(), image.height(),
									image.depth(), 1);
	displayMeOnTheScreen(outImg);
	return 0;
}
