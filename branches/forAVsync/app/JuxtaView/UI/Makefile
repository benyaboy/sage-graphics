QUANTA_DIR=/usr/local
CXXFLAGS=-Wno-deprecated -I$(QUANTA_DIR)/include `wx-config --cflags --unicode=no`
LDFLAGS=-L$(QUANTA_DIR)/lib64 -lquanta_64 `wx-config --libs --unicode=no`
CXX=g++
CC=$(CXX)

JuxtaSCUI: JuxtaSCUI.o main.o Overview.o

Overview.o: Overview.cpp
JuxtaSCUI.o: JuxtaSCUI.cpp
main.o: main.cpp

clean:
	rm -rf *.o *~ JuxtaSCUI

