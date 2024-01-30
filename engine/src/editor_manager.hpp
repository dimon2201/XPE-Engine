#pragma once

#include <ui.hpp>
#include <texture_manager.hpp>
#include <camera_manager.hpp>

namespace xpe
{
    namespace ui
    {

        class ENGINE_API cImageWindow : public cObject
        {

        public:
            string Title;
            glm::vec2 Resolution;
            glm::vec2 Position;
            cTexture* Image = nullptr;
            ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
            bool Show = false;
            bool ResizeFramebuffer = false;

            cImageWindow(
                const char* title,
                cTexture* image = nullptr,
                const glm::vec2& resolution = { 512, 512 },
                const glm::vec2& position = { 0, 0 }
            ) : Title(title), Image(image), Resolution(resolution), Position(position) {}

            bool Begin();
            void End();
            void Draw();

        private:
            bool m_Initialized = false;
            glm::vec2 m_CurrentFrameSize = { 800, 600 };
            bool m_Focused = false;
            float m_ScrollX;
            float m_ScrollY;
        };

        class ENGINE_API MEditor final
        {

        public:
            static cTexture* ViewportImage;
            static bool EnableSimulation;
            static bool EnableWorldMode;
            static bool EnableMoveMode;
            static bool EnableRotateMode;
            static bool EnableScaleMode;
            static CPerspectiveCamera* SelectedCamera;
            static cScene* SelectedScene;
            static cEntity SelectedEntity;

            static void Init();
            static void Free();
            static void Update(float dt);

        private:
            static void InitFileBrowser();
            static void FreeFileBrowser();

            static void BeginDockspace();
            static void EndDockspace();
            static void DrawLogo();
            static void DrawWindowButtons();
            static void DrawViewportWindow();
            static void DrawGizmoTab();
            static void DrawGizmo();
            static void DrawEntityWindow();
            static void DrawEntity(EntityID entityId, cScene* scene);
            static void DrawComponentWindow();
            static void DrawComponents();
            static void DrawFilesWindow();

        };
    }
}