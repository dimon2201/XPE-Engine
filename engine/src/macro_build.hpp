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
#ifdef GAMESCRIPT_DLL_BUILD

// defined for game build
#define GAMESCRIPT_API __declspec(dllexport)

// defined for launcher build
#elif defined(GAMESCRIPT_DLL)

#define GAMESCRIPT_API __declspec(dllimport)

#else

#define GAMELIB_API

#endif