cmake_minimum_required(VERSION 2.8)

include_directories(../libtempered)

if (UTILS_USE_SHARED_LIB)
	set(TEMPERED_LIB tempered-shared)
else()
	set(TEMPERED_LIB tempered-static)
endif()

add_executable(hid-query hid-query.c ${HIDAPI_STATIC_OBJECT})
target_link_libraries(hid-query ${HIDAPI_LINK_LIBS})

add_executable(tempered-exe tempered.c ${HIDAPI_STATIC_OBJECT})
set_target_properties(tempered-exe PROPERTIES OUTPUT_NAME tempered)
target_link_libraries(tempered-exe m ${TEMPERED_LIB} ${HIDAPI_LINK_LIBS})

add_executable(enumerate enumerate.c ${HIDAPI_STATIC_OBJECT})
target_link_libraries(enumerate ${TEMPERED_LIB} ${HIDAPI_LINK_LIBS})

install(TARGETS hid-query tempered-exe enumerate DESTINATION bin)
