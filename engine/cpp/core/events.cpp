#include <core/events.h>

namespace xpe {

    namespace core {

        void WindowEventListener::WindowClosed() {

        }

        void WindowEventListener::WindowMoved(int x, int y) {

        }

        void WindowEventListener::WindowResized(int width, int height) {

        }

        void WindowEventListener::WindowFrameResized(int width, int height) {

        }

        void WindowEventListener::WindowFocused() {

        }

        void WindowEventListener::WindowFocusLost() {

        }

        void MouseEventListener::MousePressed(const eMouse mouse) {

        }

        void MouseEventListener::MouseReleased(const eMouse mouse) {

        }

        void MouseEventListener::MouseHold(const eMouse mouse) {

        }

        void KeyEventListener::KeyPressed(const eKey key) {

        }

        void KeyEventListener::KeyReleased(const eKey key) {

        }

        void KeyEventListener::KeyHold(const eKey key) {

        }

        void CursorEventListener::CursorMoved(const double x, const double y) {

        }

        void CursorEventListener::CursorEntered() {

        }

        void CursorEventListener::CursorLeft() {

        }

        void ScrollEventListener::ScrollChanged(const double x, const double y) {

        }

        void CharEventListener::CharTyped(const u32 charUnicode) {

        }

        void CharEventListener::CharModsTyped(const u32 charUnicode, const int mods) {

        }

    }

}