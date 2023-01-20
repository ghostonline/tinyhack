cmake_minimum_required (VERSION 3.8)
project (Tiny3D)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(TinyTools)
set_extension_variables()

set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)

if (NOT EMSCRIPTEN)
	add_subdirectory(${EXTERN_DIR}/glad)
	add_subdirectory(${EXTERN_DIR}/glfw)
endif()
add_subdirectory(${EXTERN_DIR}/stb)
add_subdirectory(${EXTERN_DIR}/miniz)

add_library (tiny3d "")

if (EMSCRIPTEN)
	target_link_options(tiny3d PUBLIC "SHELL:-s USE_GLFW=3")
else()
	target_link_libraries(tiny3d glad)
	target_link_libraries(tiny3d glfw)
endif()
target_link_libraries(tiny3d stb)
target_link_libraries(tiny3d miniz)

set_property(TARGET tiny3d PROPERTY CXX_STANDARD 11)
set_property(TARGET tiny3d PROPERTY CXX_STANDARD_REQUIRED ON)

target_compile_definitions(tiny3d
	PRIVATE
		DEBUG_BUILD=$<CONFIG:Debug>

	PUBLIC
		BREAKPOINTS_ENABLED=$<CONFIG:Debug>
		ASSERTS_ENABLED=$<CONFIG:Debug>
		OPENGL_DEBUG_ENABLED=$<CONFIG:Debug>
)

if(MSVC)
	target_compile_definitions(tiny3d PUBLIC _CRT_SECURE_NO_WARNINGS _ITERATOR_DEBUG_LEVEL=0)
	target_compile_options(tiny3d PRIVATE /W4 /WX)
	target_compile_options(tiny3d PRIVATE /wd4100) # Unreferenced formal parameter
elseif(APPLE OR EMSCRIPTEN)
	target_compile_options(tiny3d PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-missing-braces)
endif()

target_include_directories(tiny3d PUBLIC src)
target_sources(tiny3d PRIVATE
	src/Util.h
	src/RangeUtil.h
	src/Image.cpp
	src/Image.h
	src/KeyboardState.h
	src/MouseState.h
	src/Random.h
	src/RandomUtil.h
	src/Direction.cpp
	src/Direction.h

	src/algorithm/EasingFunc.h
	src/algorithm/MazeGenerator.cpp
	src/algorithm/MazeGenerator.h

	src/ecs/Aspect.h
	src/ecs/Component.cpp
	src/ecs/Component.h
	src/ecs/ComponentPool.cpp
	src/ecs/ComponentPool.h
	src/ecs/ECS.cpp
	src/ecs/ECS.h
	src/ecs/EntityID.h

	src/os/Window.h
	src/os/Window.cpp
	src/os/GLFW.h
	src/os/Path.h
	src/os/Path.cpp
	src/os/FileSystem.h
	src/os/FileSystem.cpp
	src/os/File.h
	src/os/FileSystem_${EXTENSION_OS}.cpp

	src/gfx/Renderer.cpp
	src/gfx/Renderer.h
	src/gfx/VertexAttributeConfig.h
	src/gfx/GfxRef.h
	src/gfx/ShaderSource.h
	src/gfx/MeshSource.h
	src/gfx/RenderConstants.h

	src/gfx/gl/DebugOpenGL.cpp
	src/gfx/gl/DebugOpenGL.h
	src/gfx/gl/OpenGLConfig.h
	src/gfx/gl/UtilOpenGL.cpp
	src/gfx/gl/UtilOpenGL.h
	src/gfx/gl/PrimitivesOpenGL.h

	src/ds/ArrayView.h
	src/ds/ByteArrayView.h
	src/ds/StringView.h
	src/ds/Size2.h
	src/ds/Rect.h
	src/ds/Color.cpp
	src/ds/Color.h
	src/ds/Range.h
	src/ds/Ref.h
	src/ds/Pool.h
	src/ds/Array2.h
	src/ds/TimeSpan.h

	src/diag/Assert.cpp
	src/diag/Assert.h
	src/diag/Debugger.h
	src/diag/Log.cpp
	src/diag/Log.h
	src/diag/LogArg.cpp
	src/diag/LogArg.h

	src/math/Math.h
	src/math/Math_misc.h
	src/math/Math_matrix.h
	src/math/Math_matrix.cpp
	src/math/Vec2.h
	src/math/Vec3.h
	src/math/Vec4.h
	src/math/Mat22.h
	src/math/Mat33.h
	src/math/Mat44.h
	src/math/Quat.h

	src/io/BinaryStream.h

	src/text/Box.cpp
	src/text/Box.h
	src/text/Console.cpp
	src/text/Console.h
	src/text/ConsoleRenderer_vsh_gl.h
	src/text/ConsoleRenderer_fsh_gl.h
	src/text/ConsoleRenderer_vsh_gles.h
	src/text/ConsoleRenderer_fsh_gles.h
	src/text/ConsoleRenderer.cpp
	src/text/ConsoleRenderer.h
	src/text/FontTexture.h
	src/text/XpImage.cpp
	src/text/XpImage.h
)

get_target_property(SOURCE_FILES tiny3d SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${SOURCE_FILES})
ensure_existing_files(${SOURCE_FILES})
