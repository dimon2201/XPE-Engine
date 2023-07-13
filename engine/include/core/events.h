#pragma once

#include <core/keycodes.h>
#include <core/mousecodes.h>

namespace xpe {

    namespace core {

        typedef void (*WindowClosedFn)(void* thiz);
        template<typename T>
        static void OnWindowClosed(void* const thiz) {
            ((T*) thiz)->WindowClosed();
        }

        typedef void (*WindowMovedFn)(void* thiz, int x, int y);
        template<typename T>
        static void OnWindowMoved(void* const thiz, int x, int y) {
            ((T*) thiz)->WindowMoved(x, y);
        }

        typedef void (*WindowResizedFn)(void* thiz, int width, int height);
        template<typename T>
        static void OnWindowResized(void* const thiz, int w, int h) {
            ((T*) thiz)->WindowResized(w, h);
        }

        typedef void (*WindowFrameResizedFn)(void* thiz, int width, int height);
        template<typename T>
        static void OnWindowFrameResized(void* const thiz, int w, int h) {
            ((T*) thiz)->WindowFrameResized(w, h);
        }

        typedef void (*WindowFocusedFn)(void* thiz);
        template<typename T>
        static void OnWindowFocused(void* const thiz) {
            ((T*) thiz)->WindowFocused();
        }

        typedef void (*WindowFocusLostFn)(void* thiz);
        template<typename T>
        static void OnWindowFocusLost(void* const thiz) {
            ((T*) thiz)->WindowFocusLost();
        }

        typedef void (*KeyPressedFn)(void* thiz, const eKey key);
        template<typename T>
        static void OnKeyPressed(void* const thiz, const eKey key) {
            ((T*) thiz)->KeyPressed(key);
        }

        typedef void (*KeyReleasedFn)(void* thiz, const eKey key);
        template<typename T>
        static void OnKeyReleased(void* const thiz, const eKey key) {
            ((T*) thiz)->KeyReleased(key);
        }

        typedef void (*KeyHoldFn)(void* thiz, const eKey key);
        template<typename T>
        static void OnKeyHold(void* const thiz, const eKey key) {
            ((T*) thiz)->KeyHold(key);
        }

        typedef void (*MousePressedFn)(void* thiz, const eMouse mouse);
        template<typename T>
        static void OnMousePressed(void* const thiz, const eMouse mouse) {
            ((T*) thiz)->MousePressed(mouse);
        }

        typedef void (*MouseReleasedFn)(void* thiz, const eMouse mouse);
        template<typename T>
        static void OnMouseReleased(void* const thiz, const eMouse mouse) {
            ((T*) thiz)->MouseReleased(mouse);
        }

        typedef void (*MouseHoldFn)(void* thiz, const eMouse mouse);
        template<typename T>
        static void OnMouseHold(void* const thiz, const eMouse mouse) {
            ((T*) thiz)->MouseHold(mouse);
        }

        typedef void (*CursorMovedFn)(void* thiz, const double x, const double y);
        template<typename T>
        static void OnCursorMoved(void* const thiz, const double x, const double y) {
            ((T*) thiz)->CursorMoved(x, y);
        }

        typedef void (*CursorEnteredFn)(void* thiz);
        template<typename T>
        static void OnCursorEntered(void* const thiz) {
            ((T*) thiz)->CursorEntered();
        }

        typedef void (*CursorLeftFn)(void* thiz);
        template<typename T>
        static void OnCursorLeft(void* const thiz) {
            ((T*) thiz)->CursorLeft();
        }

        typedef void (*ScrollChangedFn)(void* thiz, const double x, const double y);
        template<typename T>
        static void OnScrollChanged(void* const thiz, const double x, const double y) {
            ((T*) thiz)->ScrollChanged(x, y);
        }

        typedef void (*CharTypedFn)(void* thiz, const u32 charUnicode);
        template<typename T>
        static void OnCharTyped(void* const thiz, const u32 charUnicode) {
            ((T*) thiz)->CharTyped(charUnicode);
        }

        typedef void (*CharModsTypedFn)(void* thiz, const u32 charUnicode, const int mods);
        template<typename T>
        static void OnCharModsTyped(void* const thiz, const u32 charUnicode, const int mods) {
            ((T*) thiz)->CharModsTyped(charUnicode, mods);
        }

        template<typename EventFunction>
        struct Event {

            void* Thiz = nullptr;
            EventFunction Function = nullptr;
            int Priority = 0;

            Event() = default;
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

        #define DEFINE_EVENT(e) \
        struct ENGINE_API e : Event<e##Fn> {  \
            e(void* const thiz, e##Fn function, int priority) : Event<e##Fn>(thiz, function, priority) {} \
        }; \

        DEFINE_EVENT(WindowClosed)
        DEFINE_EVENT(WindowResized)
        DEFINE_EVENT(WindowMoved)
        DEFINE_EVENT(WindowFrameResized)
        DEFINE_EVENT(WindowFocused)
        DEFINE_EVENT(WindowFocusLost)

        DEFINE_EVENT(KeyPressed)
        DEFINE_EVENT(KeyReleased)
        DEFINE_EVENT(KeyHold)

        DEFINE_EVENT(MousePressed)
        DEFINE_EVENT(MouseReleased)
        DEFINE_EVENT(MouseHold)

        DEFINE_EVENT(CursorMoved)
        DEFINE_EVENT(CursorEntered)
        DEFINE_EVENT(CursorLeft)

        DEFINE_EVENT(ScrollChanged)

        DEFINE_EVENT(CharTyped)
        DEFINE_EVENT(CharModsTyped)

        template<typename EventFunction>
        class EventBuffer final {

        public:
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

            m_Events.reserve(sizeof(Event<EventFunction>) * count);
        }

    }

}