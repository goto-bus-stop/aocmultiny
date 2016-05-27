build: aocmultiny
	i686-w64-mingw32-g++ aocmultiny/*.hpp aocmultiny/*.cpp

run: build
	wine a.exe
