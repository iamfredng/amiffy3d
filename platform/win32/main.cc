

#include <array>
#include <iostream>

#include <Windows.h>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <SDL.h>

#include "lua_manager.h"

bool isQuit;

// 日志输出选项
#define AMIFFY3D_DEBUG

int main(int argc, char *args[]) {

#ifdef AMIFFY3D_DEBUG
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
#endif

    std::array<std::string, 1> lists = { { "scripts/some.lua" } };

    amiffy3d::LuaManager *lm = new amiffy3d::LuaManager();
    lm->Init();
    //lm->Load("some.lua");
    lm->LoadScripts(lists.data(), lists.size());
    lm->ExecuteLuaFunction("dadadsa");

    // 初始化 SDL
    if (SDL_Init(SDL_INIT_VIDEO) == -1){
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }

    // 获取屏幕分辨率
    SDL_DisplayMode vMode;
    for (int i = 0; i < SDL_GetNumVideoDisplays(); i++) {
        int rvl = SDL_GetCurrentDisplayMode(i, &vMode);
        if (rvl == 0) {
            SDL_Log("屏幕分辨率为: %dx%dpx @%dhz", vMode.w, vMode.h, vMode.refresh_rate);
        }
    }

    // 初始化窗体
    SDL_Window *win;
    win = SDL_CreateWindow("SDL测试", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!win) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, SDL_GetError());
        return 1;
    }

    // 初始化OPENGL 上下文
    SDL_GLContext ctx;
    ctx = SDL_GL_CreateContext(win);
    SDL_GL_MakeCurrent(win, ctx);

    SDL_Log("Version: %s", glGetString(GL_VERSION));
    SDL_Log("Vendor: %s", glGetString(GL_VENDOR));
    SDL_Log("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    SDL_SetWindowSize(win, vMode.w, vMode.h);
    //SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_Log("Driver: %s", SDL_GetHint(SDL_HINT_RENDER_DRIVER));

    // 初始化Render
    SDL_Renderer *render;
    render = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC);

    if (!render) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, SDL_GetError());
        return 1;
    }

    SDL_GL_SetSwapInterval(-1);

    // 主循环
    SDL_Event event;
    while (!isQuit) {
        SDL_RenderClear(render);
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    isQuit = TRUE;
                }
            case SDL_QUIT:
                isQuit = TRUE;
            default:
                break;
            }
        }
        SDL_GL_SwapWindow(win);
        SDL_RenderPresent(render);
    }

    lm->Dispose();

    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
};
