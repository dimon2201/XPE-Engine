#pragma once

namespace xpe {

    namespace math {

        template<typename T>
        void clamp(T& value, const T min, const T max)
        {
            if (value < min) {
                value = min;
            }
            else if (value > max) {
                value = max;
            }
        }

        template<typename T>
        T min(const T& right, const T& left)
        {
            return right < left ? right : left;
        }

        template<typename T>
        T max(const T& right, const T& left)
        {
            return right > left ? right : left;
        }

    }

}