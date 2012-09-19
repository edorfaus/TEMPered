
help:
	@echo
	@echo "This project uses the CMake build system."
	@echo
	@echo "This means that to build it, you should create a directory somewhere,"
	@echo "cd to it, and run cmake there to generate the out-of-tree build"
	@echo "structures - if necessary, reconfigure it for your system - and then"
	@echo "run make in that directory."
	@echo
	@echo "For more information on how to use CMake, see http://cmake.org/"
	@echo
	@echo "As a convenience, the cmake target in this makefile will generate a"
	@echo "build dir and run cmake in it for you, while distclean will remove it."
	@echo

cmake:
	mkdir -p build
	cd build && cmake .. || true
	@echo
	@echo "Next actions: cd build ; if necessary, reconfigure CMake ; run make"
	@echo

distclean:
	[ ! -d build ] || rm -r build

.PHONY: help cmake distclean
