#pragma once

namespace xpe {

    namespace core {

        template<typename EventFunction>
        struct Event {

            void* Thiz = nullptr;
            EventFunction Function = nullptr;
            int Priority = 0;

            Event() = default;

            Event(EventFunction function, int priority)
            : Function(function), Priority(priority) {}

            Event(void* const thiz, EventFunction function, int priority)
            : Thiz(thiz), Function(function), Priority(priority) {}

            inline friend bool operator<(const Event<EventFunction>& e1, const Event<EventFunction>& e2) {
                return e1.Priority < e2.Priority;
            }

            inline friend bool operator>(const Event<EventFunction>& e1, const Event<EventFunction>& e2) {
                return e1.Priority > e2.Priority;
            }

            inline friend bool operator==(const Event<EventFunction>& e1, const Event<EventFunction>& e2) {
                return e1.Thiz == e2.Thiz;
            }

            inline friend bool operator!=(const Event<EventFunction>& e1, const Event<EventFunction>& e2) {
                return e1.Thiz != e2.Thiz;
            }

        };

        template<typename EventFunction>
        class EventBuffer final {

        public:
            ~EventBuffer();

            template<typename... Args>
            void AddEvent(Args &&... eventArgs);

            void RemoveEvent(void* const thiz);

            void Clear();

            void Reserve(const usize count);

            inline const vector<Event<EventFunction>>& GetEvents() const {
                return m_Events;
            }

        private:
            vector<Event<EventFunction>> m_Events;
            std::mutex m_Mutex;
        };

        template<typename EventFunction>
        EventBuffer<EventFunction>::~EventBuffer() {
            m_Events.clear();
        }

        template<typename EventFunction>
        template<typename... Args>
        void EventBuffer<EventFunction>::AddEvent(Args &&... eventArgs) {
            std::lock_guard<std::mutex> lock(m_Mutex);

            Event<EventFunction> event(std::forward<Args>(eventArgs)...);

            // check if event function already added
            for (const auto& currentEvent : m_Events) {
                if (event.Function == currentEvent.Function) {
                    LogWarning("EventBuffer::AddEvent(): EventFunction is already added!");
                    return;
                }
            }

            m_Events.emplace_back(event);

            // store event functions by Priority number
            std::sort(m_Events.begin(), m_Events.end(), [](Event<EventFunction>& e1, Event<EventFunction>& e2) { return e1 > e2; });
        }

        template<typename EventFunction>
        void EventBuffer<EventFunction>::RemoveEvent(void* const thiz) {
            std::lock_guard<std::mutex> lock(m_Mutex);

            for (vector<Event<EventFunction>>::iterator it = m_Events.begin(); it != m_Events.end(); it++) {
                if (thiz == it->Thiz) {
                    m_Events.erase(it);
                    break;
                }
            }
        }

        template<typename EventFunction>
        void EventBuffer<EventFunction>::Clear() {
            std::lock_guard<std::mutex> lock(m_Mutex);

            m_Events.clear();
        }

        template<typename EventFunction>
        void EventBuffer<EventFunction>::Reserve(const usize count) {
            std::lock_guard<std::mutex> lock(m_Mutex);

            m_Events.reserve(count);
        }

    }

}