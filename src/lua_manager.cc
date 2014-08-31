// Copyright (c) 2014 amiffy.com.
// All rights reserved.

// Original author: fred ng <iamfredng@gmail.com>

// lua_manager.cc: LuaManager类的实现，具体细节在lua_manager.h文件中。

#include "lua_manager.h"

#include <assert.h>

#include <SDL.h>

extern "C" {
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

namespace amiffy3d {

LuaManager::LuaManager(void) {

}

LuaManager::~LuaManager(void) {



    lua_close(lua_state_);
    lua_state_ = 0;
}

bool LuaManager::LoadScripts(const std::string lists[], const int list_size) {

    for (int i = 0; i < list_size; ++i) {
        SDL_Log("文件1: %s", lists[i].c_str());
        if (!Load(lists[i])) {
            return false; // 任何一个失败都失败
        }
    }
    return false;
}

bool LuaManager::Load(const std::string script_file_path) {
    if (luaL_loadfile(lua_state_, script_file_path.c_str()) || lua_pcall(lua_state_, 0, 0, 0)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                    "加载lua脚本失败，请检查文件： \"%s\"，错误信息：%s",
                    script_file_path.c_str(),
                    lua_tostring(lua_state_, -1));
        return false;
    }
    return true;
}

void LuaManager::Init(void) {
    lua_state_ = luaL_newstate();
    luaL_openlibs(lua_state_);
}

void LuaManager::Dispose(void) {
    delete this;
}

void LuaManager::ExecuteLuaFunction(const std::string lua_function_name) {
    //lua_getglobal(L, "getKey");
    //lua_pushstring(L, "a1");
    //lua_pushstring(L, "a2");
    //lua_pcall(L, 2, 1, 0);
    //lua_isstring(L, -1);
    //const char *rvl = lua_tostring(L, -1);
    ////rvl = "ghahaha";
    //lua_pop(L, 1);
    //std::cout << rvl << std::endl;
}

};

