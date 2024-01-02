#include <ecs/ecs.hpp>
#include <ecs/components.hpp>
#include <rendering/bindings.hpp>

#include "particle_manager.hpp"

namespace xpe {

    namespace particle {

        usize cParticleManager::m_EmitterCount = 0;
        s32 cParticleManager::m_LastEmitterIndex = 0;

        void cParticleManager::Init()
        {
        }

        void cParticleManager::Free()
        {
        }

        sParticleEmitter cParticleManager::AddEmitter(usize spawnCount)
        {
            sParticleEmitter emitter;
            emitter.ListIndex = m_LastEmitterIndex;
            emitter.EmitterCount = m_EmitterCount + 1;
            emitter.SpawnCount = spawnCount;

            //Buffers::ParticleEmitter->GetList()[m_LastEmitterIndex] = emitter;
            //Buffers::ParticleEmitter->Flush();

            m_EmitterCount += 1;
            m_LastEmitterIndex += 1;

            return emitter;
        }

        void cParticleManager::RemoveEmitter(const sParticleEmitter& emitter)
        {
            if (m_LastEmitterIndex < 2)
            {
                m_LastEmitterIndex = 0;
                return;
            }

            Buffers::ParticleEmitter->GetList()[emitter.ListIndex] = Buffers::ParticleEmitter->GetList()[m_LastEmitterIndex - 1];

            m_EmitterCount -= 1;
            m_LastEmitterIndex -= 1;
        }

        usize cParticleManager::GetEmitterCount(ecs::cScene* scene)
        {
            auto emitters = scene->GetComponents<CParticleEmitter>();
            m_EmitterCount = emitters.size();

            return m_EmitterCount;
        }

    }

}