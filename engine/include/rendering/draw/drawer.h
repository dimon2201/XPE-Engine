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
            Drawer(CameraBuffer* cameraBuffer, Shader* shader);
            virtual ~Drawer();

            virtual void Draw(Scene* scene, RenderTarget* renderTarget) = 0;

        protected:
            void Init();
            void Bind();

            Pipeline* m_Pipeline = nullptr;
        };

    }

}