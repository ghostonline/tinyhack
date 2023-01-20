cmake_minimum_required (VERSION 3.5)
project (glad)

set (SOURCE_FILES
	src/glad.c
)

add_library(glad STATIC ${SOURCE_FILES})
target_include_directories(glad
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
)
