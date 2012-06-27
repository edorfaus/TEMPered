cmake_minimum_required(VERSION 2.8)

include_directories(../libtempered)

if (UTILS_USE_SHARED_LIB)
	set(TEMPERED_LIB tempered-shared)
else()
	set(TEMPERED_LIB tempered-static)
endif()

add_executable(enumerate enumerate.c ${HIDAPI_STATIC_OBJECT})
target_link_libraries(enumerate ${TEMPERED_LIB} ${HIDAPI_LINK_LIBS})

add_executable(read-all read-all.c ${HIDAPI_STATIC_OBJECT})
target_link_libraries(read-all ${TEMPERED_LIB} ${HIDAPI_LINK_LIBS})

add_executable(read-repeat read-repeat.c ${HIDAPI_STATIC_OBJECT})
target_link_libraries(read-repeat ${TEMPERED_LIB} ${HIDAPI_LINK_LIBS})
