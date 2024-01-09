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
            glm::vec3 WorldPosition;
            glm::vec3 Velocity;
            float Lifetime = 0.0f;
        };

        struct ENGINE_API sParticleEmitter
        {
            s32 _BufferOffset;
            u32 _EmitterCount;
            u32 SpawnCount;
            glm::vec3 WorldPosition;
        };

        class ENGINE_API cParticleManager final
        {

        public:
            static void Init();
            static void Free();

            static sParticleEmitter AddEmitter(usize spawnCount);
            static void RemoveEmitter(const sParticleEmitter& emitter);
            static void UpdateEmitter(const cEntity& entity, cScene* scene);

            static usize GetEmitterCount(cScene* scene);

        private:
            static usize m_EmitterCount;

        };

    }

}