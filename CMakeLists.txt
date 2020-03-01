# *********************************************************************
# LP3_SDL
#       Handles very rudimentary, basic stuff all the other libs need.
# *********************************************************************
cmake_minimum_required(VERSION 3.9.0 FATAL_ERROR)

if(NOT DEFINED PROJECT_NAME)
    set(NOT_SUBPROJECT ON)
endif()

project(Lp3_Sdl CXX)

include(CTest)
include(CMakePackageConfigHelpers)

include("${CMAKE_BINARY_DIR}/conan_paths.cmake" OPTIONAL
        RESULT_VARIABLE using_conan)
if(NOT using_conan)
    include("${CMAKE_MODULE_PATH}/conan_paths.cmake" OPTIONAL
            RESULT_VARIABLE using_conan)
endif()
if(using_conan)
    # If a module creates and exports CMake package configs, this will let us
    # find them instead of the "Find" package scripts Conan generates for the
    # majority of packages which won't otherwise work.
    set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
endif()

# Allow user to ask explicitly to build tests
option(LP3_SDL_Build_Tests "Build tests when BUILD_TESTING is enabled."
       ${NOT_SUBPROJECT})

if(NOT "${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    find_package(sdl2 2.0.9 MODULE REQUIRED)
endif()

add_library(
    lp3_sdl
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/sdl.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/sdl/config.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl.cpp)
set_target_properties(lp3_sdl PROPERTIES OUTPUT_NAME "Lp3_Sdl")

# Mandate the use of at least C++17 by everything that uses this
target_compile_features(lp3_sdl PUBLIC cxx_std_17)

target_include_directories(
    lp3_sdl
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
    PRIVATE src)

# This is built as a "shared library" in sarcastic air quotes. It's only
# made that way to make linking faster, and relies on consumers using the same
# version of the runtime it was built with. IIRC this used to not be that big
# of a problem with VS, but now it is, thus the disabled warnings.
if(BUILD_SHARED_LIBS)
    target_compile_definitions(
        lp3_sdl
        PUBLIC LP3_SDL_API_DYNAMIC
        PRIVATE LP3_SDL_API_CREATE)
    if(MSVC)
        target_compile_options(lp3_sdl PRIVATE /wd4251 /wd4275)
    endif()
endif()

if(NOT "${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    target_link_libraries(lp3_sdl PUBLIC sdl2::sdl2)
else()
    target_compile_options(lp3_sdl PUBLIC "SHELL:-s USE_SDL=2")
    target_link_options(lp3_sdl PUBLIC "SHELL:-s USE_SDL=2")
endif()

# *********************************************************************
# Package / Install Stuff
# *********************************************************************

install(DIRECTORY include/ DESTINATION include)

install(
    TARGETS lp3_sdl
    EXPORT Lp3_Sdl-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES
    DESTINATION include)

add_library(lp3::sdl ALIAS lp3_sdl)
install(
    EXPORT Lp3_Sdl-targets
    FILE Lp3_Sdl-targets.cmake
    NAMESPACE lp3::
    DESTINATION lib/cmake/Lp3_Sdl)

file(
    WRITE "${PROJECT_BINARY_DIR}/Lp3_SdlConfig.cmake"
    "
include(CMakeFindDependencyMacro)
if (NOT \"\${CMAKE_SYSTEM_NAME}\" MATCHES \"Emscripten\")
    find_dependency(sdl2)
endif()
include(\"\${CMAKE_CURRENT_LIST_DIR}/Lp3_Sdl-targets.cmake\")
")

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/Lp3_SdlConfigVersion.cmake"
    VERSION 1.0.1
    COMPATIBILITY AnyNewerVersion)

install(FILES "${PROJECT_BINARY_DIR}/Lp3_SdlConfig.cmake"
              "${PROJECT_BINARY_DIR}/Lp3_SdlConfigVersion.cmake"
        DESTINATION lib/cmake/Lp3_Sdl)

# *********************************************************************
# Tests and Drivers / Demos
# *********************************************************************

if(BUILD_TESTING AND LP3_SDL_Build_Tests)
    find_package(Catch2 CONFIG REQUIRED)

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
    target_link_libraries(sdlrwops_wrapper lp3_sdl Catch2::Catch2)
    make_test(sdlrwops_wrapper)

endif()

get_directory_property(is_child PARENT_DIRECTORY)
if(NOT is_child)
    set(LP3_SDL_FULL True)
endif()

if(LP3_SDL_FULL)
    if(LP3_SETUP_FOLDERS)
        set_target_properties(lp3_sdl PROPERTIES FOLDER lp3_sdl)
    endif()
endif()
