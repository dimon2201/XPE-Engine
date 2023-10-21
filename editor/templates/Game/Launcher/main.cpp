#include <launcher.h>
#include <game.h>

using namespace xpe::core;

Application* CreateApplication() {
    return new TemplateGame();
}