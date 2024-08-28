
default: fight

fight: fight.cxx
	g++ -Wall -O3 -o fight fight.cxx -lGLESv2 -lglfw 

