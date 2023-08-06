#pragma once

#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        using namespace core;

        template<typename T>
        class ItemBuffer : public Buffer
        {

        public:
            T Item;

        public:
            ItemBuffer() = default;
            ItemBuffer(u32 slot);

        public:
            void Free();
            void Flush();
            void FlushItem(const T& item);
        };

        template<typename T>
        ItemBuffer<T>::ItemBuffer(u32 slot)
        {
            Type = eBufferType::STRUCTURED;
            Slot = slot;
            StructureSize = sizeof(T);
            NumElements = 1;
            context::CreateBuffer(*this);
            context::WriteBuffer(*this, &Item, StructureSize);
        }

        template<typename T>
        void ItemBuffer<T>::Free()
        {
            context::FreeBuffer(*this);
        }

        template<typename T>
        void ItemBuffer<T>::Flush()
        {
            context::WriteBuffer(*this, &Item, StructureSize);
        }

        template<typename T>
        void ItemBuffer<T>::FlushItem(const T &item)
        {
            Item = item;
            context::WriteBuffer(*this, &Item, StructureSize);
        }

    }

}