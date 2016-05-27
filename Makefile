# Requires DirectX SDK stuff to be symlinked into ./lib for now
build: aocmultiny lib/dplayx.lib lib/dxguid.lib
	i686-w64-mingw32-g++ \
		-Wall \
		-static \
		-L ./lib \
		aocmultiny/*.hpp aocmultiny/*.cpp \
		-lole32 -loleaut32 \
		-ldplayx -ldxguid

run: build
	wine a.exe
