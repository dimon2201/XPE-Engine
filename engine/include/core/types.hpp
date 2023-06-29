#pragma once

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
        using boolean = uword;

        class ENGINE_API Object
        {
            public:
                Object() {}
                virtual ~Object() {}

                uword GetType();

                void* operator new(size_t byteSize);
                void operator delete(void* address);
        };

        constexpr boolean K_TRUE                    = 1;
        constexpr boolean K_FALSE                   = 0;
        constexpr uword K_GPUAPI_D3D11              = 0;
        constexpr usize K_MEMORY_KIB                = 1024;
        constexpr usize K_MEMORY_MIB                = 1024 * K_MEMORY_KIB;
        constexpr usize K_MEMORY_GIB                = 1024 * K_MEMORY_MIB;
        constexpr usize K_MAX_STRING_BYTE_SIZE      = 128;
        constexpr usize K_MAX_MEMORY_POOL_BYTE_SIZE = 64 * K_MEMORY_MIB;

        template<typename T>
        using Scope = std::unique_ptr<T>;

        template<typename T, typename ... Args>
        ENGINE_API constexpr Scope<T> createScope(Args&& ... args) {
            return std::make_unique<T>(std::forward<Args>(args)...);
        }

        template<typename T>
        using Ref = std::shared_ptr<T>;

        template<typename T, typename ... Args>
        ENGINE_API constexpr Ref<T> createRef(Args&& ... args) {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }

        template<typename T>
        using Weak = std::weak_ptr<T>;

        template<typename T, typename ... Args>
        ENGINE_API constexpr Weak<T> createWeak(Args&& ... args) {
            return std::weak_ptr<T>(std::forward<Args>(args)...);
        }

    }
}