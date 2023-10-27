#pragma once

#include <core/events.h>

namespace xpe {

    namespace core {

        class ENGINE_API EventManager final {

        public:
            static void Init();
            static void Free();
            static EventManager& Get();

            static EventManager* s_Instance;

            EventBuffer<EventWindowClosed> WindowClosedEvents;
            EventBuffer<EventWindowResized> WindowResizedEvents;
            EventBuffer<EventWindowFrameResized> WindowFrameResizedEvents;
            EventBuffer<EventWindowMoved> WindowMovedEvents;
            EventBuffer<EventWindowFocused> WindowFocusedEvents;
            EventBuffer<EventWindowFocusLost> WindowFocusLostEvents;

            EventBuffer<EventKeyPressed> KeyPressedEvents;
            EventBuffer<EventKeyReleased> KeyReleasedEvents;
            EventBuffer<EventKeyHold> KeyHoldEvents;

            EventBuffer<EventMousePressed> MousePressedEvents;
            EventBuffer<EventMouseReleased> MouseReleasedEvents;
            EventBuffer<EventMouseHold> MouseHoldEvents;

            EventBuffer<EventCursorMoved> CursorMovedEvents;
            EventBuffer<EventCursorEntered> CursorEnteredEvents;
            EventBuffer<EventCursorLeft> CursorLeftEvents;

            EventBuffer<EventScrollChanged> ScrollChangedEvents;

            EventBuffer<EventCharTyped> CharTypedEvents;
            EventBuffer<EventCharModsTyped> CharModsTypedEvents;

            EventBuffer<EventSampleCountChanged> SampleCountChangedEvents;

        };

    }

}