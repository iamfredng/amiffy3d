//
// Created by iamfr on 2023/4/23.
//

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <imgui.h>

#include "uicomponents.h"

extern "C" {
#include <log.h>
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}

namespace ImGui {
inline ImVec2 Vec2MinVec2( ImVec2 v1, ImVec2 v2 )
{
    return ImVec2( v1.x - v2.x, v1.y - v1.y );
}
inline ImVec2 Vec2PlusVec2( ImVec2 v1, ImVec2 v2 )
{
    return ImVec2( v1.x + v2.x, v1.y + v2.y );
}

// Implementation
IMGUI_API
bool ColoredButtonV1( const char* label, const ImVec2& size_arg, ImU32 text_color, ImU32 bg_color_1,
                      ImU32 bg_color_2 )
{
    ImGuiWindow* window = GetCurrentWindow();
    if ( window->SkipItems ) return false;

    ImGuiContext&     g          = *GImGui;
    const ImGuiStyle& style      = g.Style;
    const ImGuiID     id         = window->GetID( label );
    const ImVec2      label_size = CalcTextSize( label, NULL, true );

    ImVec2 pos  = window->DC.CursorPos;
    ImVec2 size = CalcItemSize( size_arg,
                                label_size.x + style.FramePadding.x * 2.0f,
                                label_size.y + style.FramePadding.y * 2.0f );

    const ImRect bb( pos, Vec2PlusVec2( pos, size ) );
    ItemSize( size, style.FramePadding.y );
    if ( !ItemAdd( bb, id ) ) return false;

    ImGuiButtonFlags flags = ImGuiButtonFlags_None;
    if ( g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat ) flags |= ImGuiButtonFlags_Repeat;

    bool hovered, held;
    bool pressed = ButtonBehavior( bb, id, &hovered, &held, flags );

    // Render
    const bool is_gradient = bg_color_1 != bg_color_2;
    if ( held || hovered ) {
        // Modify colors (ultimately this can be prebaked in the style)
        float h_increase = ( held && hovered ) ? 0.02f : 0.02f;
        float v_increase = ( held && hovered ) ? 0.20f : 0.07f;

        ImVec4 bg1f = ColorConvertU32ToFloat4( bg_color_1 );
        ColorConvertRGBtoHSV( bg1f.x, bg1f.y, bg1f.z, bg1f.x, bg1f.y, bg1f.z );
        bg1f.x = ImMin( bg1f.x + h_increase, 1.0f );
        bg1f.z = ImMin( bg1f.z + v_increase, 1.0f );
        ColorConvertHSVtoRGB( bg1f.x, bg1f.y, bg1f.z, bg1f.x, bg1f.y, bg1f.z );
        bg_color_1 = GetColorU32( bg1f );
        if ( is_gradient ) {
            ImVec4 bg2f = ColorConvertU32ToFloat4( bg_color_2 );
            ColorConvertRGBtoHSV( bg2f.x, bg2f.y, bg2f.z, bg2f.x, bg2f.y, bg2f.z );
            bg2f.z = ImMin( bg2f.z + h_increase, 1.0f );
            bg2f.z = ImMin( bg2f.z + v_increase, 1.0f );
            ColorConvertHSVtoRGB( bg2f.x, bg2f.y, bg2f.z, bg2f.x, bg2f.y, bg2f.z );
            bg_color_2 = GetColorU32( bg2f );
        }
        else {
            bg_color_2 = bg_color_1;
        }
    }
    RenderNavHighlight( bb, id );

#if 0
    // V1 : faster but prevents rounding
    window->DrawList->AddRectFilledMultiColor(bb.Min, bb.Max, bg_color_1, bg_color_1, bg_color_2, bg_color_2);
    if (g.Style.FrameBorderSize > 0.0f)
        window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_Border), 0.0f, 0, g.Style.FrameBorderSize);
#endif

    // V2
    int vert_start_idx = window->DrawList->VtxBuffer.Size;
    window->DrawList->AddRectFilled( bb.Min, bb.Max, bg_color_1, g.Style.FrameRounding );
    int vert_end_idx = window->DrawList->VtxBuffer.Size;
    if ( is_gradient )
        ShadeVertsLinearColorGradientKeepAlpha( window->DrawList,
                                                vert_start_idx,
                                                vert_end_idx,
                                                bb.Min,
                                                bb.GetBL(),
                                                bg_color_1,
                                                bg_color_2 );
    if ( g.Style.FrameBorderSize > 0.0f )
        window->DrawList->AddRect( bb.Min,
                                   bb.Max,
                                   GetColorU32( ImGuiCol_Border ),
                                   g.Style.FrameRounding,
                                   0,
                                   g.Style.FrameBorderSize );

    if ( g.LogEnabled ) LogSetNextTextDecoration( "[", "]" );
    PushStyleColor( ImGuiCol_Text, text_color );
    RenderTextClipped( Vec2PlusVec2( bb.Min, style.FramePadding ),
                       Vec2MinVec2( bb.Max, style.FramePadding ),
                       label,
                       NULL,
                       &label_size,
                       style.ButtonTextAlign,
                       &bb );
    PopStyleColor();

    IMGUI_TEST_ENGINE_ITEM_INFO( id, label, g.LastItemData.StatusFlags );
    return pressed;
}
}   // namespace ImGui


namespace Amiffy {

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
    const char* str = lua_tostring( L, 1 );

    bool isClicked = ImGui::ColoredButtonV1( str,
                                             ImVec2( -FLT_MIN, 0.0f ),
                                             IM_COL32( 255, 255, 255, 255 ),
                                             IM_COL32( 200, 60, 60, 255 ),
                                             IM_COL32( 180, 40, 90, 255 ) );

    lua_pushboolean( L, isClicked );
    return 1;
}

static int amiffy_text( lua_State* L )
{
    const char* str = lua_tostring( L, 1 );
    ImGui::BulletText( str );
    return 0;
}

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
    return ImGui::ColorConvertU32ToFloat4( Color( c ) );
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
        ImGui::ColorConvertU32ToFloat4( ( Color( 0x2680EB ) & 0x00FFFFFF ) | 0x33000000 );
    colors [ImGuiCol_DragDropTarget] = ImVec4( 1.00f, 1.00f, 0.00f, 0.90f );
    colors [ImGuiCol_NavHighlight] =
        ImGui::ColorConvertU32ToFloat4( ( Color( 0x2C2C2C ) & 0x00FFFFFF ) | 0x0A000000 );
    colors [ImGuiCol_NavWindowingHighlight] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
    colors [ImGuiCol_NavWindowingDimBg]     = ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );
    colors [ImGuiCol_ModalWindowDimBg]      = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );
}


void useDarkThemem() {}

void AmiffyUIComponents::installUIComponents()
{
    //    luaL_Reg log [] = { { "begin_window", amiffy_begin_window },
    //                        { "end_window", amiffy_end_window },
    //                        { "button", amiffy_button },
    //                        { "text", amiffy_text },
    //                        { NULL, NULL } };
    //    luaL_newlib( L, log );
    //
    //    log_info( "初始化imgui模块" );
    //
    //    return 1;
}
AmiffyUIComponents::~AmiffyUIComponents()
{
    ui = nullptr;
}
AmiffyUIComponents::AmiffyUIComponents( AmiffyUI* _ui )
{
    ui = _ui;
}
}   // namespace Amiffy
