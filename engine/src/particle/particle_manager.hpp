#pragma once

#include <core/types.hpp>

namespace xpe {

    namespace ecs
    {
        class cScene;
    }

    namespace particle {

        using namespace core;
        using namespace ecs;

        struct ENGINE_API sParticle final
        {
            glm::mat4 ModelMatrix;
        };

        struct ENGINE_API sParticleEmitter
        {
            s32 ListIndex;
            u32 EmitterCount;
            u32 SpawnCount;
        };

        class ENGINE_API cParticleManager final
        {

        public:
            static void Init();
            static void Free();

            static sParticleEmitter AddEmitter(usize spawnCount);
            static void RemoveEmitter(const sParticleEmitter& emitter);

            static usize GetEmitterCount(cScene* scene);

        private:
            static usize m_EmitterCount;
            static s32 m_LastEmitterIndex;

        };

    }

}