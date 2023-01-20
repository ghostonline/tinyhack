@echo off
set PATH=%CD%;%PATH%
"C:\Program Files\CMake\bin\cmake.exe" -DCMAKE_TOOLCHAIN_FILE="%EMSDK%\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake" -DCMAKE_BUILD_TYPE=MinSizeRel -B build_release -G Ninja ..\
