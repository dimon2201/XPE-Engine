#pragma once

#include <PxPhysicsAPI.h>

namespace xpe
{
    namespace physics
    {
        using namespace physx;

        class ENGINE_API cPhysicsAllocator : public physx::PxAllocatorCallback
        {

        public:
            void* allocate(size_t size, const char* typeName, const char* filename, int line) override;
            void deallocate(void* ptr) override;

        };
    }
}