WXC = wx-config --static=yes

CC = i586-mingw32msvc-g++
CFLAGS = -c -Wall -std=c++14 -g $(shell $(WXC) --cxxflags) -DUNICODE
LDFLAGS = -static -lole32 -loleaut32 -ldplayx -ldxguid -lws2_32 $(shell $(WXC) --libs)
SOURCE_DIR = src
SOURCE_FILES = $(shell find $(SOURCE_DIR) -name *.cpp)
LIBS = lib/dplayx.lib lib/dxguid.lib
OBJECTS = $(SOURCE_FILES:.cpp=.o)
BUILD_DIR = bin
EXECUTABLE = client.exe

all: $(SOURCE_FILES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(LIBS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

run: $(EXECUTABLE)
	wine $(EXECUTABLE)

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

.PHONY: clean run
