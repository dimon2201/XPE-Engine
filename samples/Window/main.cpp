#include <iostream>
#include "../../engine/src/core/core.hpp"

using namespace xpe::core;

class GameApp : public App_Interface
{
    public:
        GameApp() {}
        ~GameApp() {}

        void Init(Window* window, RenderingContext_Interface* context) override final
        {
        }

        void Update(Window* window, RenderingContext_Interface* context) override final
        {
        }
};

int main()
{
    GameApp app;
    WindowDescriptor desc;
    desc.Width = 800;
    desc.Height = 600;
    desc.Title = "Game App Test";

    RunApp(&app, desc);

    return 0;
}