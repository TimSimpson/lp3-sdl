# LP3 SDL

[![Travis Build Status](https://travis-ci.org/TimSimpson/lp3-sdl.svg?branch=master)](https://travis-ci.org/TimSimpson/lp3-sdl)
[![Build status](https://ci.appveyor.com/api/projects/status/vqcmfp6sflj902o7/branch/master?svg=true)](https://ci.appveyor.com/project/TimSimpson/lp3-sdl/branch/master)
[ ![Download](https://api.bintray.com/packages/timsimpson/richter/Lp3-Sdl%3ATimSimpson/images/download.svg) ](https://bintray.com/timsimpson/richter/Lp3-Sdl%3ATimSimpson/_latestVersion)


Super basic C++ wrappers for SDL2.

This library requires the [Conan](https://conan.io/) package manager and the [BinCrafter's packages for SDL2](https://bintray.com/bincrafters/public-conan/sdl2%3Abincrafters) and [SDL2 Image](https://bintray.com/bincrafters/public-conan/sdl2_image%3Abincrafters).

Why even use this? There's no great reason really except it smooths over some oddness of working with Emscripten: if you use CMake and rely on the package config installed by this, it will detect if the system name is Emscripten, and if so, correctly state the command line options needed to use SDL2 and avoid pulling libraries. Also it tells Conan to avoid downloading SDL2 for Emscripten builds as well.
