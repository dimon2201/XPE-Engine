#pragma once

#include <core/event.h>

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

    }

}