WXC = wx-config --static=yes
PKGCONFIG = i686-w64-mingw32-pkg-config
CC = i686-w64-mingw32-g++

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
NICESP_CPPS = $(shell find $(NICESP_SOURCE_DIR) -name *.cpp)
NICESP_OBJECTS = $(NICESP_CPPS:.cpp=.o)
NICESP_SOURCE_FILES = $(shell find $(NICESP_SOURCE_DIR) -name *.cpp -or -name *.hpp)

all: $(SOURCE_FILES) $(EXECUTABLE) $(NICESP_DLL)

$(EXECUTABLE): $(OBJECTS) $(LIBS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

src/%.o: src/%.cpp
	$(CC) $(CFLAGS) $< -o $@

run: $(EXECUTABLE)
	wine $(EXECUTABLE)

clean:
	rm -rf $(OBJECTS) $(NICESP_OBJECTS) $(EXECUTABLE) $(NICESP_DLL)

$(NICESP_DLL): $(NICESP_OBJECTS)
	$(CC) -shared -o $@ $(NICESP_OBJECTS) \
		-lole32 -ldxguid $(shell $(PKGCONFIG) --libs gio-2.0 nice)

nicesp/%.o: nicesp/%.cpp
	$(CC) -c -Wall -std=c++14 -g \
		-I./include \
		$(shell $(PKGCONFIG) --cflags gio-2.0 nice) \
		-DUNICODE \
		$< -o $@

.PHONY: clean run
