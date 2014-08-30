// Copyright (c) 2014 amiffy.com.
// All rights reserved.

// Original author: fred ng <iamfredng@gmail.com>

// lua_manager.h: 定义了amiffy3d引擎和Lua脚本虚拟机的沟通、管理和操作。

#ifndef AMIFFY3D_SCRIPTS_LUA
#define AMIFFY3D_SCRIPTS_LUA

#include <iostream>

// 嵌入LUA脚本解析器
extern "C" {
#include <lua/lua.h>
}

namespace amiffy3d {


//Lua脚本管理器
class LuaManager {

public:

    LuaManager(void);

    //初始化管理器
    void Init(void);

    //加载脚本列表
    //@param[in] 脚本名称数组
    //@return 加载结果
    bool LoadScripts(const std::string lists[], const int list_size);

    //加载脚本
    //@param[in] 脚本路径
    //@return 加载结果
    bool Load(const std::string script_file_path);

    //执行lua脚本内的函数
    //@param[in] lua函数名称
    void ExecuteLuaFunction(const std::string lua_function_name);

    //销毁管理器
    void Dispose(void);

private:
    virtual ~LuaManager(void);

private:
    lua_State *lua_state_;

};

};
#endif // AMIFFY3D_SCRIPTS_LUA
