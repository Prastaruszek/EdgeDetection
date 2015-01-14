#include "CImg.h"

using namespace cimg_library;

template<typename T>
void displayMeOnTheScreen(CImg<T> image){
	CImgDisplay main_disp(image,"Click a point");
	while (!main_disp.is_closed());
	main_disp.wait();
}
//make detect - w katalogu matce zbuduje nam glowny program. Mozna jeszcze zobaczyc
//calkiem ciekawy przyklad - make example - ale tu cos bardziej pod nasze potrzeby
int main() {
	CImg<unsigned char> image("images/lena512_color.bmp");

	printf("width=%d hight=%d depth=%d size=%lu spectrum=%d\n",image.width(), 
					image.height(), image.depth(), image.size(),
									image.spectrum());;
	//for(int i=0; i<image.size(); ++i){
	
	//}
	unsigned char* red = image.data(0,0,0,0);
	unsigned char* grey = image.data(0,0,0,1);
	unsigned char* blue = image.data(0,0,0,2);
	printf("colors have their own line in mem: %lu %lu %lu\n",
							red-red, grey-red, blue-red); 
	char grayscale[image.size()];
	int size =image.size()/image.spectrum();
	for(int i=0; i<size; ++i){
		grayscale[i] = round(0.299*((double)red[i]) 
		+ 0.587*((double)grey[i]) + 0.114*((double)blue[i]));
	}
	CImg<unsigned char> outImg(grayscale, image.width(), image.height(),
									image.depth(), 1);
	displayMeOnTheScreen(outImg);
	return 0;
}
