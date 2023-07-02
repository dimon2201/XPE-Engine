#pragma once

namespace xpe {

    namespace math {

        template<typename T>
        void clamp(T& value, const T min, const T max) {
            if (value < min) {
                value = min;
            }
            else if (value > max) {
                value = max;
            }
        }

        template<typename T, glm::qualifier Q>
        glm::mat<4, 4, T, Q> lookAt(glm::vec<3, T, Q> const& eye, glm::vec<3, T, Q> const& center, glm::vec<3, T, Q> const& up) {
            if (core::EngineConfig::GPU_API == core::GraphicsAPI::DX11) {
                return glm::lookAtLH(eye, center, up);
            } else {
                return glm::lookAtRH(eye, center, up);
            }
        }

        template<typename T>
        glm::mat<4, 4, T, glm::defaultp> perspective(T fovy, T aspect, T zNear, T zFar) {
            if (core::EngineConfig::GPU_API == core::GraphicsAPI::DX11) {
                return glm::perspectiveLH(fovy, aspect, zNear, zFar);
            } else {
                return glm::perspectiveRH(fovy, aspect, zNear, zFar);
            }
        }

        template<typename T>
        glm::mat<4, 4, T, glm::defaultp> ortho(T left, T right, T bottom, T top, T zNear, T zFar) {
            if (core::EngineConfig::GPU_API == core::GraphicsAPI::DX11) {
                return glm::orthoLH(left, right, bottom, top, zNear, zFar);
            } else {
                return glm::orthoRH(left, right, bottom, top, zNear, zFar);
            }
        }

    }

}