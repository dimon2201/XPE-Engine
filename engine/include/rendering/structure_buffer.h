#pragma once

#include <rendering/context.hpp>

namespace xpe {

    namespace render {

        using namespace core;

        class Context;

        template<typename T>
        class StructureBuffer : public Buffer {

        public:
            StructureBuffer() = default;
            StructureBuffer(Context* context, usize size, u32 slot, Boolean duplicate);

        public:
            void Free();

            void Flush();

            void FlushItem(u32 index, const T& item);

            void Resize(const usize size);

            void Reserve(const usize size);

            void Recreate(const usize size);

            inline T* GetItem(const u32 index) {
                // check if index is in the size bounds
                // if not, then resize to index + 1
                if (index >= Size()) {
                    Resize(index + 1);
                }
                return &m_List[index];
            }

            inline vector<T>& GetList() {
                return m_List;
            }

            inline usize Size() {
                return m_List.size();
            }

        protected:
            Context* m_Context = nullptr;
            vector<T> m_List;
        };

        template<typename T>
        StructureBuffer<T>::StructureBuffer(Context* context, usize size, u32 slot, Boolean duplicate) : m_Context(context)
        {
            m_List.reserve(size * sizeof(T));
            Type = eBufferType::STRUCTURED;
            Slot = slot;
            StructureSize = sizeof(T);
            FirstElement = 0;
            NumElements = m_List.capacity();
            ByteSize = StructureSize * NumElements;
            Duplicate = duplicate;
            m_Context->CreateBuffer(*this);
        }

        template<typename T>
        void StructureBuffer<T>::Free() {
            m_Context->FreeBuffer(*this);
            m_List.clear();
        }

        template<typename T>
        void StructureBuffer<T>::Flush() {
            m_Context->WriteBuffer(*this, m_List.data(), m_List.size() * StructureSize);
        }

        template<typename T>
        void StructureBuffer<T>::FlushItem(u32 index, const T &item) {
            if (index >= m_List.size()) {
                Resize(index);
            }
            m_List[index] = item;
            m_Context->WriteBufferOffset(*this, StructureSize * index, &m_List.back(), StructureSize);
        }

        template<typename T>
        void StructureBuffer<T>::Recreate(const usize size) {
            NumElements = size;
            ByteSize = NumElements * StructureSize;
            m_Context->FreeBuffer(*this);
            m_Context->CreateBuffer(*this);
            Flush();
        }

        template<typename T>
        void StructureBuffer<T>::Resize(const usize size) {
            usize capacity = m_List.capacity();
            m_List.resize(size);
            if (capacity < size * StructureSize) {
                Recreate(size);
            }
        }

        template<typename T>
        void StructureBuffer<T>::Reserve(const usize size) {
            usize capacity = m_List.capacity();
            m_List.reserve(size * StructureSize);
            if (capacity < size * StructureSize) {
                Recreate(size);
            }
        }

    }

}