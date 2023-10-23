#pragma once

#include <PxPhysicsAPI.h>

namespace xpe {

    namespace physics {

        using namespace core;
        using namespace physx;

        class ENGINE_API PhysicsErrorCallback : public PxErrorCallback {

        public:
            bool EnableInfoLog;
            bool EnableWarningLog;
            bool EnableErrorLog;

            PhysicsErrorCallback(bool enableInfoLog = true, bool enableWarningLog = true, bool enableErrorLog = true);

            ~PhysicsErrorCallback() override;

            void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override;

        };

    }

}