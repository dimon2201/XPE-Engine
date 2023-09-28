#pragma once

#include <rendering/core/core.h>

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        using namespace ecs;

        struct Shader;
        struct RenderTarget;
        struct Pipeline;

        class ENGINE_API Drawer : public core::Object
        {

        public:
            Drawer(Shader* shader, RenderTarget* renderTarget,
                   const vector<Buffer*>& VSBuffers = {},
                   const vector<Buffer*>& PSBuffers = {}
            );

            virtual ~Drawer();

            virtual void Draw(Scene* scene) = 0;

            void Begin();
            void End();

        protected:
            void Init();

            Pipeline* m_Pipeline = nullptr;
        };

    }

}