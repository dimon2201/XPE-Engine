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
        struct Viewport;
        struct Pipeline;
        struct RenderPass;

        class ENGINE_API Drawer : public core::Object
        {

        public:
            Drawer(CameraBuffer* cameraBuffer, Shader* shader, RenderTarget* renderTarget, Viewport* viewport);
            virtual ~Drawer();

            virtual void Draw(Scene* scene) = 0;

            void Begin(RenderTarget* renderTarget);
            void End();

        protected:
            void Init();

            Pipeline* m_Pipeline = nullptr;
            RenderPass* m_RenderPass = nullptr;
        };

    }

}