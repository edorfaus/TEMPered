cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE libtempered_FILES *.[ch])

if (DEFINED CMAKE_INSTALL_INCLUDEDIR)
	install(FILES tempered.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()

if (BUILD_SHARED_LIB)
	add_library(tempered-shared SHARED ${libtempered_FILES})
	set_target_properties(tempered-shared PROPERTIES
		OUTPUT_NAME tempered
		SOVERSION 0
	)
	if (DEFINED CMAKE_INSTALL_LIBDIR)
		install(
			TARGETS tempered-shared
			LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		)
	endif()
endif()

if (BUILD_STATIC_LIB)
	add_library(tempered-static STATIC ${libtempered_FILES})
	set_target_properties(tempered-static PROPERTIES
		OUTPUT_NAME tempered
	)
	if (DEFINED CMAKE_INSTALL_LIBDIR)
		install(
			TARGETS tempered-static
			ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		)
	endif()
endif()
