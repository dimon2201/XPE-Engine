#pragma once

#include <rendering/camera_manager.h>
#include <rendering/shadow_manager.h>
#include <rendering/skybox_manager.h>

#include <ecs/components.hpp>

namespace xpe
{
    namespace render
    {
        class cCanvas;
        class cRenderSystem;
        class sRenderTarget;
    }

    namespace anim
    {
        class cAnimSystem;
    }

    namespace audio
    {
        class cAudioSystem;
    }

    namespace ecs
    {
        class cScene;
    }

    namespace physics
    {
        class cPhysicsSystem;
    }

    namespace core {

        class ENGINE_API cApp : public cObject {

        public:
            cTime CPUTime;              // delta CPU time ms = end time - begin time
            cTime DeltaTime;            // delta CurrentTime ms = end time - begin time
            cTime CurrentTime;
            sAppConfig Config;

            cApp() {}
            ~cApp() {}

            void Run();

        protected:

            virtual void Init() {}
            virtual void Update() {}
            virtual void Free() {}
            virtual void Render();

            virtual void InitShaders();

            void LockFPSFromConfig();

            std::atomic_bool m_Opened = true;

            cScene* m_Scene = nullptr;

            cCanvas* m_Canvas = nullptr;

            sViewport m_Viewport;

            cRenderSystem* m_RenderSystem = nullptr;
            cAnimSystem* m_AnimSystem = nullptr;
            cAudioSystem* m_AudioSystem = nullptr;
            cPhysicsSystem* m_PhysicsSystem = nullptr;
        };
    }
}

xpe::core::cApp* CreateApplication();