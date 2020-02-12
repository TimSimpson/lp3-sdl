cmake_minimum_required(VERSION 3.5)
project(test_package)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

find_package(Lp3_Main CONFIG REQUIRED)
# ~start-doc cmake-example
find_package(Lp3_Sdl CONFIG REQUIRED)

# Create a simple executable which uses SDL2
add_executable(Sdl2Simple WIN32 ${CMAKE_CURRENT_SOURCE_DIR}/Sdl2Simple.cpp)
# Associate lp3_sdl as a dependency. Pulls in SDL2 and SDL2 image libraries
# from BinCrafters. For Emscripten builds, adds appropriate compile and link
# options to include built-in SDL2 support.
target_link_libraries(Sdl2Simple PUBLIC lp3::lp3_sdl)
# ~end-doc cmake-example
target_link_libraries(Sdl2Simple PUBLIC lp3::lp3_main)


if ("${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    set_target_properties(Sdl2Simple PROPERTIES
        SUFFIX ".html"
        LINK_FLAGS "--emrun"
    )
    target_link_options(Sdl2Simple PRIVATE
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/media@/"
    )
endif()
