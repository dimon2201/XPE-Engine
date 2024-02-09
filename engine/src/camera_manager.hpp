#pragma once

#include <components.hpp>

namespace xpe
{
    namespace render
    {
        class ENGINE_API MCamera final
        {

        public:
            static CPerspectiveCamera* Camera;

            static void Init(int viewWidth, int viewHeight);
            static void Free();

            static glm::vec2 GetPanSpeed();
            static float GetZoomSpeed();

            static void Move();
            static void Pan(const glm::vec2& delta);
            static void ZoomIn();
            static void ZoomOut();
            static void Look(const double x, const double y);

            static void Resize(int width, int height);

            static void ScrollChanged(const double x, const double y);
            static void WindowFrameResized(int width, int height);
            static void WindowRatioChanged(float ratio);
            static void CursorMoved(const double x, const double y);

            static void Flush();

            static glm::vec3 GetUpDirection();
            static glm::vec3 GetRightDirection();
            static glm::vec3 GetForwardDirection();
            static glm::vec3 CalculateFrontPosition();
            static glm::vec3 CalculateBackPosition();
            static glm::vec3 CalculateRightPosition();
            static glm::vec3 CalculateLeftPosition();
            static glm::vec3 CalculateUpPosition();
            static glm::vec3 CalculateDownPosition();
            static glm::quat GetOrientation();

        private:
            static void UpdateProjection();
            static void UpdateView(const glm::vec3& position);

            static int s_ViewWidth;
            static int s_ViewHeight;
            static glm::vec3 s_Position;
        };
    }
}