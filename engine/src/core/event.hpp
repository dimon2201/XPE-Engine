#pragma once

#define event_begin(name, ...) \
        typedef void (*f##name)(void* thiz, __VA_ARGS__); \
        template<typename T> \
        static void On##name(void* const thiz, __VA_ARGS__) { \

#define event_end(name, ...) ((T*) thiz)->name(__VA_ARGS__); }

namespace xpe {

    namespace core {

        template<typename EventFunction>
        struct sEvent
        {

            void* This = nullptr;
            EventFunction Function = nullptr;
            int Priority = 0;

            sEvent() = default;

            sEvent(EventFunction function, int priority)
            : Function(function), Priority(priority) {}

            sEvent(void* const _this, EventFunction function, int priority)
            : This(_this), Function(function), Priority(priority) {}

            inline friend bool operator<(const sEvent<EventFunction>& e1, const sEvent<EventFunction>& e2)
            {
                return e1.Priority < e2.Priority;
            }

            inline friend bool operator>(const sEvent<EventFunction>& e1, const sEvent<EventFunction>& e2)
            {
                return e1.Priority > e2.Priority;
            }

            inline friend bool operator==(const sEvent<EventFunction>& e1, const sEvent<EventFunction>& e2)
            {
                return e1.This == e2.This;
            }

            inline friend bool operator!=(const sEvent<EventFunction>& e1, const sEvent<EventFunction>& e2)
            {
                return e1.This != e2.This;
            }

        };

        template<typename EventFunction>
        class cEventBuffer : public cObject
        {

        public:
            ~cEventBuffer();

            template<typename... Args>
            void AddEvent(Args &&... eventArgs);

            void RemoveEvent(void* const _this);

            void Clear();

            void Reserve(const usize count);

            template<typename... Args>
            void NotifyAll(Args &&... args);

            inline const vector<sEvent<EventFunction>>& GetEvents() const
            {
                return m_Events;
            }

        private:
            vector<sEvent<EventFunction>> m_Events;
            std::mutex m_Mutex;
        };

        template<typename EventFunction>
        cEventBuffer<EventFunction>::~cEventBuffer()
        {
            m_Events.clear();
        }

        template<typename EventFunction>
        template<typename... Args>
        void cEventBuffer<EventFunction>::AddEvent(Args &&... eventArgs)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            sEvent<EventFunction> event(std::forward<Args>(eventArgs)...);

            m_Events.emplace_back(event);

            // store event functions by Priority in ascending order
            std::sort(m_Events.begin(), m_Events.end(), [](sEvent<EventFunction>& e1, sEvent<EventFunction>& e2) { return e1 < e2; });
        }

        template<typename EventFunction>
        void cEventBuffer<EventFunction>::RemoveEvent(void* const _this)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            usize size = m_Events.size();
            for (int i = 0 ; i < size ; i++) {
                auto& event = m_Events[i];
                if (_this == event.This) {
                    m_Events.erase(m_Events.begin() + i);
                    break;
                }
            }
        }

        template<typename EventFunction>
        void cEventBuffer<EventFunction>::Clear()
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            m_Events.clear();
        }

        template<typename EventFunction>
        void cEventBuffer<EventFunction>::Reserve(const usize count)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            m_Events.reserve(count);
        }

        template<typename EventFunction>
        template<typename... Args>
        void cEventBuffer<EventFunction>::NotifyAll(Args &&... args)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            for (auto& event : m_Events) {
                event.Function(event.This, args...);
            }
        }

    }

}