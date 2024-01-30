#include <physics_allocators.hpp>

namespace xpe
{
    namespace physics
    {
        void* cPhysicsAllocator::allocate(size_t size, const char* typeName, const char* filename, int line)
        {
            //hstringstream ss;
            //ss << "\n--------------------- PhysX Allocator -------------------\n\n";
            //ss << "Type: " << typeName << "\n";
            //ss << "File: " << filename << "\n";
            //ss << "sLine: " << line << "\n";
            //ss << "Size: " << size << "\n";
            //ss << "\n---------------------------------------------------------\n";
            //LogInfo(ss.str());

            void* ptr = align16_alloc(size);
            PX_ASSERT((size_t(ptr) & 15)==0)

#if PX_STOMP_ALLOCATED_MEMORY
            if (ptr != NULL)
		    {
			    PxMemSet(ptr, PxI32(0xcd), PxU32(size));
		    }
#endif
            return ptr;
        }

        void cPhysicsAllocator::deallocate(void* ptr)
        {
            //LogInfo("PhysX freeing memory {}", ptr);
            align16_free(ptr);
        }
    }
}