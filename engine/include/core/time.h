#pragma once

#ifdef DEBUG

#define LogTime(t) \
LogInfo("Delta time: {} ms", t.Millis()); \
LogInfo("FPS: {}", t.Fps())

#define LogCpuTime(t) \
LogInfo("CPU time: {} ms", t.Millis()); \
LogInfo("CPU FPS: {}", t.Fps())

#else

#define LogTime(t)
#define LogCpuTime(t)

#endif

namespace xpe {

    namespace core {

        typedef const std::chrono::duration<double, std::milli> TimeMillis;

        class ENGINE_API Time final {

        public:
            Time() = default;

            Time(const float millis) : m_Millis(millis) {
                Update();
            }

            Time(const Time& time) : m_Millis(time) {
                Update();
            }

            inline float Millis() const {
                return m_Millis;
            }

            inline float Seconds() const {
                return m_Seconds;
            }

            inline float Minutes() const {
                return m_Minutes;
            }

            inline float Hours() const {
                return m_Hours;
            }

            inline float Fps() const {
                return m_Fps;
            }

            inline void SetFps(float fps) {
                m_Millis = 1000.0f / fps;
                Update();
            }

            friend inline Time operator+(const Time& t1, const Time& t2) {
                return t1.m_Millis + t2.m_Millis;
            }

            friend inline Time operator-(const Time& t1, const Time& t2) {
                return t1.m_Millis - t2.m_Millis;
            }

            friend inline Time operator*(const Time& t1, const Time& t2) {
                return t1.m_Millis * t2.m_Millis;
            }

            friend inline Time operator/(const Time& t1, const Time& t2) {
                return t1.m_Millis / t2.m_Millis;
            }

            friend inline void operator+=(Time& t1, const Time& t2) {
                t1.m_Millis += t2.m_Millis;
            }

            friend inline void operator-=(Time& t1, const Time& t2) {
                t1.m_Millis -= t2.m_Millis;
            }

            friend inline void operator*=(Time& t1, const Time& t2) {
                t1.m_Millis *= t2.m_Millis;
            }

            friend inline void operator/=(Time& t1, const Time& t2) {
                t1.m_Millis /= t2.m_Millis;
            }

            friend inline float operator+(const Time& t1, float t2) {
                return t1.m_Millis + t2;
            }

            friend inline float operator-(const Time& t1, float t2) {
                return t1.m_Millis - t2;
            }

            friend inline float operator*(const Time& t1, float t2) {
                return t1.m_Millis * t2;
            }

            friend inline float operator/(const Time& t1, float t2) {
                return t1.m_Millis / t2;
            }

            friend inline void operator+=(Time& t1, float t2) {
                t1.m_Millis += t2;
            }

            friend inline void operator-=(Time& t1, float t2) {
                t1.m_Millis -= t2;
            }

            friend inline void operator*=(Time& t1, float t2) {
                t1.m_Millis *= t2;
            }

            friend inline void operator/=(Time& t1, float t2) {
                t1.m_Millis /= t2;
            }

            inline Time& operator=(float f) {
                m_Millis = f;
                Update();
                return *this;
            }

            inline operator float() const {
                return m_Millis;
            }

        private:

            void Update() {
                m_Fps     = 1000.0f  / m_Millis;
                m_Seconds = m_Millis * 0.001f;
                m_Minutes = m_Millis * 0.06f;
                m_Hours   = m_Millis * 3.6f;
            }

        private:
            float m_Fps = 0;
            float m_Millis = 0.0f;
            float m_Seconds = 0.0f;
            float m_Minutes = 0.0f;
            float m_Hours = 0.0f;
        };

    }

}