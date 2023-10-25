#pragma once

namespace xpe {

    namespace core {

        template<typename EventFunction>
        struct Event
        {

            void* This = nullptr;
            EventFunction Function = nullptr;
            int Priority = 0;

            Event() = default;

            Event(EventFunction function, int priority)
            : Function(function), Priority(priority) {}

            Event(void* const _this, EventFunction function, int priority)
            : This(_this), Function(function), Priority(priority) {}

            inline friend bool operator<(const Event<EventFunction>& e1, const Event<EventFunction>& e2)
            {
                return e1.Priority < e2.Priority;
            }

            inline friend bool operator>(const Event<EventFunction>& e1, const Event<EventFunction>& e2)
            {
                return e1.Priority > e2.Priority;
            }

            inline friend bool operator==(const Event<EventFunction>& e1, const Event<EventFunction>& e2)
            {
                return e1.This == e2.This;
            }

            inline friend bool operator!=(const Event<EventFunction>& e1, const Event<EventFunction>& e2)
            {
                return e1.This != e2.This;
            }

        };

        template<typename EventFunction>
        class EventBuffer : public Object
        {

        public:
            ~EventBuffer();

            template<typename... Args>
            void AddEvent(Args &&... eventArgs);

            void RemoveEvent(void* const _this);

            void Clear();

            void Reserve(const usize count);

            template<typename... Args>
            void NotifyAll(Args &&... args);

            inline const vector<Event<EventFunction>>& GetEvents() const
            {
                return m_Events;
            }

        private:
            vector<Event<EventFunction>> m_Events;
            std::mutex m_Mutex;
        };

        template<typename EventFunction>
        EventBuffer<EventFunction>::~EventBuffer()
        {
            m_Events.clear();
        }

        template<typename EventFunction>
        template<typename... Args>
        void EventBuffer<EventFunction>::AddEvent(Args &&... eventArgs)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            Event<EventFunction> event(std::forward<Args>(eventArgs)...);

            // check if event function already added
            for (const auto& currentEvent : m_Events)
            {
                if (event.Function == currentEvent.Function)
                {
                    LogWarning("EventBuffer::AddEvent(): EventFunction is already added!");
                    return;
                }
            }

            m_Events.emplace_back(event);

            // store event functions by Priority number
            std::sort(m_Events.begin(), m_Events.end(), [](Event<EventFunction>& e1, Event<EventFunction>& e2) { return e1 > e2; });
        }

        template<typename EventFunction>
        void EventBuffer<EventFunction>::RemoveEvent(void* const _this)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            usize size = m_Events.size();
            for (int i = 0 ; i < size ; i++) {
                auto& event = m_Events[i];
                if (_this == event->This) {
                    m_Events.erase(event);
                    break;
                }
            }
        }

        template<typename EventFunction>
        void EventBuffer<EventFunction>::Clear()
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            m_Events.clear();
        }

        template<typename EventFunction>
        void EventBuffer<EventFunction>::Reserve(const usize count)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            m_Events.reserve(count);
        }

        template<typename EventFunction>
        template<typename... Args>
        void EventBuffer<EventFunction>::NotifyAll(Args &&... args)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            for (auto& event : m_Events) {
                event.Function(event.This, args...);
            }
        }

    }

}