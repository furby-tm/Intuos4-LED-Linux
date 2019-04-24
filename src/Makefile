
all: intuos4-led-check intuos4-led-config 

intuos4-led-check: intuos4-led-check.o WacomIntuos4LED.o icon-library.o
	g++ -o intuos4-led-check icon-library.o WacomIntuos4LED.o intuos4-led-check.o  -lusb-1.0 -lMagick++

intuos4-led-config: intuos4-led-config.o WacomIntuos4LED.o icon-library.o
	g++ -o intuos4-led-config icon-library.o WacomIntuos4LED.o intuos4-led-config.o  -lusb-1.0 -lMagick++

intuos4-led-check.o: intuos4-led-check.cpp WacomIntuos4LED.h
	g++ -c intuos4-led-check.cpp
	
intuos4-led-config.o: intuos4-led-config.cpp WacomIntuos4LED.h
	g++ -c intuos4-led-config.cpp
	
WacomIntuos4LED.o: WacomIntuos4LED.h WacomIntuos4LED.cpp icon-library.cpp
	g++ -c -I/usr/include/ImageMagick WacomIntuos4LED.cpp
	
icon-library.o: icon-library.cpp
	g++ -c icon-library.cpp
	
clean: 
	rm *.o intuos4-led-config intuos4-led-check
