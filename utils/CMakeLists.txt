cmake_minimum_required(VERSION 2.8)

include_directories(../libtempered)
include_directories(../libtempered-util)

if (UTILS_USE_SHARED_LIB)
	set(TEMPERED_LIB tempered-shared)
else()
	set(TEMPERED_LIB tempered-static)
endif()

if (UTILS_USE_SHARED_LIB)
	set(TEMPERED_UTIL_LIB tempered-util-shared)
else()
	set(TEMPERED_UTIL_LIB tempered-util-static)
endif()

add_executable(hid-query hid-query.c ${HIDAPI_STATIC_OBJECT})
target_link_libraries(hid-query ${HIDAPI_LINK_LIBS})

add_executable(tempered-exe tempered.c ${HIDAPI_STATIC_OBJECT})
set_target_properties(tempered-exe PROPERTIES OUTPUT_NAME tempered)
target_link_libraries(tempered-exe
	${TEMPERED_LIB} ${TEMPERED_UTIL_LIB} ${HIDAPI_LINK_LIBS}
)

if (DEFINED CMAKE_INSTALL_BINDIR)
	install(TARGETS hid-query tempered-exe DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
