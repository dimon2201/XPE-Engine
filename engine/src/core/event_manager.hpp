#pragma once

#include <core/events.hpp>

namespace xpe {

    namespace core {

        class ENGINE_API cEventManager final {

        public:
            static void Init();
            static void Free();
            static cEventManager& Get();

            static cEventManager* s_Instance;

            cEventBuffer<fWindowClosed> WindowClosedEvents;
            cEventBuffer<fWindowResized> WindowResizedEvents;
            cEventBuffer<fWindowFrameResized> WindowFrameResizedEvents;
            cEventBuffer<fWindowMoved> WindowMovedEvents;
            cEventBuffer<fWindowFocused> WindowFocusedEvents;
            cEventBuffer<fWindowFocusLost> WindowFocusLostEvents;

            cEventBuffer<fKeyPressed> KeyPressedEvents;
            cEventBuffer<fKeyReleased> KeyReleasedEvents;
            cEventBuffer<fKeyHold> KeyHoldEvents;

            cEventBuffer<fMousePressed> MousePressedEvents;
            cEventBuffer<fMouseReleased> MouseReleasedEvents;
            cEventBuffer<fMouseHold> MouseHoldEvents;

            cEventBuffer<fCursorMoved> CursorMovedEvents;
            cEventBuffer<fCursorEntered> CursorEnteredEvents;
            cEventBuffer<fCursorLeft> CursorLeftEvents;

            cEventBuffer<fScrollChanged> ScrollChangedEvents;

            cEventBuffer<fCharTyped> CharTypedEvents;
            cEventBuffer<fCharModsTyped> CharModsTypedEvents;

            cEventBuffer<fSampleCountChanged> SampleCountChangedEvents;
            cEventBuffer<fWindowRatioChanged> WindowRatioChangedEvents;

        };

    }

}