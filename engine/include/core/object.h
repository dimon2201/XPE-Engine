#pragma once

namespace xpe {

    namespace core {

        class ENGINE_API Object
        {

        public:
            Object() = default;
            ~Object() = default;

            void* operator new(usize byteSize);
            void operator delete(void* address);
        };

        class ENGINE_API HotObject
        {

        public:
            HotObject() = default;
            ~HotObject() = default;

            void* operator new(usize byteSize);
            void operator delete(void* address);
        };

    }

}