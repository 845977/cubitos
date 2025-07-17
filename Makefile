DEFAULT_GOAL := main

CXX = g++
CXX_STANDARD := -std=c++14
CXX_FORMAT := clang-format
LIBS = linbox
CXXFLAGS = -O3 -Wall `pkg-config --cflags $(LIBS)`
LDFLAGS = `pkg-config --libs $(LIBS)`

SRC = region.cc csimplex.cc point.cc ccomplex.cc barcode.cc
HEADERS = smatrix.h cubitos.h module.h algorithms/reductions.h

OBJ = $(SRC:.cc=.o)

PREFIX = /usr/local
INSTALL = $(PREFIX)/bin
FILE = cubitos

main: $(HEADERS) $(OBJ) $(INL) main.o config.h
	$(CXX) $(CXXFLAGS) -o $(FILE) main.o $(OBJ) $(LDFLAGS) 

main.o: main.cc config.h $(HEADERS)

%.o: %.cc %.h config.h
	$(CXX) $(CXX_STANDARD) $(CXXFLAGS) -c $< -o $@

format:
	for file in *.cc *.h; do \
		$(CXX_FORMAT) -i $$file ; \
	done

clean:
	rm -rf *.o *.gch $(FILE)
