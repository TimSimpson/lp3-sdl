# This is included from the find modules generated by Conan. It lets CMake
# know to tell Emscripten to include it's copy of the SDL2 libraries.

if("${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    LIST(APPEND lp3_sdl_COMPILE_OPTIONS_LIST"SHELL:-s USE_SDL=2")
    # TODO: find a way to add this to target_link_options too
endif()
