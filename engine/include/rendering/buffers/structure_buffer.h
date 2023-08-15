#pragma once

#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        using namespace core;

        template<typename T>
        class StructureBuffer : public Buffer
        {

        public:
            StructureBuffer() = default;
            StructureBuffer(usize count, u32 slot);
            ~StructureBuffer();

        public:
            void Flush();

            void FlushItem(u32 index, const T& item);

            void FlushItemAt(u32 index);

            template<typename ... Args>
            u32 Add(Args&&... args);

            void RemoveAt(u32 index);

            void Resize(const usize count);

            void Reserve(const usize count);

            void Recreate(const usize count);

            void Clear();

            inline usize Size() const
            {
                return m_List.size();
            }

            inline usize Capacity() const
            {
                return m_List.capacity();
            }

            inline T* Get(u32 index)
            {
                if (index >= m_List.size())
                {
                    LogError("Index {} is out of bounds of size {}", index, m_List.size());
                    return nullptr;
                }
                return &m_List[index];
            }

            inline T* operator [](u32 index)
            {
                return Get(index);
            }

        protected:
            vector<T> m_List;
        };

        template<typename T>
        StructureBuffer<T>::StructureBuffer(usize count, u32 slot)
        {
            Type = eBufferType::STRUCTURED;
            Slot = slot;
            StructureSize = sizeof(T);
            NumElements = count;
            if (count > 0)
            {
                m_List.resize(count);
                context::CreateBuffer(*this);
                context::CopyBuffer(*this, m_List.data(), ByteSize());
            }
        }

        template<typename T>
        StructureBuffer<T>::~StructureBuffer()
        {
            context::FreeBuffer(*this);
            m_List.clear();
        }

        template<typename T>
        void StructureBuffer<T>::Flush()
        {
            usize size = m_List.size();
            if (size != NumElements) {
                Recreate(size);
            }
            else {
                context::CopyBuffer(*this, m_List.data(), ByteSize());
            }
        }

        template<typename T>
        void StructureBuffer<T>::FlushItem(u32 index, const T& newItem)
        {
            T* item = operator[](index);
            if (item == nullptr) {
                return;
            }
            *item = newItem;
            context::CopyBufferOffset(*this, StructureSize * index, item, StructureSize);
        }

        template<typename T>
        void StructureBuffer<T>::FlushItemAt(u32 index)
        {
            T* item = operator[](index);
            if (item == nullptr) {
                return;
            }
            context::CopyBufferOffset(*this, StructureSize * index, item, StructureSize);
        }

        template<typename T>
        void StructureBuffer<T>::Recreate(const usize count)
        {
            m_List.resize(count);
            NumElements = count;
            context::FreeBuffer(*this);
            context::CreateBuffer(*this);
            context::CopyBuffer(*this, m_List.data(), ByteSize());
        }

        template<typename T>
        void StructureBuffer<T>::Resize(const usize count)
        {
            Recreate(count);
        }

        template<typename T>
        void StructureBuffer<T>::Reserve(const usize count)
        {
            m_List.reserve(count);
        }

        template<typename T>
        void StructureBuffer<T>::Clear()
        {
            m_List.clear();
        }

        template<typename T>
        template<typename... Args>
        u32 StructureBuffer<T>::Add(Args &&... args)
        {
            m_List.emplace_back(args...);
            return m_List.size() - 1;
        }

        template<typename T>
        void StructureBuffer<T>::RemoveAt(u32 index)
        {
            m_List.erase(m_List.begin() + index);
        }

    }

}