struct Particle
{
    float3 WorldPosition;
    float3 Velocity;
    float Lifetime;
};

struct ParticleEmitter
{
    int _BufferOffset;
    uint _EmitterCount;
    uint SpawnCount;
    float3 WorldPosition;
    float MaxLifetime;
};

struct ParticlePassInfo
{
    uint ParticleCount;
};

RWStructuredBuffer<Particle> ParticlesUAV : K_SLOT_BUFFER_PARTICLE_UAV;

StructuredBuffer<Particle> ParticlesSRV : K_SLOT_BUFFER_PARTICLE_SRV;

StructuredBuffer<ParticleEmitter> ParticleEmittersSRV : K_SLOT_BUFFER_PARTICLE_EMITTER;

RWStructuredBuffer<ParticlePassInfo> ParticlePassInfoUAV : K_SLOT_BUFFER_PARTICLE_PASS_INFO;