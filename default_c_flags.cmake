# This file initializes the CFLAGS to compile with warnings on by default.
if (MSVC)
	if (CMAKE_C_FLAGS_INIT MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4"
			CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT}"
		)
	else()
		set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} /W4")
	endif()
elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	# We could use -std=c99 except that we're using strdup() which is in POSIX.
	set(CMAKE_C_FLAGS_INIT
		"${CMAKE_C_FLAGS_INIT} -std=gnu99 -Wall -pedantic -Wextra"
	)
endif()
