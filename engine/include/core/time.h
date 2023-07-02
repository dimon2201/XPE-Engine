#pragma once

namespace xpe {

    namespace core {

        typedef const std::chrono::duration<double, std::milli> TimeMillis;

        class ENGINE_API Time final {

        public:
            Time() = default;
            Time(const double millis) : m_Millis(millis) {}

            inline float Millis() const {
                return m_Millis;
            }

            inline float Seconds() const {
                return m_Millis * 0.001f;
            }

            inline float Minutes() const {
                return m_Millis * 0.06f;
            }

            inline float Hours() const {
                return m_Millis * 3.6f;
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

        private:
            float m_Millis = 0.0f;
        };

    }

}