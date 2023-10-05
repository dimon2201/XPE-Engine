#pragma once

#include <PxPhysicsAPI.h>

namespace xpe {

    namespace physics {

        using namespace core;
        using namespace physx;

        class ENGINE_API PhysicsErrorCallback : public PxErrorCallback {

        public:
            bool EnableInfo;
            bool EnableWarning;
            bool EnableError;

            PhysicsErrorCallback(bool enableInfo = true, bool enableWarning = true, bool enableError = true);

            ~PhysicsErrorCallback() override;

            void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override;

        };

    }

}