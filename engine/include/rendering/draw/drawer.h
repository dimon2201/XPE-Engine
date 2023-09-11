#pragma once

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        using namespace ecs;

        struct CameraBuffer;
        struct Shader;
        struct RenderTarget;
        struct Pipeline;

        class ENGINE_API Drawer : public core::Object
        {

        public:
            Drawer(CameraBuffer* cameraBuffer, Shader* shader, RenderTarget* renderTarget);
            virtual ~Drawer();

            virtual void Draw(Scene* scene) = 0;

            void Begin();
            void End();

            RenderTarget* GetRenderTarget();

        protected:
            void Init();

            Pipeline* m_Pipeline = nullptr;
        };

    }

}