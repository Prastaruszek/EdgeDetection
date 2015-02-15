#ifndef _FILTER
#define _FILTER


class Filter
{
public:
	Filter(unsigned char* img, int width, int height, Filter* another):img(img), 
							width(width), height(height), 
							another(another){}
	virtual void filter(unsigned char* dst) = 0;  
protected:
	Filter* another;
	unsigned char* img;
	int width;      
	int height; 
};

class SobelFilter : public Filter{
public:
	SobelFilter(unsigned char* img, int width, int height);
	SobelFilter(Filter* another);
	void filter(unsigned char* dst);
};

#endif
