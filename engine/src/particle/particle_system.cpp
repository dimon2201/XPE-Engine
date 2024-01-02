#include <ecs/ecs.hpp>
#include <ecs/components.hpp>
#include <rendering/bindings.hpp>

#include "particle_manager.hpp"
#include "particle_system.hpp"

namespace xpe {

    namespace particle {

        cParticleSystem::cParticleSystem()
        {
        }

        cParticleSystem::~cParticleSystem()
        {
        }

        void cParticleSystem::Update(cScene* scene, const cTime& dt)
        {
            auto emitters = scene->GetComponents<CParticleEmitter>();

            if (emitters.size() > 0)
            {
                for (auto [entity, emitter] : emitters.each())
                {
                    sParticleEmitter gpuEmitter;
                    gpuEmitter.EmitterCount = cParticleManager::GetEmitterCount(scene);
                    gpuEmitter.SpawnCount = emitter.SpawnCount;

                    Buffers::ParticleEmitter->Add(gpuEmitter);
                }

                Buffers::ParticleEmitter->Flush();
            }
        }

    }

}