cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE libtempered_FILES *.[ch])

if (BUILD_SHARED_LIB)
	add_library(tempered-shared SHARED ${libtempered_FILES})
	set_target_properties(tempered-shared PROPERTIES
		OUTPUT_NAME tempered
		SOVERSION 0
	)
endif()

if (BUILD_STATIC_LIB)
	add_library(tempered-static STATIC ${libtempered_FILES})
	set_target_properties(tempered-static PROPERTIES
		OUTPUT_NAME tempered
	)
endif()
