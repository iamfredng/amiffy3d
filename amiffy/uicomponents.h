//
// Created by iamfr on 2023/4/23.
//

#ifndef AMIFFY_UICOMPONENTS_H_
#define AMIFFY_UICOMPONENTS_H_

#include "ui.h"

namespace Amiffy {

class AmiffyUIComponents
{
public:
    void useLightTheme();
    void useDarkThemem();
    void installUIComponents();
    //    static int  luaopen_imgui( lua_State* L );
    explicit AmiffyUIComponents(AmiffyUI* _ui);
    ~AmiffyUIComponents();

private:
    AmiffyUI* ui;
};

}   // namespace Amiffy
#endif   // AMIFFY_UICOMPONENTS_H_
