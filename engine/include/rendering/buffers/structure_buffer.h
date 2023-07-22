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

            void Resize(const usize count);

            void Reserve(const usize count);

            void Recreate(const usize count);

            inline T* GetItem(const u32 index) {
                // check if index is in the size bounds
                // if not, then resize to index + 1
                if (index >= Count()) {
                    Resize(index + 1);
                }
                return &m_List[index];
            }

            inline vector<T>& GetList() {
                return m_List;
            }

            inline usize Count() {
                return m_List.size();
            }

            inline usize Size() {
                return m_List.size() * StructureSize;
            }

        protected:
            Context* m_Context = nullptr;
            vector<T> m_List;
        };

        template<typename T>
        StructureBuffer<T>::StructureBuffer(Context* context, usize count, u32 slot, Boolean duplicate) : m_Context(context)
        {
            m_List.reserve(count);
            Type = eBufferType::STRUCTURED;
            Slot = slot;
            StructureSize = sizeof(T);
            FirstElement = 0;
            NumElements = count;
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
            if (index >= Count()) {
                Resize(index + 1);
            }
            m_List[index] = item;
            m_Context->WriteBufferOffset(*this, StructureSize * index, &m_List.back(), StructureSize);
        }

        template<typename T>
        void StructureBuffer<T>::Recreate(const usize count) {
            NumElements = count;
            ByteSize = NumElements * StructureSize;
            m_Context->FreeBuffer(*this);
            m_Context->CreateBuffer(*this);
            Flush();
        }

        template<typename T>
        void StructureBuffer<T>::Resize(const usize count) {
            usize capacity = m_List.capacity();
            m_List.resize(count);
            if (capacity < count) {
                Recreate(count);
            }
        }

        template<typename T>
        void StructureBuffer<T>::Reserve(const usize count) {
            usize capacity = m_List.capacity();
            m_List.reserve(count);
            if (capacity < count) {
                Recreate(count);
            }
        }

    }

}