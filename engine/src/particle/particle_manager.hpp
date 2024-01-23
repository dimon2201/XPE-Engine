#pragma once

#include <core/types.hpp>

namespace xpe
{

    namespace ecs
    {
        class cScene;
    }

    namespace particle
    {

        using namespace core;
        using namespace ecs;

        struct ENGINE_API sParticle final
        {
            glm::vec3 WorldPosition;
            glm::vec3 Velocity;
            f32 Lifetime = 0.0f;
        };

        struct ENGINE_API sParticleEmitter
        {
            s32 _BufferOffset = 0;
            u32 _EmitterCount = 0;
            u32 SpawnCount = 0;
            glm::vec3 WorldPosition = glm::vec3(0.0f);
            f32 MaxLifetime = 0.0f;
        };

        class ENGINE_API MParticle final
        {

        public:
            static void Init();
            static void Free();

            static void Update(cScene* scene, const cTime& dt);

            static sParticleEmitter AddEmitter(usize spawnCount);
            static void RemoveEmitter(const sParticleEmitter& emitter);
            static void UpdateEmitter(const cEntity& entity, cScene* scene);

            static usize GetEmitterCount(cScene* scene);

        private:
            static usize m_EmitterCount;

        };

    }

}