cmake_minimum_required (VERSION 3.5)
project (miniz)

add_library(miniz STATIC miniz.h miniz.c)
target_compile_definitions(miniz
	PUBLIC
		MINIZ_NO_ARCHIVE_APIS
		MINIZ_NO_STDIO
		MINIZ_NO_ZLIB_COMPATIBLE_NAME
)
target_include_directories(miniz
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)
