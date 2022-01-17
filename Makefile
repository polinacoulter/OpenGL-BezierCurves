#CXXFLAGS := -Wall -g -std=c++11 -Wno-deprecated
#LDFLAGS := -framework GLUT -framework OpenGL -framework Cocoa
FILES := *.cpp

all:
	g++ -std=c++11 -Wall -Wno-deprecated -o graphics $(FILES) -framework OpenGL -framework GLUT

run: all
	./graphics

clean:
	rm graphics *.o
