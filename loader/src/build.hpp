#pragma once

// dynamic library
#ifdef LOADER_DLL_BUILD

// defined for loader build
#define LOADER_API __declspec(dllexport)

// defined for client build
#elif defined(LOADER_DLL)

#define LOADER_API __declspec(dllimport)

#else

// if client decided to build static library
#define LOADER_API

#endif