#pragma once

namespace xpe
{
    namespace core
    {
        class Application;

        struct ENGINE_API WindowDescriptor
        {
            const char* Title;
            s32 Width;
            s32 Height;
            s32 X;
            s32 Y;
            Boolean Vsync;
        };

        struct ENGINE_API Window final {
            void* Instance = nullptr;
            void* Win32Instance = nullptr;
            WindowDescriptor Descriptor;
        };

        class ENGINE_API WindowManager final {

        public:
            static void Init();
            static void Free();

            static void InitWindow(const WindowDescriptor& windowDescriptor);
            static void FreeWindow();

            static const WindowDescriptor& GetDescriptor();
            static int GetWidth();
            static int GetHeight();
            static int GetPosX();
            static int GetPosY();
            static const char* GetTitle();
            static void* GetInstance();
            static void* GetWin32Instance();

            static void SetPos(int x, int y);
            static void SetSize(int w, int h);

            static bool ShouldClose();
            static void Close();

            static void PollEvents();
            static void Swap();

            static void SetUserPointer(void* userPtr);
            static void* GetUserPointer();

            static void SetVSync(Boolean vsync);

        private:
            static Window s_Window;

        };

    }
}