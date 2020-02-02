# *********************************************************************
# LP3_SDL
#       Handles very rudimentary, basic stuff all the other libs need.
# *********************************************************************
project(Lp3_Sdl CXX)
cmake_minimum_required(VERSION 3.9.0 FATAL_ERROR)
include(CMakePackageConfigHelpers)

enable_testing()

include("${CMAKE_BINARY_DIR}/conan_paths.cmake"
        OPTIONAL
        RESULT_VARIABLE using_conan)

if(using_conan)
    set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
endif()

if (NOT "${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    find_package(sdl2 2.0.9 MODULE REQUIRED)
    find_package(sdl2_image 2.0.5 REQUIRED)
endif()

add_library(lp3_sdl
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/sdl.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/sdl/config.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl.cpp
)
set_target_properties(lp3_sdl PROPERTIES OUTPUT_NAME "Lp3_Sdl")

# Mandate the use of at least C++17 by everything that uses this
target_compile_features(lp3_sdl
    PUBLIC
    cxx_std_17
)

target_include_directories(lp3_sdl
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src)

# This is built as a "shared library" in sarcastic air quotes. It's only
# made that way to make linking faster, and relies on consumers using the same
# version of the runtime it was built with. IIRC this used to not be that big
# of a problem with VS, but now it is, thus the disabled warnings.
if(BUILD_SHARED_LIBS)
    target_compile_definitions(lp3_sdl
        PUBLIC  LP3_SDL_API_DYNAMIC
        PRIVATE LP3_SDL_API_CREATE
    )
    if (MSVC)
        target_compile_options(lp3_sdl PRIVATE /wd4251 /wd4275)
    endif()
endif()

if (NOT "${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    target_link_libraries(lp3_sdl
        PUBLIC sdl2::sdl2
               sdl2_image::sdl2_image
    )
else()
    target_compile_options(lp3_sdl PUBLIC
        "SHELL:-s USE_SDL=2"
        "SHELL:-s USE_SDL_IMAGE=2"
        "SHELL:-s SDL2_IMAGE_FORMATS=[\"png\",\"bmp\"]"
    )
    target_link_options(lp3_sdl PUBLIC
        "SHELL:-s USE_SDL=2"
        "SHELL:-s USE_SDL_IMAGE=2"
        "SHELL:-s SDL2_IMAGE_FORMATS=[\"png\",\"bmp\"]"
    )
endif()


# *********************************************************************
# Package / Install Stuff
# *********************************************************************

install(DIRECTORY include/ DESTINATION include)

install(TARGETS lp3_sdl EXPORT Lp3_Sdl-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

add_library(lp3::core ALIAS lp3_sdl)
install(EXPORT Lp3_Sdl-targets
    FILE Lp3_Sdl-targets.cmake
    NAMESPACE lp3::
    DESTINATION lib/cmake/Lp3_Sdl
)

file(WRITE "${PROJECT_BINARY_DIR}/Lp3_Sdl-config.cmake" "
include(CMakeFindDependencyMacro)
find_dependency(sdl2)
include(\"\${CMAKE_CURRENT_LIST_DIR}/Lp3_Sdl-targets.cmake\")
")

write_basic_package_version_file("${PROJECT_BINARY_DIR}/Lp3_Sdl-config-version.cmake"
    VERSION 1.0.1
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${PROJECT_BINARY_DIR}/Lp3_Sdl-config.cmake"
    "${PROJECT_BINARY_DIR}/Lp3_Sdl-config-version.cmake"
    DESTINATION lib/cmake/Lp3_Sdl
)

# *********************************************************************
# Tests and Drivers / Demos
# *********************************************************************

get_directory_property(is_child PARENT_DIRECTORY)
if (NOT is_child)
    set(LP3_SDL_FULL True)
endif()

if (LP3_SDL_FULL)
    add_executable(Sdl2Simple WIN32
                   ${CMAKE_CURRENT_SOURCE_DIR}/demos/Sdl2Simple.cpp)
    target_link_libraries(Sdl2Simple lp3_sdl)
    if ("${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
        set_target_properties(Sdl2Simple PROPERTIES
            SUFFIX ".html"
            LINK_FLAGS "--emrun"
        )
        target_link_options(Sdl2Simple PRIVATE
            "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/ci/media@/"
        )
    endif()


    if (LP3_SETUP_FOLDERS)
        set_target_properties (lp3_sdl PROPERTIES FOLDER core)
        set_target_properties (Sdl2Simple PROPERTIES FOLDER core)
    endif()
    if(BUILD_SHARED_LIBS)
        if (MSVC)
            target_compile_options(Sdl2Simple PRIVATE /wd4251 /wd4275)
        endif()
    endif()
endif()

