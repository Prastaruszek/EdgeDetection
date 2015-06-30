#include <ctime>
#include "CImg.h"
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cimg_library;

void process(int* src, int* dst, int width, int height);

template<typename T>
void displayMeOnTheScreen(CImg<T> image){
	CImgDisplay main_disp(image,"Click a point");
	while (!main_disp.is_closed());
	main_disp.wait();
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
	
	cv::Mat image = cv::imread(img);
	cv::Mat grayscale, blured;
	
	cvtColor(image, grayscale, CV_RGB2GRAY);
	
	blur( grayscale, blured, cv::Size(4,4) );
	
	
	int height = image.rows,
		width = image.cols;
	
	//std::cout  << "height" << height << std::endl;
	std::vector<int> arr;
	arr.assign(blured.datastart, blured.dataend);
	int dst[arr.size()];
	std::cout << "size = " << arr.size()/512 << std::endl;
	

	
	
	process(arr.data(), dst, height, width);
	unsigned char mid[arr.size()];
	for(int i=0; i<arr.size(); ++i){
		mid[i]=dst[i];
	}
	
	cv::Mat result(height, width, CV_8UC1, mid);
	cv::imshow("Image", result);

    cv::waitKey(0);

	
	return 0;
}
