
cmake:
	mkdir -p build
	cd build && cmake ..
	@echo
	@echo "Next actions: cd build ; if necessary, reconfigure CMake ; run make"
	@echo

distclean:
	[ ! -d build ] || rm -r build

.PHONY: cmake distclean
