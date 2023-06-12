#include <iostream>
#include "../../engine/src/core/app.hpp"

using namespace xpe::core;

class GameApp : public App_Interface
{
    public:
        GameApp() {}
        ~GameApp() {}

        void Init() override final
        {
        }

        void Update() override final
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