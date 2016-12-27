# Build toolchain
WXC = wx-config --static=yes
PKGCONFIG = i686-w64-mingw32-pkg-config
CC = i686-w64-mingw32-g++
AR = i686-w64-mingw32-ar

# Common flags
CFLAGS = -c -Wall -std=c++14 -g -I./include -DUNICODE
# Convenience defs for linking DirectPlay/dplib
LD_DPLAY = -ldxguid -ldplayx
LD_DPLIB = -Llib/ -ldplib $(LD_DPLAY)

# Common utilities used by various targets
UTIL_FILES = src/util.cpp
UTIL_OBJECTS = $(UTIL_FILES:.cpp=.o)

# dplib target
DPLIB_SRC = src/dplib
DPLIB_FILES = $(shell find $(DPLIB_SRC) -name "*.cpp" \! -path "*/test/*")
DPLIB_OBJECTS = $(DPLIB_FILES:.cpp=.o) $(UTIL_OBJECTS)
DPLIB_TARGET = lib/dplib.lib

# AoCMultiny Application target
CLIENT_TARGET = client.exe
CLIENT_SRC = src/aocmultiny
CLIENT_FILES = $(shell find $(CLIENT_SRC) -name "*.cpp")
CLIENT_OBJECTS = $(CLIENT_FILES:.cpp=.o) $(UTIL_OBJECTS)
CLIENT_LIBS = lib/dplayx.lib lib/dxguid.lib $(DPLIB_TARGET)
CLIENT_CFLAGS = -Isrc/ $(shell $(WXC) --cxxflags)
CLIENT_LDFLAGS = -static $(LD_DPLIB) -lole32 -loleaut32 -lws2_32 $(shell $(WXC) --libs)

# libnice DirectPlay Service Provider DLL target
NICESP_TARGET = nicesp.dll
NICESP_SRC = src/nicesp
NICESP_FILES = $(shell find $(NICESP_SRC) -name "*.cpp")
NICESP_OBJECTS = $(NICESP_FILES:.cpp=.o) $(DPLIB_OBJECTS)
NICESP_CFLAGS = -Isrc/ $(shell $(PKGCONFIG) --cflags gio-2.0 nice)
NICESP_LDFLAGS = -shared $(LD_DPLAY) -lole32 $(shell $(PKGCONFIG) --libs gio-2.0 nice)

# Tests
TEST_TARGET = test.exe
TEST_FILES = $(shell find src -path "*/test/*.cpp") src/test_main.cpp
TEST_OBJECTS = $(TEST_FILES:.cpp=.o)
TEST_LDFLAGS = $(LD_DPLIB) -lole32

# All objects (used for cleanup)
OBJECTS = \
	$(UTIL_OBJECTS) \
	$(CLIENT_OBJECTS) \
	$(DPLIB_OBJECTS) \
	$(NICESP_OBJECTS) \
	$(TEST_OBJECTS)

# Default: build both targets

all: $(CLIENT_TARGET) $(NICESP_TARGET)

# Library stuffs

lib:
	mkdir lib

$(DPLIB_TARGET): lib $(DPLIB_OBJECTS)
	$(AR) rvs $@ $(DPLIB_OBJECTS)

# Linking

$(CLIENT_TARGET): $(CLIENT_OBJECTS) $(CLIENT_LIBS)
	$(CC) -o $@ $(CLIENT_OBJECTS) $(CLIENT_LDFLAGS)

$(NICESP_TARGET): $(NICESP_OBJECTS)
	$(CC) -o $@ $(NICESP_OBJECTS) $(NICESP_LDFLAGS)

# Compiling

$(DPLIB_SRC)/%.o: $(DPLIB_SRC)/%.cpp
	$(CC) $(CFLAGS) $< -o $@
$(CLIENT_SRC)/%.o: $(CLIENT_SRC)/%.cpp
	$(CC) $(CFLAGS) $(CLIENT_CFLAGS) $< -o $@
$(NICESP_SRC)/%.o: $(NICESP_SRC)/%.cpp
	$(CC) $(CFLAGS) $(NICESP_CFLAGS) $< -o $@
# compile leftovers (utils probably)
src/%.o: src/%.cpp
	$(CC) $(CFLAGS) $< -o $@

# Tests

$(TEST_TARGET): $(DPLIB_TARGET) $(TEST_OBJECTS)
	$(CC) -o $@ $(TEST_OBJECTS) $(TEST_LDFLAGS)

# Running the Application

run: $(CLIENT_TARGET)
	wine $(CLIENT_TARGET)

test: $(TEST_TARGET)
	wine $(TEST_TARGET)

# Cleanups

clean:
	rm -f $(OBJECTS) $(CLIENT_TARGET) $(NICESP_TARGET) $(DPLIB_TARGET) $(TEST_TARGET)

.PHONY: all run clean
