
#include <array>
#include <iostream>

#include <SDL.h>

#include "lua_manager.h"

bool isQuit;

#define AMIFFY3D_DEBUG

int SDL_main(int argc, char *args[]) {
#ifdef AMIFFY3D_DEBUG
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
#endif

    std::array<std::string, 1> lists = { { "some.lua" } };

    amiffy3d::LuaManager *lm = new amiffy3d::LuaManager();
    lm->Init();
    //lm->Load("some.lua");
    lm->LoadScripts(lists.data(), lists.size());
    lm->ExecuteLuaFunction("dadadsa");

    lm->Dispose();

    return 0;
};
