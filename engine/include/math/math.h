#pragma once

#define PI 3.14159265359f

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

        template<typename T, glm::qualifier Q>
        glm::mat<4, 4, T, Q> lookAt(glm::vec<3, T, Q> const& eye, glm::vec<3, T, Q> const& center, glm::vec<3, T, Q> const& up)
        {
            return glm::lookAt(eye, center, up);
        }

        template<typename T>
        glm::mat<4, 4, T, glm::defaultp> perspective(T fovy, T aspect, T zNear, T zFar)
        {
            return glm::perspective(fovy, aspect, zNear, zFar);
        }

        template<typename T>
        glm::mat<4, 4, T, glm::defaultp> ortho(T left, T right, T bottom, T top, T zNear, T zFar)
        {
            return glm::ortho(left, right, bottom, top, zNear, zFar);
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