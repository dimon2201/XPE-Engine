#pragma once

#include <PxPhysicsAPI.h>

namespace xpe {

    namespace physics {

        using namespace core;
        using namespace physx;

        class ENGINE_API PhysicsAllocator : public physx::PxAllocatorCallback
        {

        public:
            ~PhysicsAllocator() override;

            void* allocate(size_t size, const char* typeName, const char* filename, int line) override;
            void deallocate(void* ptr) override;

        };

    }

}