#include <launcher.h>
#include <game.h>

using namespace xpe::core;

class TemplateApplication : public xpe::core::Application {

    void Init() override {}

    void Update() override {}

    void Free() override {}

    Game* CreateGame() override {
        return new TemplateGame();
    }

};

Application* CreateApplication() {
    return new TemplateApplication();
}