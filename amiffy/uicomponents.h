//
// Created by iamfr on 2023/4/23.
//

#ifndef AMIFFY_UICOMPONENTS_H_
#define AMIFFY_UICOMPONENTS_H_

#include <lua/lua.h>

namespace Amiffy {


class AmiffyUIComponents
{
public:
    void useLightTheme();
    void useDarkThemem();
    static int  luaopen_imgui( lua_State* L );
};

}   // namespace Amiffy
#endif   // AMIFFY_UICOMPONENTS_H_
