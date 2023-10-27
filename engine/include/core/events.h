#pragma once

#include <core/event.h>

namespace xpe {

    namespace core {

        event_begin(WindowClosed)
        event_end(WindowClosed)

        event_begin(WindowMoved, int x, int y)
        event_end(WindowMoved, x, y)

        event_begin(WindowResized, int w, int h)
        event_end(WindowResized, w, h)

        event_begin(WindowFrameResized, int w, int h)
        event_end(WindowFrameResized, w, h)

        event_begin(WindowFocused)
        event_end(WindowFocused)

        event_begin(WindowFocusLost)
        event_end(WindowFocusLost)

        event_begin(KeyPressed, const eKey key)
        event_end(KeyPressed, key)

        event_begin(KeyReleased, const eKey key)
        event_end(KeyReleased, key)

        event_begin(KeyHold, const eKey key)
        event_end(KeyHold, key)

        event_begin(MousePressed, const eMouse mouse)
        event_end(MousePressed, mouse)

        event_begin(MouseReleased, const eMouse mouse)
        event_end(MouseReleased, mouse)

        event_begin(MouseHold, const eMouse mouse)
        event_end(MouseHold, mouse)

        event_begin(CursorMoved, const double x, const double y)
        event_end(CursorMoved, x, y)

        event_begin(CursorEntered)
        event_end(CursorEntered)

        event_begin(CursorLeft)
        event_end(CursorLeft)

        event_begin(ScrollChanged, const double x, const double y)
        event_end(ScrollChanged, x, y)

        event_begin(CharTyped, const u32 charUnicode)
        event_end(CharTyped, charUnicode)

        event_begin(CharModsTyped, const u32 charUnicode, const int mods)
        event_end(CharModsTyped, charUnicode, mods)

        event_begin(FileAdded, const string& watchpath, const string& filepath)
        event_end(FileAdded, watchpath, filepath)

        event_begin(FileDeleted, const string& watchpath, const string& filepath)
        event_end(FileDeleted, watchpath, filepath)

        event_begin(FileModified, const string& watchpath, const string& filepath)
        event_end(FileModified, watchpath, filepath)

        event_begin(FileNewName, const string& watchpath, const string& filepath)
        event_end(FileNewName, watchpath, filepath)

        event_begin(FileOldName, const string& watchpath, const string& filepath)
        event_end(FileOldName, watchpath, filepath)

        event_begin(SampleCountChanged, u32 sampleCount)
        event_end(SampleCountChanged, sampleCount)

    }

}