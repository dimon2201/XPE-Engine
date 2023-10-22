#pragma once

// dynamic library
#ifdef ENGINE_DLL_BUILD

// defined for engine build
#define ENGINE_API __declspec(dllexport)

// defined for client build
#elif defined(ENGINE_DLL)

#define ENGINE_API __declspec(dllimport)

#else

// if client decided to build static library
#define ENGINE_API

#endif

// dynamic library
#ifdef GAME_DLL_BUILD

// defined for game build
#define GAME_API __declspec(dllexport)

// defined for launcher build
#elif defined(GAME_DLL)

#define GAME_API __declspec(dllimport)

#else

#define GAME_API

#endif