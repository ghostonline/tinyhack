cmake_minimum_required (VERSION 3.8)
project (TinyHack)

set(GAME_TARGET tinyhack)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(TinyTools)
set_extension_variables()

add_subdirectory(engine)

add_executable (${GAME_TARGET} "")

target_link_libraries(${GAME_TARGET} tiny3d)

target_include_directories(${GAME_TARGET} PRIVATE "src")

set_property(TARGET ${GAME_TARGET} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${GAME_TARGET} PROPERTY CXX_STANDARD_REQUIRED ON)

target_compile_definitions(${GAME_TARGET}
	PRIVATE
		DEBUG_BUILD=$<CONFIG:Debug>
)

if(MSVC)
	set_target_properties(${GAME_TARGET} PROPERTIES WIN32_EXECUTABLE TRUE)
	target_compile_definitions(${GAME_TARGET} PRIVATE _CRT_SECURE_NO_WARNINGS)
	target_compile_options(${GAME_TARGET} PRIVATE /W4 /WX)
	target_compile_options(${GAME_TARGET} PRIVATE /wd4100) # Unreferenced formal parameter
	target_compile_options(${GAME_TARGET} PRIVATE /wd4505) # Unreferenced local function
elseif(EMSCRIPTEN)
	target_link_options(${GAME_TARGET}
		PRIVATE
			"SHELL:-s SINGLE_FILE=1"
			--closure $<NOT:$<CONFIG:Debug>>
			"SHELL:-s DEMANGLE_SUPPORT=$<CONFIG:Debug>"
	)
elseif(APPLE)
	set_target_properties(${GAME_TARGET} PROPERTIES
		MACOSX_BUNDLE TRUE
		MACOSX_FRAMEWORK_IDENTIFIER nl.ghostweb.${GAME_TARGET}
	)
endif()

if (APPLE OR EMSCRIPTEN)
	target_compile_options(${GAME_TARGET} PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-missing-braces)
endif()

target_sources(${GAME_TARGET} PRIVATE
	src/main_${EXTENSION_SYSTEM}.cpp
	src/Application.cpp
	src/Application.h
	src/BaseScene.cpp
	src/BaseScene.h
	src/ConsoleTools.cpp
	src/ConsoleTools.h
	src/DeathScene.cpp
	src/DeathScene.h
	src/GameScene.cpp
	src/GameScene.h
	src/HelpScene.cpp
	src/HelpScene.h
	src/Palette.cpp
	src/Palette.h
	src/SceneStack.cpp
	src/SceneStack.h
	src/StringTools.cpp
	src/StringTools.h
	src/TitleScene.cpp
	src/TitleScene.h
	src/UpdateArgs.h

	src/algorithm/PathFinder.cpp
	src/algorithm/PathFinder.h

	src/animation/Animator.cpp
	src/animation/Animator.h
	src/animation/FlashPoint.h
	src/animation/ScanAnimation.h
	src/animation/FocusAnimation.h
	src/animation/ProgressBarExplosion.h

	src/entity/ComponentData.h
	src/entity/Systems.cpp
	src/entity/Systems.h

	src/fov/Fov.cpp
	src/fov/Fov.h
	src/fov/ViewDirection.h
	src/fov/VisibilityMap.cpp
	src/fov/VisibilityMap.h

	src/game/MessageLog.cpp
	src/game/MessageLog.h
	src/game/World.cpp
	src/game/World.h

	src/hud/ProgressBar.cpp
	src/hud/ProgressBar.h

	src/input/Input.cpp
	src/input/Input.h
	src/input/InputAction.h
	src/input/InputAction.h
	src/input/InputAction_Data.h

	src/lang/Lang.cpp
	src/lang/Lang.h
	src/lang/LangData.h

	src/level/Network.cpp
	src/level/Network.h
	src/level/NetworkGenerator.cpp
	src/level/NetworkGenerator.h
	src/level/NetworkTools.cpp
	src/level/NetworkTools.h

	src/resources/ResourceLoader.cpp
	src/resources/ResourceLoader.h
)

get_target_property(SOURCE_FILES ${GAME_TARGET} SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${SOURCE_FILES})
ensure_existing_files(${SOURCE_FILES})

# Somehow the first one is ignored on MacOS, hence the empty dummy.txt file
set(RESOURCE_FILES
	data/terminal16x16_gs_ro.png
	data/title.xp
	data/dummy.txt
)
target_resources(${GAME_TARGET} PRIVATE ${RESOURCE_FILES})
