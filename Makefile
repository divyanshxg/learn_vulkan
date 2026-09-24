default:
	cd build ; cmake .. ; cmake --build . -j; cd app ; ./Executable
prepare:
	rm -rf build ; mkdir build 

