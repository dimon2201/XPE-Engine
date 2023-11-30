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
            sItemBuffer(u32 slot, sBuffer::eSubType subtype = sBuffer::eSubType::NONE, sBuffer::eViewType viewtype = sBuffer::eViewType::NONE);
            ~sItemBuffer();

            void Flush();
            void FlushItem(const T& item);
        };

        template<typename T>
        sItemBuffer<T>::sItemBuffer(u32 slot, sBuffer::eSubType subtype, sBuffer::eViewType viewtype)
        {
            Type = sBuffer::eType::ITEM;
            ViewType = viewtype;
            SubType = subtype;
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