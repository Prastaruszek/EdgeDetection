all: src/detect.cpp
	g++ -o run.exe src/*.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11

clean: 
	-rm run.exe

