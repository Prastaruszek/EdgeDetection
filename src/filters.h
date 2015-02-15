#ifndef _FILTER
#define _FILTER


class Filter
{
public:
	virtual void filter(unsigned char* dst) = 0;  
};

class SobelFilter : public Filter{
public:
	SobelFilter(unsigned char* img, int width, int height);
	void filter(unsigned char* dst);
private:
	unsigned char* img;
	int width;      
	int height;   
	
};

#endif
