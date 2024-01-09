#include <ecs/ecs.hpp>
#include <ecs/components.hpp>
#include <rendering/bindings.hpp>

#include "particle_manager.hpp"

namespace xpe {

    namespace particle {

        usize cParticleManager::m_EmitterCount = 0;

        void cParticleManager::Init()
        {
        }

        void cParticleManager::Free()
        {
        }

        sParticleEmitter cParticleManager::AddEmitter(usize spawnCount)
        {
            sParticleEmitter emitter;
            emitter._BufferOffset = m_EmitterCount;
            emitter._EmitterCount = m_EmitterCount + 1;
            emitter.SpawnCount = spawnCount;

            m_EmitterCount += 1;

            return emitter;
        }

        void cParticleManager::RemoveEmitter(const sParticleEmitter& emitter)
        {
            if (m_EmitterCount < 2)
            {
                m_EmitterCount = 0;
                return;
            }

            Buffers::ParticleEmitter->GetList()[emitter._BufferOffset] = Buffers::ParticleEmitter->GetList()[m_EmitterCount - 1];

            m_EmitterCount -= 1;
        }

        void cParticleManager::UpdateEmitter(const cEntity& entity, cScene* scene)
        {
            auto& emitter = scene->GetComponent<CParticleEmitter>(entity.GetID());
            auto& transform = scene->GetComponent<CTransform>(entity.GetID());
            emitter.WorldPosition = transform.Position;

            Buffers::ParticleEmitter->GetList()[emitter._BufferOffset] = emitter;
            Buffers::ParticleEmitter->Flush();
        }

        usize cParticleManager::GetEmitterCount(ecs::cScene* scene)
        {
            auto emitters = scene->GetComponents<CParticleEmitter>();
            m_EmitterCount = emitters.size();

            return m_EmitterCount;
        }

    }

}