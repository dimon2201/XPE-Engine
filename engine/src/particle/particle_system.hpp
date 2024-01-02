#pragma once

namespace xpe {

    namespace particle {

        using namespace core;
        using namespace ecs;
        using namespace render;

        class ENGINE_API cParticleSystem : public cSystem
        {

        public:
            cParticleSystem();
            ~cParticleSystem();
            
            void Update(cScene* scene, const cTime& dt) override final;
        };

    }

}