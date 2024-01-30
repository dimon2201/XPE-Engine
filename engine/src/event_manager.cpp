#include <event_manager.hpp>

namespace xpe
{
    namespace core
    {
        cEventBuffer<void> MEvent::WindowClosed;
        cEventBuffer<int, int> MEvent::WindowResized;
        cEventBuffer<int, int> MEvent::WindowFrameResized;
        cEventBuffer<int, int> MEvent::WindowMoved;
        cEventBuffer<void> MEvent::WindowFocused;
        cEventBuffer<void> MEvent::WindowFocusLost;
        cEventBuffer<float> MEvent::WindowRatioChanged;

        cEventBuffer<const eKey> MEvent::KeyPressed;
        cEventBuffer<const eKey> MEvent::KeyReleased;
        cEventBuffer<const eKey> MEvent::KeyHold;

        cEventBuffer<const eMouse> MEvent::MousePressed;
        cEventBuffer<const eMouse> MEvent::MouseReleased;
        cEventBuffer<const eMouse> MEvent::MouseHold;

        cEventBuffer<const double, const double> MEvent::CursorMoved;
        cEventBuffer<void> MEvent::CursorEntered;
        cEventBuffer<void> MEvent::CursorLeft;

        cEventBuffer<const double, const double> MEvent::ScrollChanged;

        cEventBuffer<const u32> MEvent::CharTyped;
        cEventBuffer<const u32, const int> MEvent::CharModsTyped;

        cEventBuffer<u32> MEvent::SampleCountChanged;
    }
}