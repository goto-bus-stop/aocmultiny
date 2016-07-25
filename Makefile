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

NICESP_DLL = nicesp.dll
NICESP_SOURCE_DIR = nicesp
NICESP_SOURCE_FILES = $(shell find $(NICESP_SOURCE_DIR) -name *.cpp -or -name *.hpp)

all: $(SOURCE_FILES) $(EXECUTABLE) $(NICESP_DLL)

$(EXECUTABLE): $(OBJECTS) $(LIBS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

run: $(EXECUTABLE)
	wine $(EXECUTABLE)

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

$(NICESP_DLL): $(NICESP_SOURCE_FILES)
	$(CC) \
		-shared \
		-std=c++14 \
		-I./include \
		-o $@ \
		$(NICESP_SOURCE_DIR)/nicesp.cpp \
		-lole32 -ldxguid

.PHONY: clean run
