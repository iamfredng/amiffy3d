// Copyright (c) 2014 amiffy.com.
// All rights reserved.

// Original author: fred ng <iamfredng@gmail.com>

// lua_manager.h: ������amiffy3d�����Lua�ű�������Ĺ�ͨ������Ͳ�����

#ifndef AMIFFY3D_SCRIPTS_LUA
#define AMIFFY3D_SCRIPTS_LUA

#include <iostream>

// Ƕ��LUA�ű�������
extern "C" {
#include <lua/lua.h>
}

namespace amiffy3d {


//Lua�ű�������
class LuaManager {

public:

    LuaManager(void);

    //��ʼ��������
    void Init(void);

    //���ؽű��б�
    //@param[in] �ű���������
    //@return ���ؽ��
    bool LoadScripts(const std::string lists[], const int list_size);

    //���ؽű�
    //@param[in] �ű�·��
    //@return ���ؽ��
    bool Load(const std::string script_file_path);

    //ִ��lua�ű��ڵĺ���
    //@param[in] lua��������
    void ExecuteLuaFunction(const std::string lua_function_name);

    //���ٹ�����
    void Dispose(void);

private:
    virtual ~LuaManager(void);

private:
    lua_State *lua_state_;

};

};
#endif // AMIFFY3D_SCRIPTS_LUA
