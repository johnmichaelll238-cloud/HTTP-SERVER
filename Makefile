MODE ?= debug
CXX = g++
SRC = src/main.cpp
OUT = ./build/main

ifeq ($(MODE), debug)
	CXXFLAGS = -g O0
else
	CXXFLAGS = -O2
endif

all:
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS)
clean:
	rm -f $(OUT)
	