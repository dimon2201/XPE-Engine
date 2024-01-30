#pragma once

#include <PxPhysicsAPI.h>

namespace xpe
{
    namespace physics
    {
        using namespace physx;

        class ENGINE_API cPhysicsErrorCallback : public PxErrorCallback
        {

        public:
            bool EnableInfo;
            bool EnableWarning;
            bool EnableError;

            cPhysicsErrorCallback(bool enableInfo = true, bool enableWarning = true, bool enableError = true)
            : EnableInfo(enableInfo), EnableWarning(enableWarning), EnableError(enableError) {}

            void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override;

        };

        class cPhysicsSimulationEventCallback : public PxSimulationEventCallback
        {
        
        public:
            virtual void onConstraintBreak(PxConstraintInfo*, PxU32) override;
            virtual void onWake(PxActor**, PxU32) override;
            virtual void onSleep(PxActor**, PxU32) override;
            virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
            virtual void onTrigger(PxTriggerPair*, PxU32) override;
            virtual void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) override;

        };
    }
}