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
    # Note: I'm on Windows right now. If I don't put MODULE here, the code will
    # pull in the target SDL2::SDL2-static, and NOT "sdl2::sdl2". But later,
    # when test_package tries to use the installed config file, it will call
    # `find_package(sdl2)` and pull in a version which only has the target
    # `sdl2::sdl2`!
    find_package(SDL2 2.0.9 REQUIRED)
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

# Stupid Conan can't just name the effing libraries the same as the rest of the
# world. Check it out:
# https://docs.conan.io/en/latest/integrations/build_system/cmake/find_packages.html
# https://www.reddit.com/r/cpp/comments/bsjr6r/how_to_nonintrusively_try_the_conan_package/
if(NOT "${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    if (TARGET SDL2::SDL2)
        set(SDL2_TARGET_NAME "SDL2::SDL2")
        target_link_libraries(lp3_sdl PUBLIC SDL2::SDL2)
    elseif(TARGET SDL2::SDL2-static)
        set(SDL2_TARGET_NAME "SDL2::SDL2-static")
        target_link_libraries(lp3_sdl PUBLIC SDL2::SDL2-static)
    else()
        message(FATAL_ERROR "Could not find target SDL2::SDL2 or SDL2::SDL2-static!")
    endif()
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
    # On Windows, Conan generated Findsdl2.cmake may get used here instead of
    # the one we want. If that's happened, we can tell because the targets
    # we need will be missing. So make aliases of the Conan targets instead.
    find_dependency(SDL2)
    if (NOT TARGET ${SDL2_TARGET_NAME} AND TARGET \"sdl2::sdl2\")
        add_library(${SDL2_TARGET_NAME} INTERFACE IMPORTED)
        set_target_properties(${SDL2_TARGET_NAME} PROPERTIES INTERFACE_LINK_LIBRARIES \"sdl2::sdl2\")
    endif()
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
