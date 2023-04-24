//
// Created by iamfr on 2023/4/23.
//
#include <imgui.h>

extern "C" {
#include "uicomponents.h"
#include <log.h>
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}

static int amiffy_begin_window( lua_State* L )
{
    const char* str    = lua_tostring( L, 1 );
    bool        active = lua_toboolean( L, 2 );

    bool rvl = ImGui::Begin( str, &active, 0 );

    //    log_info( "%s", str );

    lua_pushboolean( L, rvl );
    return 1;
}

static int amiffy_end_window( lua_State* L )
{
    ImGui::End();
    return 0;
}

static int amiffy_button( lua_State* L )
{
    const char* str       = lua_tostring( L, 1 );
    bool        isClicked = ImGui::Button( str );

    lua_pushboolean( L, isClicked );
    return 1;
}

static int amiffy_text( lua_State* L )
{
    const char* str = lua_tostring( L, 1 );
    ImGui::BulletText( str );
    return 0;
}

namespace Amiffy {

using namespace ImGui;


unsigned int Color( unsigned int c )
{
    // add alpha.
    // also swap red and blue channel for some reason.
    // todo: figure out why, and fix it.
    const short a = 0xFF;
    const short r = ( c >> 16 ) & 0xFF;
    const short g = ( c >> 8 ) & 0xFF;
    const short b = ( c >> 0 ) & 0xFF;
    return ( a << 24 ) | ( r << 0 ) | ( g << 8 ) | ( b << 16 );
}

inline ImVec4 Hex2Vec4( unsigned int c )
{
    return ColorConvertU32ToFloat4( Color( c ) );
}

void AmiffyUIComponents::useLightTheme()
{
    ImGuiStyle* style = &ImGui::GetStyle();

    style->GrabRounding = 4.0f;

    ImVec4* colors = style->Colors;

    colors [ImGuiCol_Text]                 = Hex2Vec4( 0x505050 );
    colors [ImGuiCol_TextDisabled]         = Hex2Vec4( 0xBCBCBC );
    colors [ImGuiCol_WindowBg]             = Hex2Vec4( 0xF5F5F5 );
    colors [ImGuiCol_ChildBg]              = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
    colors [ImGuiCol_PopupBg]              = Hex2Vec4( 0xFFFFFF );
    colors [ImGuiCol_Border]               = Hex2Vec4( 0xCACACA );
    colors [ImGuiCol_BorderShadow]         = Hex2Vec4( 0X000000 );
    colors [ImGuiCol_FrameBg]              = Hex2Vec4( 0xFAFAFA );
    colors [ImGuiCol_FrameBgHovered]       = Hex2Vec4( 0xFFFFFF );
    colors [ImGuiCol_FrameBgActive]        = Hex2Vec4( 0xEAEAEA );
    colors [ImGuiCol_TitleBg]              = Hex2Vec4( 0xE1E1E1 );
    colors [ImGuiCol_TitleBgActive]        = Hex2Vec4( 0xEAEAEA );
    colors [ImGuiCol_TitleBgCollapsed]     = Hex2Vec4( 0xCACACA );
    colors [ImGuiCol_MenuBarBg]            = Hex2Vec4( 0xF5F5F5 );
    colors [ImGuiCol_ScrollbarBg]          = Hex2Vec4( 0xF5F5F5 );
    colors [ImGuiCol_ScrollbarGrab]        = Hex2Vec4( 0xCACACA );
    colors [ImGuiCol_ScrollbarGrabHovered] = Hex2Vec4( 0x8E8E8E );
    colors [ImGuiCol_ScrollbarGrabActive]  = Hex2Vec4( 0x707070 );
    colors [ImGuiCol_CheckMark]            = Hex2Vec4( 0x1473E6 );
    colors [ImGuiCol_SliderGrab]           = Hex2Vec4( 0x707070 );
    colors [ImGuiCol_SliderGrabActive]     = Hex2Vec4( 0x505050 );
    colors [ImGuiCol_Button]               = Hex2Vec4( 0xFAFAFA );
    colors [ImGuiCol_ButtonHovered]        = Hex2Vec4( 0xFFFFFF );
    colors [ImGuiCol_ButtonActive]         = Hex2Vec4( 0xEAEAEA );
    colors [ImGuiCol_Header]               = Hex2Vec4( 0x2680EB );
    colors [ImGuiCol_HeaderHovered]        = Hex2Vec4( 0x1473E6 );
    colors [ImGuiCol_HeaderActive]         = Hex2Vec4( 0x0D66D0 );
    colors [ImGuiCol_Separator]            = Hex2Vec4( 0xCACACA );
    colors [ImGuiCol_SeparatorHovered]     = Hex2Vec4( 0x8E8E8E );
    colors [ImGuiCol_SeparatorActive]      = Hex2Vec4( 0x707070 );
    colors [ImGuiCol_ResizeGrip]           = Hex2Vec4( 0xCACACA );
    colors [ImGuiCol_ResizeGripHovered]    = Hex2Vec4( 0x8E8E8E );
    colors [ImGuiCol_ResizeGripActive]     = Hex2Vec4( 0x707070 );
    colors [ImGuiCol_PlotLines]            = Hex2Vec4( 0x2680EB );
    colors [ImGuiCol_PlotLinesHovered]     = Hex2Vec4( 0x0D66D0 );
    colors [ImGuiCol_PlotHistogram]        = Hex2Vec4( 0x2680EB );
    colors [ImGuiCol_PlotHistogramHovered] = Hex2Vec4( 0x0D66D0 );
    colors [ImGuiCol_TextSelectedBg] =
        ColorConvertU32ToFloat4( ( Color( 0x2680EB ) & 0x00FFFFFF ) | 0x33000000 );
    colors [ImGuiCol_DragDropTarget] = ImVec4( 1.00f, 1.00f, 0.00f, 0.90f );
    colors [ImGuiCol_NavHighlight] =
        ColorConvertU32ToFloat4( ( Color( 0x2C2C2C ) & 0x00FFFFFF ) | 0x0A000000 );
    colors [ImGuiCol_NavWindowingHighlight] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
    colors [ImGuiCol_NavWindowingDimBg]     = ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );
    colors [ImGuiCol_ModalWindowDimBg]      = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );
}


void useDarkThemem() {}

int AmiffyUIComponents::luaopen_imgui( lua_State* L )
{
    luaL_Reg log [] = { { "begin_window", amiffy_begin_window },
                        { "end_window", amiffy_end_window },
                        { "button", amiffy_button },
                        { "text", amiffy_text },
                        { NULL, NULL } };
    luaL_newlib( L, log );

    log_info( "初始化imgui模块" );

    return 1;
}
}   // namespace Amiffy
