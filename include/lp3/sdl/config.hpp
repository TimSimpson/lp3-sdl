#ifndef LP3_SDL_CONFIG_HPP
#define LP3_SDL_CONFIG_HPP
#pragma once

#if defined(_WIN32)                                                            \
        && (defined(BUILD_SHARED_LIBS) || defined(LP3_SDL_API_DYNAMIC))
#if defined(LP3_SDL_API_CREATE)
#define LP3_SDL_API __declspec(dllexport)
#else
#define LP3_SDL_API __declspec(dllimport)
#endif
#else
#define LP3_SDL_API
#endif

#endif
