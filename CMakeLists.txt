# *********************************************************************
# LP3_SDL
#       Handles very rudimentary, basic stuff all the other libs need.
# *********************************************************************
cmake_minimum_required(VERSION 3.9.0 FATAL_ERROR)

if(NOT DEFINED PROJECT_NAME)
    set(NOT_SUBPROJECT ON)
endif()

project(lp3-sdl CXX)

include(CTest)
include(CMakePackageConfigHelpers)

# Allow user to ask explicitly to build tests
option(LP3_SDL_Build_Tests "Build tests when BUILD_TESTING is enabled."
       ${NOT_SUBPROJECT})

if(NOT "${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    find_package(SDL2)
    if(TARGET SDL2::SDL2-static)
        set(SDL2_TARGET SDL2::SDL2-static)
    elseif(TARGET SDL2::SDL2)
        set(SDL2_TARGET SDL2::SDL2)
    else()
        # Try using BinCrafter's find package name from pre 2.0.12 versions.
        find_package(sdl2)
        if(TARGET sdl2::sdl2)
            set(SDL2_TARGET sdl2::sdl2)
        else()
            message(FATAL_ERROR "Could not find SDL2!")
        endif()
    endif()
endif()

add_library(
    sdl
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/sdl.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/sdl/config.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl.cpp)
set_target_properties(sdl PROPERTIES OUTPUT_NAME "lp3-sdl")

# Mandate the use of at least C++17 by everything that uses this
target_compile_features(sdl PUBLIC cxx_std_17)

target_include_directories(
    sdl
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
    PRIVATE src)

# This is built as a "shared library" in sarcastic air quotes. It's only
# made that way to make linking faster, and relies on consumers using the same
# version of the runtime it was built with. IIRC this used to not be that big
# of a problem with VS, but now it is, thus the disabled warnings.
if(BUILD_SHARED_LIBS)
    target_compile_definitions(
        sdl
        PUBLIC LP3_SDL_API_DYNAMIC
        PRIVATE LP3_SDL_API_CREATE)
    if(MSVC)
        target_compile_options(sdl PRIVATE /wd4251 /wd4275)
    endif()
endif()

if(NOT "${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    target_link_libraries(sdl PUBLIC "${SDL2_TARGET}")
else()
    target_compile_options(sdl PUBLIC "SHELL:-s USE_SDL=2")
    target_link_options(sdl PUBLIC "SHELL:-s USE_SDL=2")
endif()

# *********************************************************************
# Tests and Drivers / Demos
# *********************************************************************

if(BUILD_TESTING AND LP3_SDL_Build_Tests)
    find_package(Catch2 REQUIRED)

    function(make_test exe_target)
        if("${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
            add_test(NAME "test_${exe_target}"
                     COMMAND node $<TARGET_FILE:${exe_target}>)
        else()
            add_test(NAME "test_${exe_target}" COMMAND ${exe_target})
        endif()
    endfunction()

    add_executable(sdlrwops_wrapper
                   ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_sdlrwops_wrapper.cpp)
    target_link_libraries(sdlrwops_wrapper sdl Catch2::Catch2)
    make_test(sdlrwops_wrapper)

endif()

get_directory_property(is_child PARENT_DIRECTORY)
if(NOT is_child)
    set(LP3_SDL_FULL True)
endif()

if(LP3_SDL_FULL)
    if(LP3_SETUP_FOLDERS)
        set_target_properties(sdl PROPERTIES FOLDER lp3-sdl)
    endif()
endif()

# *********************************************************************
# Package / Install Stuff
# *********************************************************************

install(DIRECTORY include/ DESTINATION include)

install(
    TARGETS sdl
    EXPORT lp3-sdl-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES
    DESTINATION include)

add_library(lp3::sdl ALIAS sdl)
install(
    EXPORT lp3-sdl-targets
    FILE lp3-sdl-targets.cmake
    NAMESPACE lp3::
    DESTINATION lib/cmake/lp3-sdl)

file(
    WRITE "${PROJECT_BINARY_DIR}/lp3-sdlConfig.cmake"
    "
include(CMakeFindDependencyMacro)
if (NOT \"\${CMAKE_SYSTEM_NAME}\" MATCHES \"Emscripten\")
    find_dependency(SDL2)
endif()
include(\"\${CMAKE_CURRENT_LIST_DIR}/lp3-sdl-targets.cmake\")
")

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/lp3-sdlConfigVersion.cmake"
    VERSION 1.0.1
    COMPATIBILITY AnyNewerVersion)

install(
    FILES "${PROJECT_BINARY_DIR}/lp3-sdlConfig.cmake"
          "${PROJECT_BINARY_DIR}/lp3-sdlConfigVersion.cmake"
          "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ConanFindModuleExtra.cmake"
    DESTINATION lib/cmake/lp3-sdl)
