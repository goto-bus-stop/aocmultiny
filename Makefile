CC = i686-w64-mingw32-g++
CFLAGS = -c -Wall -std=c++14 -g
LDFLAGS = -static -lole32 -loleaut32 -ldplayx -ldxguid -lws2_32
SOURCE_DIR = src
SOURCE_FILES = $(shell find $(SOURCE_DIR) -name *.cpp)
LIBS = lib/dplayx.lib lib/dxguid.lib
OBJECTS = $(SOURCE_FILES:.cpp=.o)
BUILD_DIR = bin
EXECUTABLE = a.exe

all: $(SOURCE_FILES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(LIBS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

run: a.exe
	wine a.exe

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

.PHONY: clean run
