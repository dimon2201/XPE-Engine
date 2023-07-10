#pragma once

#include <core/keycodes.h>
#include <core/mousecodes.h>

namespace xpe {

    namespace core {

        class ENGINE_API AppEventListener {};

        class ENGINE_API WindowEventListener {

        public:
            virtual void WindowClosed();
            virtual void WindowMoved(int x, int y);
            virtual void WindowResized(int width, int height);
            virtual void WindowFrameResized(int width, int height);
            virtual void WindowFocused();
            virtual void WindowFocusLost();

        };

        class ENGINE_API KeyEventListener {

        public:
            virtual void KeyPressed(const eKey key);
            virtual void KeyReleased(const eKey key);
            virtual void KeyHold(const eKey key);

        };

        class ENGINE_API MouseEventListener {

        public:
            virtual void MousePressed(const eMouse mouse);
            virtual void MouseReleased(const eMouse mouse);
            virtual void MouseHold(const eMouse mouse);

        };

        class ENGINE_API CursorEventListener {

        public:
            virtual void CursorMoved(const double x, const double y);
            virtual void CursorEntered();
            virtual void CursorLeft();

        };

        class ENGINE_API ScrollEventListener {

        public:
            virtual void ScrollChanged(const double x, const double y);

        };

        class ENGINE_API CharEventListener {

        public:
            virtual void CharTyped(const u32 charUnicode);
            virtual void CharModsTyped(const u32 charUnicode, const int mods);

        };

        template<typename EventListener>
        struct Event final {

            EventListener* Listener = nullptr;
            int Priority = 0;

            Event() = default;
            Event(EventListener* listener, int priority)
            : Listener(listener), Priority(priority) {}

            inline friend bool operator<(const Event<EventListener>& e1, const Event<EventListener>& e2) {
                return e1.Priority < e2.Priority;
            }

            inline friend bool operator>(const Event<EventListener>& e1, const Event<EventListener>& e2) {
                return e1.Priority > e2.Priority;
            }

            inline friend bool operator==(const Event<EventListener>& e1, const Event<EventListener>& e2) {
                return e1.Listener == e2.Listener;
            }

            inline friend bool operator!=(const Event<EventListener>& e1, const Event<EventListener>& e2) {
                return e1.Listener != e2.Listener;
            }

        };

        template<typename EventListener>
        class EventBuffer final {

        public:
            template<typename... Args>
            void AddEvent(Args &&... eventArgs);

            void RemoveEvent(EventListener* listener);

            void Clear();

            void Reserve(const usize count);

            inline const vector<Event<EventListener>>& GetEvents() const {
                return m_Events;
            }

        private:
            vector<Event<EventListener>> m_Events;
            std::mutex m_Mutex;
        };

        template<typename EventListener>
        template<typename... Args>
        void EventBuffer<EventListener>::AddEvent(Args &&... eventArgs) {
            std::lock_guard<std::mutex> lock(m_Mutex);

            Event<EventListener> event(std::forward<Args>(eventArgs)...);

            // check if event listener already added
            for (const auto& currentEvent : m_Events) {
                if (event.Listener == currentEvent.Listener) {
                    LogWarning("EventBuffer::AddEvent(): EventListener is already added!");
                    return;
                }
            }

            m_Events.emplace_back(event);

            // store event listeners by Priority number
            std::sort(m_Events.begin(), m_Events.end(), [](Event<EventListener>& e1, Event<EventListener>& e2) { return e1.Priority > e2.Priority; });
        }

        template<typename EventListener>
        void EventBuffer<EventListener>::RemoveEvent(EventListener* listener) {
            std::lock_guard<std::mutex> lock(m_Mutex);

            for (vector<Event<EventListener>>::iterator it = m_Events.begin(); it != m_Events.end(); it++) {
                if (listener == it->Listener) {
                    m_Events.erase(it);
                    break;
                }
            }
        }

        template<typename EventListener>
        void EventBuffer<EventListener>::Clear() {
            std::lock_guard<std::mutex> lock(m_Mutex);

            m_Events.clear();
        }

        template<typename EventListener>
        void EventBuffer<EventListener>::Reserve(const usize count) {
            std::lock_guard<std::mutex> lock(m_Mutex);

            m_Events.reserve(sizeof(Event<EventListener>) * count);
        }

    }

}