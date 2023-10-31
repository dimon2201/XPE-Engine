#pragma once

namespace xpe {

    namespace core {

        template<typename T>
        class cRingBuffer : public cObject
        {

        public:
            void Reserve(usize capacity);
            void Resize(usize size);
            bool Push(const T& item);
            bool Pop(T& item);

        private:
            vector<T> m_List;
            usize m_Tail = 0;
            usize m_Head = 0;
            std::mutex m_Lock;
        };

        template<typename T>
        void cRingBuffer<T>::Reserve(usize capacity)
        {
            m_List.reserve(capacity);
        }

        template<typename T>
        void cRingBuffer<T>::Resize(usize size)
        {
            m_List.resize(size);
        }

        template<typename T>
        bool cRingBuffer<T>::Push(const T &item)
        {
            bool pushed = false;
            std::lock_guard<std::mutex> lock(m_Lock);
            usize next = (m_Head + 1) % m_List.size();
            if (next != m_Tail) {
                m_List[m_Head] = item;
                m_Head = next;
                pushed = true;
            }
            return pushed;
        }

        template<typename T>
        bool cRingBuffer<T>::Pop(T &item)
        {
            bool popped = false;
            std::lock_guard<std::mutex> lock(m_Lock);
            if (m_Tail != m_Head) {
                item = m_List[m_Tail];
                m_Tail = (m_Tail + 1) % m_List.size();
                popped = true;
            }
            return popped;
        }

    }

}