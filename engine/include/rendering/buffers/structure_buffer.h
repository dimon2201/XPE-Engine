#pragma once

#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        using namespace core;

        template<typename T>
        class sListBuffer : public sBuffer
        {

        public:
            sListBuffer() = default;
            sListBuffer(usize count, u32 slot);
            ~sListBuffer();

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

            inline vector<T>& GetList()
            {
                return m_List;
            }

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
        sListBuffer<T>::sListBuffer(usize count, u32 slot)
        {
            Type = eBufferType::STRUCTURED;
            Slot = slot;
            StructureSize = sizeof(T);
            NumElements = count;
            InitialData = m_List.data();
            if (count > 0)
            {
                m_List.resize(count);
                context::CreateBuffer(*this);
                context::CopyBuffer(*this, m_List.data(), GetByteSize());
            }
        }

        template<typename T>
        sListBuffer<T>::~sListBuffer()
        {
            context::FreeBuffer(*this);
            m_List.clear();
        }

        template<typename T>
        void sListBuffer<T>::Flush()
        {
            usize size = m_List.size();
            if (size != NumElements) {
                Recreate(size);
            }
            else {
                context::CopyBuffer(*this, m_List.data(), GetByteSize());
            }
        }

        template<typename T>
        void sListBuffer<T>::FlushItem(u32 index, const T& newItem)
        {
            T* item = operator[](index);
            if (item == nullptr) {
                return;
            }
            *item = newItem;
            context::CopyBufferOffset(*this, StructureSize * index, item, StructureSize);
        }

        template<typename T>
        void sListBuffer<T>::FlushItemAt(u32 index)
        {
            T* item = operator[](index);
            if (item == nullptr) {
                return;
            }
            context::CopyBufferOffset(*this, StructureSize * index, item, StructureSize);
        }

        template<typename T>
        void sListBuffer<T>::Recreate(const usize count)
        {
            m_List.resize(count);
            NumElements = count;
            InitialData = m_List.data();
            context::FreeBuffer(*this);
            context::CreateBuffer(*this);
            context::CopyBuffer(*this, m_List.data(), GetByteSize());
        }

        template<typename T>
        void sListBuffer<T>::Resize(const usize count)
        {
            m_List.resize(count);
        }

        template<typename T>
        void sListBuffer<T>::Reserve(const usize count)
        {
            m_List.reserve(count);
        }

        template<typename T>
        void sListBuffer<T>::Clear()
        {
            m_List.clear();
        }

        template<typename T>
        template<typename... Args>
        u32 sListBuffer<T>::Add(Args &&... args)
        {
            m_List.emplace_back(args...);
            return m_List.size() - 1;
        }

        template<typename T>
        void sListBuffer<T>::RemoveAt(u32 index)
        {
            m_List.erase(m_List.begin() + index);
        }

    }

}