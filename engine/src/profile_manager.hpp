#pragma once

namespace xpe
{
    namespace core
    {
        using namespace std::chrono;

        struct ENGINE_API sProfile final
        {
            const char* Function;
            const char* File;
            int Line;
            long long BeginTime;
            long long EndTime;
        };

        class ENGINE_API MProfile final
        {

        public:
            static bool Enabled;

            static void BeginSession();
            static void EndSession();
            static void Write(const sProfile& profile);

        private:
            static std::ofstream s_File;
        };

        class ENGINE_API cProfileTimer
        {

        public:
            cProfileTimer(const char* function, const char* file, int line);
            ~cProfileTimer();

        private:
            sProfile m_Profile;
        };
    }
}

#ifdef K_PROFILER
#define Profile(...) xpe::core::cProfileTimer timer##__LINE__(__FUNCTION__, __FILENAME__, __LINE__)
#else
#define Profile(...)
#endif