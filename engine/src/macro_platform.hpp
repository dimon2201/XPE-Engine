#pragma once

#ifdef _WIN32

/* Windows x64/x86 */
#ifdef _WIN64

/* Windows x64  */
#define WINDOWS
#define DX11
#define OPENAL

#else
/* Windows x86 */
#error "x86 Builds are not supported!"

#endif

#elif defined(__APPLE__) || defined(__MACH__)
#define OPENAL

#include <TargetConditionals.h>

	/* TARGET_OS_MAC exists on all the platforms
	 * so we must check all of them (in this order)
	 * to ensure that we're running on MAC
	 * and not some other Apple platform */

	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported!"

	#elif TARGET_OS_IPHONE == 1
		#define IOS
		#error "IOS is not supported!"

	#elif TARGET_OS_MAC == 1
		#define MACOS
        #define OPENAL
		#error "MacOS is not supported!"

    #else
		#error "Unknown Apple platform!"

    #endif

/* We also have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */

#elif defined(__ANDROID__)
#define ANDROID
#define OPENAL
#error "Android is not supported!"

#elif defined(__linux__)

#define LINUX
#define OPENAL
#else

/* Unknown compiler/platform */
#error "Unknown platform!"

#endif

#ifdef DX11
#define GLM_FORCE_LEFT_HANDED
#endif // DX11

#ifdef VK
#endif // VK

#ifdef OGL
#endif // OGL

#ifdef OPENAL
#endif // OPENAL

#ifdef WINDOWS
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif