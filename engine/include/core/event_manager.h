#pragma once

#include <core/events.h>

namespace xpe {

    namespace core {

        class ENGINE_API cEventManager final {

        public:
            static void Init();
            static void Free();
            static cEventManager& Get();

            static cEventManager* s_Instance;

            cEventBuffer<eWindowClosed> WindowClosedEvents;
            cEventBuffer<eWindowResized> WindowResizedEvents;
            cEventBuffer<eWindowFrameResized> WindowFrameResizedEvents;
            cEventBuffer<eWindowMoved> WindowMovedEvents;
            cEventBuffer<eWindowFocused> WindowFocusedEvents;
            cEventBuffer<eWindowFocusLost> WindowFocusLostEvents;

            cEventBuffer<eKeyPressed> KeyPressedEvents;
            cEventBuffer<eKeyReleased> KeyReleasedEvents;
            cEventBuffer<eKeyHold> KeyHoldEvents;

            cEventBuffer<eMousePressed> MousePressedEvents;
            cEventBuffer<eMouseReleased> MouseReleasedEvents;
            cEventBuffer<eMouseHold> MouseHoldEvents;

            cEventBuffer<eCursorMoved> CursorMovedEvents;
            cEventBuffer<eCursorEntered> CursorEnteredEvents;
            cEventBuffer<eCursorLeft> CursorLeftEvents;

            cEventBuffer<eScrollChanged> ScrollChangedEvents;

            cEventBuffer<eCharTyped> CharTypedEvents;
            cEventBuffer<eCharModsTyped> CharModsTypedEvents;

            cEventBuffer<eSampleCountChanged> SampleCountChangedEvents;
            cEventBuffer<eWindowRatioChanged> WindowRatioChangedEvents;

        };

    }

}