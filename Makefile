all: rspo-send

bin:
	mkdir bin

rspo-send: bin
	g++ src/$@.cpp -I/usr/local/include -L/usr/local/lib -lwiringPi -o bin/$@
	
clean:
	rm -rf bin

