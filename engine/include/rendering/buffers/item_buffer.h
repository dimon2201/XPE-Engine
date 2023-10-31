#pragma once

#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        using namespace core;

        template<typename T>
        struct sItemBuffer : public sBuffer
        {
            T Item;

            sItemBuffer() = default;
            sItemBuffer(u32 slot);
            ~sItemBuffer();

            void Flush();
            void FlushItem(const T& item);
        };

        template<typename T>
        sItemBuffer<T>::sItemBuffer(u32 slot)
        {
            Type = eBufferType::STRUCTURED;
            Slot = slot;
            StructureSize = sizeof(T);
            NumElements = 1;
            InitialData = &Item;
            context::CreateBuffer(*this);
            context::CopyBuffer(*this, &Item, StructureSize);
        }

        template<typename T>
        sItemBuffer<T>::~sItemBuffer()
        {
            context::FreeBuffer(*this);
        }

        template<typename T>
        void sItemBuffer<T>::Flush()
        {
            context::CopyBuffer(*this, &Item, StructureSize);
        }

        template<typename T>
        void sItemBuffer<T>::FlushItem(const T &item)
        {
            Item = item;
            context::CopyBuffer(*this, &Item, StructureSize);
        }

    }

}