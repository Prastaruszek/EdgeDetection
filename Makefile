all:  detect

detect: obj/detect.o obj/processImg.o obj/SobelFilter.o
	g++ obj/detect.o obj/processImg.o obj/SobelFilter.o  -o detect  -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 

obj/detect.o: src/detect.cpp src/CImg.h
	g++ -c src/detect.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -o obj/detect.o

obj/processImg.o: src/processImg.cpp src/CImg.h
	g++ -c src/processImg.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -o obj/processImg.o

obj/SobelFilter.o: src/SobelFilter.cpp src/filters.h
	g++ -c src/SobelFilter.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -o obj/SobelFilter.o	

example: obj/example.o
	g++ obj/example.o  -o example  -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11

obj/example.o: src/example/example.cpp src/CImg.h
	g++ -c src/example/example.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -std=c++11 -o obj/example.o

clean: 
	-rm detect
	-rm example
	-rm obj/*
