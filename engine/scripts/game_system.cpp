#include <game_system.hpp>

extern "C" GAMELIB_API xpe::ecs::cSystem* _cdecl CreateGameSystem()
{
    return new cGameSystem();
}