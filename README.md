# TinyHack

TinyHack is a turn based game where you need to outmaneuver your enemies while keeping an eye on your downloads and hacks. Every few turns a new AI enters the network and a full network scan will alert the nearest enemy to your position. You have to stay mobile and prevent being boxed in.

The latest version of the game is available on [Itch.io](https://xenonghost.itch.io/tinyhack).

## Licensing

Unless explicitly stated otherwise, all code and assets are licensed under the GNU General Public License v3.0. Its text can be found in the `COPYING` file.

`ninja.exe` is obtained from the official [Ninja website](https://ninja-build.org/), and falls under a separate [license](https://github.com/ninja-build/ninja/blob/master/COPYING).

Take extra care with the source code in the `engine/external` folder, these have often their own copyright and licensing notices.

Lastly, `terminal16x16_gs_ro.png` is copied from [LibTCOD](https://github.com/libtcod/libtcod/tree/main/data/fonts). It is considered free to use, but its exact license is not known.

## Building and running

TinyHack uses CMake to generate project files. The supported platforms are Windows, HTML5 and MacOS. Linux should be supported as well, although, this has never been tested.

### Windows

#### Prerequisites:

* Visual Studio 2019
* [CMake](https://cmake.org/)

#### Build steps:

Generate a VS2019 solution using CMake and compile using VS2019.

To run, make sure that the contents of the `data` folder are in the current working directory.

### HTML5 (built on Windows)

#### Prerequisites:

* [Emscripten SDK](https://emscripten.org/) (make sure to install the `latest` version)
* [CMake](https://cmake.org/)
* [Ninja](https://ninja-build.org/) (Windows version is already included in the repository)

#### Build steps:

1. Start a cmdline with the Emscripten environment active (run `emsdk_env.bat` in an existing prompt or `emcmdprompt.bat`)
2. Ensure that the path to `ninja.exe` is added to `PATH` variable
3. Run the following command to generate the project files (use `Debug` or `MinSizeRel` for `<configuration>`):

    `"<path to cmake>\cmake.exe" -DCMAKE_TOOLCHAIN_FILE="%EMSDK%\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake" -DCMAKE_BUILD_TYPE=<configuration> -B <build folder> -G Ninja ..\`

4. Navigate to the `<build folder>` specified in the previous command and execute the following command to build the project:

    `ninja.exe`

5. Copy `tinyhack.js` from the build folder and `index.html` from the `emscripten` folder to a common folder
6. Open `index.html` to open a page with the game embedded in it

### MacOS

#### Prerequisites:

* XCode
* [CMake](https://cmake.org/)

#### Build steps:

Generate a XCode project using CMake and compile using XCode.

This generates an app that contains all code and data.
