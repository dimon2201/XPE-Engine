#pragma once

#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        using namespace core;

        class Context;

        template<typename T>
        class StructureBuffer : public Buffer {

        public:
            StructureBuffer() = default;
            StructureBuffer(Context* context, usize count, u32 slot, Boolean duplicate);

        public:
            void Free();

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

            inline usize Size() const {
                return m_List.size();
            }

            inline usize Capacity() const {
                return m_List.capacity();
            }

            inline T* Get(u32 index) {
                if (index >= m_List.size()) {
                    LogError("Index {} is out of bounds of size {}", index, m_List.size());
                    return nullptr;
                }
                return &m_List[index];
            }

            inline T* operator [](u32 index) {
                return Get(index);
            }

        public:
            Context* m_Context = nullptr;
            vector<T> m_List;
        };

        template<typename T>
        StructureBuffer<T>::StructureBuffer(Context* context, usize count, u32 slot, Boolean duplicate)
        {
            m_Context = context;
            Type = eBufferType::STRUCTURED;
            Slot = slot;
            StructureSize = sizeof(T);
            FirstElement = 0;
            NumElements = count;
            ByteSize = StructureSize * count;
            Duplicate = duplicate;
            if (count > 0) {
                m_List.resize(count);
                m_Context->CreateBuffer(*this);
                m_Context->WriteBuffer(*this, m_List.data(), ByteSize);
            }
        }

        template<typename T>
        void StructureBuffer<T>::Free() {
            m_Context->FreeBuffer(*this);
            m_List.clear();
        }

        template<typename T>
        void StructureBuffer<T>::Flush() {
            usize size = m_List.size();
            if (size != NumElements) {
                Recreate(size);
            }
            else {
                m_Context->WriteBuffer(*this, m_List.data(), ByteSize);
            }
        }

        template<typename T>
        void StructureBuffer<T>::FlushItem(u32 index, const T& newItem) {
            T* item = operator[](index);
            if (item == nullptr) {
                return;
            }
            *item = newItem;
            m_Context->WriteBufferOffset(*this, StructureSize * index, item, StructureSize);
        }

        template<typename T>
        void StructureBuffer<T>::FlushItemAt(u32 index) {
            T* item = operator[](index);
            if (item == nullptr) {
                return;
            }
            m_Context->WriteBufferOffset(*this, StructureSize * index, item, StructureSize);
        }

        template<typename T>
        void StructureBuffer<T>::Recreate(const usize count) {
            m_List.resize(count);
            NumElements = count;
            ByteSize = StructureSize * count;
            m_Context->FreeBuffer(*this);
            m_Context->CreateBuffer(*this);
            m_Context->WriteBuffer(*this, m_List.data(), ByteSize);
        }

        template<typename T>
        void StructureBuffer<T>::Resize(const usize count) {
            Recreate(count);
        }

        template<typename T>
        void StructureBuffer<T>::Reserve(const usize count) {
            m_List.reserve(count);
        }

        template<typename T>
        void StructureBuffer<T>::Clear() {
            m_List.clear();
        }

        template<typename T>
        template<typename... Args>
        u32 StructureBuffer<T>::Add(Args &&... args) {
            m_List.emplace_back(args...);
            return m_List.size() - 1;
        }

        template<typename T>
        void StructureBuffer<T>::RemoveAt(u32 index) {
            m_List.erase(m_List.begin() + index);
        }

    }

}