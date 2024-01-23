#include <ecs/ecs.hpp>
#include <ecs/components.hpp>
#include <rendering/bindings.hpp>

#include "particle_manager.hpp"

namespace xpe {

    namespace particle {

        usize MParticle::m_EmitterCount = 0;

        void MParticle::Init()
        {
        }

        void MParticle::Free()
        {
        }

        void MParticle::Update(cScene* scene, const cTime& dt)
        {
            auto emitters = scene->GetComponents<CParticleEmitter>();

            if (emitters.size() > 0)
            {
                for (auto [entity, emitter] : emitters.each())
                {
                    sParticleEmitter gpuEmitter;
                    gpuEmitter._EmitterCount = MParticle::GetEmitterCount(scene);
                    gpuEmitter.SpawnCount = emitter.SpawnCount;

                    Buffers::ParticleEmitter->Add(gpuEmitter);
                }

                Buffers::ParticleEmitter->Flush();
            }
        }

        sParticleEmitter MParticle::AddEmitter(usize spawnCount)
        {
            sParticleEmitter emitter;
            emitter._BufferOffset = m_EmitterCount;
            emitter._EmitterCount = m_EmitterCount + 1;
            emitter.SpawnCount = spawnCount;
            emitter.MaxLifetime = 10.0f;

            m_EmitterCount += 1;

            return emitter;
        }

        void MParticle::RemoveEmitter(const sParticleEmitter& emitter)
        {
            if (m_EmitterCount < 2)
            {
                m_EmitterCount = 0;
                return;
            }

            Buffers::ParticleEmitter->GetList()[emitter._BufferOffset] = Buffers::ParticleEmitter->GetList()[m_EmitterCount - 1];

            m_EmitterCount -= 1;
        }

        void MParticle::UpdateEmitter(const cEntity& entity, cScene* scene)
        {
            auto& emitter = scene->GetComponent<CParticleEmitter>(entity.GetID());
            auto& transform = scene->GetComponent<CTransform>(entity.GetID());
            emitter.WorldPosition = transform.Position;

            Buffers::ParticleEmitter->GetList()[emitter._BufferOffset] = emitter;
            Buffers::ParticleEmitter->Flush();
        }

        usize MParticle::GetEmitterCount(ecs::cScene* scene)
        {
            auto emitters = scene->GetComponents<CParticleEmitter>();
            m_EmitterCount = emitters.size();

            return m_EmitterCount;
        }

    }

}