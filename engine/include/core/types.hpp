#pragma once

#define STR(clazz) #clazz

#define K_MEMORY_KB 1024
#define K_MEMORY_MB 1024 * K_MEMORY_KB
#define K_MEMORY_GB 1024 * K_MEMORY_MB

#define K_PI 3.14159265359f

namespace xpe
{
    namespace core
    {
        using u8 = uint8_t;
        using s8 = int8_t;
        using u16 = uint16_t;
        using s16 = int16_t;
        using u32 = uint32_t;
        using s32 = int32_t;
        using u64 = uint64_t;
        using s64 = int64_t;
        using f32 = float;
        using f64 = double;
        using dword = u32;
        using qword = u64;

        #if defined(__ILP32__) || defined(__arm__) || defined(_M_ARM) || defined(__i386__) || defined(_M_IX86) || defined(_X86_)
            // 32-bit machine
            using ssize = s32;
            using usize = u32;
            using sword = s32;
            using uword = u32;

        #elif defined(__amd64__) || defined(_M_AMD64) || defined(_M_X64) || defined(__aarch64__) || defined(__ia64__) || defined(_M_IA64)
            // 64-bit machine
            using ssize = s64;
            using usize = u64;
            using sword = s64;
            using uword = u64;

        #endif

        using Boolean = u32;

        class ENGINE_API cObject
        {
        public:
            void* operator new(usize byteSize);
            void operator delete(void* address);
        };

        class ENGINE_API cHotObject
        {
        public:
            void* operator new(usize byteSize);
            void operator delete(void* address);
        };

    }
}