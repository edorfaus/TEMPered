cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE libtempered_util_FILES *.[ch])

install(FILES tempered-util.h DESTINATION include)

if (BUILD_SHARED_LIB)
	add_library(tempered-util-shared SHARED ${libtempered_util_FILES})
	set_target_properties(tempered-util-shared PROPERTIES
		OUTPUT_NAME tempered-util
		SOVERSION 0
	)
	target_link_libraries(tempered-util-shared m)
	install(TARGETS tempered-util-shared LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

if (BUILD_STATIC_LIB)
	add_library(tempered-util-static STATIC ${libtempered_util_FILES})
	set_target_properties(tempered-util-static PROPERTIES
		OUTPUT_NAME tempered-util
	)
	target_link_libraries(tempered-util-static m)
	install(TARGETS tempered-util-static ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
