namespace xpe {

    namespace core {

        void* align16_malloc(usize size)
        {

#ifdef WINDOWS
            return _aligned_malloc(size, 16);
#elif  LINUX
            return ::memalign(16, size);
#else
            return main_alloc(size);
#endif

        }

        void align16_free(void* address)
        {

#ifdef WINDOWS
            return _aligned_free(address);
#elif  LINUX
            return ::free(address);
#else
            return main_free(address);
#endif

        }

    }

}