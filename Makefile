all:  detect

detect: src/detect.cpp src/CImg.h
	-g++ -o detect.exe src/detect.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11

example: src/example/example.cpp src/CImg.h
	-g++ -o example.exe src/example/example.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11

clean: 
	-rm run.exe

