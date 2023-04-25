//
// Created by iamfr on 2023/4/22.
//

#include "ui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <d3d11.h>
#include <imgui.h>

#include "amiffyconf.h"
#include "uicomponents.h"

extern "C" {
#include <log.h>
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam,
                                                              LPARAM lParam );
namespace Amiffy {

static AmiffyUIData* g_uidata;
static AmiffyUI*     g_ui;

CHAR get_char( WPARAM wparam )
{
    CHAR a = NULL;
    for ( char ch = 'a'; ch <= 'z'; ch++ ) {
        if ( wparam == ch ) {
            a = ch;
        }
    }
    for ( char ch = 'A'; ch <= 'Z'; ch++ ) {
        if ( wparam == ch ) {
            a = ch;
        }
    }
    return a;
}

void Win32CreateRenderTarget( AmiffyUIData* data )
{
    ID3D11Texture2D* pBackBuffer;
    data->swapChain->GetBuffer( 0, IID_PPV_ARGS( &pBackBuffer ) );
    data->device->CreateRenderTargetView( pBackBuffer, nullptr, &data->renderTargetView );
    pBackBuffer->Release();
}

void Win32CleanupRenderTarget( AmiffyUIData* data )
{
    if ( data->renderTargetView != nullptr ) {
        data->renderTargetView->Release();
        data->renderTargetView = nullptr;
    }
}

void Win32CleanupDeviceD3D( AmiffyUIData* data )
{
    Win32CleanupRenderTarget( data );
    if ( data->swapChain ) {
        data->swapChain->Release();
        data->swapChain = nullptr;
    }
    if ( data->deviceContext ) {
        data->deviceContext->Release();
        data->deviceContext = nullptr;
    }
    if ( data->device ) {
        data->device->Release();
        data->device = nullptr;
    }
}

LRESULT WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if ( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) ) return true;

    switch ( msg ) {
    case WM_SIZE:
        if ( g_uidata->device != nullptr && wParam != SIZE_MINIMIZED ) {
            Win32CleanupRenderTarget( g_uidata );
            g_uidata->swapChain->ResizeBuffers(
                0, (UINT) LOWORD( lParam ), (UINT) HIWORD( lParam ), DXGI_FORMAT_UNKNOWN, 0 );
            Win32CreateRenderTarget( g_uidata );
        }
        return 0;
    case WM_SYSCOMMAND:
        if ( ( wParam & 0xfff0 ) == SC_KEYMENU )   // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY: ::PostQuitMessage( 0 ); return 0;
    case WM_KEYDOWN:
        if ( g_ui != nullptr ) {
            // key_callback( int key, int scancode, int action, int mods )
            g_ui->globalKeyCallback( get_char( wParam ), 0, 1, 0 );
        }
        break;
    case WM_KEYUP:
        if ( g_ui != nullptr ) {
            // key_callback( int key, int scancode, int action, int mods )
            g_ui->globalKeyCallback( get_char( wParam ), 0, 0, 0 );
        }
        break;
    }
    return ::DefWindowProcW( hWnd, msg, wParam, lParam );
}

bool Win32CreateDeviceD3D( AmiffyUIData* data )
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount                        = 2;
    sd.BufferDesc.Width                   = 0;
    sd.BufferDesc.Height                  = 0;
    sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator   = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags                              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow                       = data->hwnd;
    sd.SampleDesc.Count                   = 1;
    sd.SampleDesc.Quality                 = 0;
    sd.Windowed                           = TRUE;
    sd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL       featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray [2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };
    HRESULT res = D3D11CreateDeviceAndSwapChain( nullptr,
                                                 D3D_DRIVER_TYPE_HARDWARE,
                                                 nullptr,
                                                 createDeviceFlags,
                                                 featureLevelArray,
                                                 2,
                                                 D3D11_SDK_VERSION,
                                                 &sd,
                                                 &data->swapChain,
                                                 &data->device,
                                                 &featureLevel,
                                                 &data->deviceContext );
    if ( res == DXGI_ERROR_UNSUPPORTED )   // Try high-performance WARP software driver if hardware
                                           // is not available.
        res = D3D11CreateDeviceAndSwapChain( nullptr,
                                             D3D_DRIVER_TYPE_WARP,
                                             nullptr,
                                             createDeviceFlags,
                                             featureLevelArray,
                                             2,
                                             D3D11_SDK_VERSION,
                                             &sd,
                                             &data->swapChain,
                                             &data->device,
                                             &featureLevel,
                                             &data->deviceContext );
    if ( res != S_OK ) return false;

    Win32CreateRenderTarget( data );
    return true;
}

void AmiffyUI::globalKeyCallback( int key, int scancode, int action, int mods )
{
    for ( int i = 0; i < keyCallbackHandlers.size(); i++ ) {
        auto item = keyCallbackHandlers [i];
        if ( nullptr != item ) {
            item( key, scancode, action, mods );
        }
    }
    amiffy->globalKeyCallback( key, scancode, action, mods );
}

void AmiffyUI::createWindow()
{
    ImGui_ImplWin32_EnableDpiAwareness();

    WNDCLASSEXW wcex   = {};
    wcex.cbSize        = sizeof( WNDCLASSEXW );
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = WndProc;
    wcex.hInstance     = GetModuleHandle( nullptr );
    wcex.hIcon         = nullptr;
    wcex.hIconSm       = nullptr;
    wcex.hCursor       = nullptr;
    wcex.hbrBackground = nullptr;
    wcex.lpszClassName = L"Amiffy";

    ::RegisterClassExW( &wcex );
    HWND hwnd = ::CreateWindowExW( 0,
                                   wcex.lpszClassName,
                                   L"Dear ImGui DirectX11 Example",
                                   WS_OVERLAPPEDWINDOW,
                                   100,
                                   100,
                                   1280,
                                   800,
                                   nullptr,
                                   nullptr,
                                   wcex.hInstance,
                                   this );

    uidata.wc   = &wcex;
    uidata.hwnd = hwnd;

    if ( !Win32CreateDeviceD3D( &uidata ) ) {
        Win32CleanupDeviceD3D( &uidata );
        ::UnregisterClassW( uidata.wc->lpszClassName, uidata.wc->hInstance );
        log_error( "初始化d3d失败" );
        return;
    }

    ::ShowWindow( uidata.hwnd, SW_SHOWDEFAULT );
    ::UpdateWindow( uidata.hwnd );
}

void AmiffyUI::desotryWindow()
{
    Win32CleanupDeviceD3D( &uidata );
    ::DestroyWindow( uidata.hwnd );
    //    ::UnregisterClassExW( uidata.wc->lpszClassName, uidata.wc->hInstance );
}

AmiffyUI::AmiffyUI( Amiffy* _amiffy )
{
    amiffy   = _amiffy;
    runing   = false;
    uidata   = AmiffyUIData();
    g_uidata = &uidata;
    g_ui     = this;
}

AmiffyUI::~AmiffyUI()
{
    std::destroy( keyCallbackHandlers.begin(), keyCallbackHandlers.end() );
    amiffy = nullptr;
}

void AmiffyUI::openUIModule() const
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    io.IniFilename = "";   // 禁用imgui.ini 窗口大小配置
    io.Fonts->AddFontFromFileTTF( AMIFFY_DEFAULT_FONT_NAME,
                                  AMIFFY_DEFAULT_FONT_SIZE,
                                  nullptr,
                                  io.Fonts->GetGlyphRangesChineseSimplifiedCommon() );
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls

                                                            //    ImGui::StyleColorsLight();
    ImGui::StyleColorsClassic();
    //    ImGui::StyleColorsDark();
    //    uiComponents.useLightTheme();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init( uidata.hwnd );
    ImGui_ImplDX11_Init( uidata.device, uidata.deviceContext );
}

void AmiffyUI::beginUIEventLoop()
{
    // Main loop
    while ( !runing ) {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while ( ::PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) ) {
            ::TranslateMessage( &msg );
            ::DispatchMessage( &msg );
            if ( msg.message == WM_QUIT ) runing = true;
        }
        if ( runing ) break;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        RECT rect;
        GetClientRect( uidata.hwnd, &rect );
        int width  = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        //        ImGui::ShowDemoWindow();

        bool isopen = true;
        ImGui::Begin( "window", &isopen, 0 );
        //        ImGui::SameLine();
        //        ImGui::BeginGroup();

        //        ImGui::SameLine();

        ImGui::Button( "button 1" );
        ImGui::SameLine();
        ImGui::Button( "button 1" );
        ImGui::SameLine();
        ImGui::Button( "button 1" );
        //        ImGui::SameLine();
        ImGui::Button( "button 1" );
        //        ImGui::SameLine();
        ImGui::Button( "button 1" );
        //        ImGui::SameLine();
        ImGui::Button( "button 1" );

        //        ImGui::EndGroup();

        ImGui::End();

        if ( nullptr != frameHandler ) {
            frameHandler( width, height );
        }

        //        if ( nullptr != _frame_update_handler ) {
        //            _frame_update_handler( width, height );
        //        }

        // Rendering
        ImGui::Render();
        changeBgColor( 0, 0, 0, 1 );
        uidata.deviceContext->OMSetRenderTargets( 1, &uidata.renderTargetView, nullptr );
        uidata.deviceContext->ClearRenderTargetView( uidata.renderTargetView, bgcolor );
        ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );

        uidata.swapChain->Present( 1, 0 );   // Present with vsync
        //        swapChain->Present( 0, 0 );   // Present without vsync
    }
}

void AmiffyUI::changeBgColor( float r, float g, float b, float a )
{
    bgcolor [0] = r;
    bgcolor [1] = g;
    bgcolor [2] = b;
    bgcolor [3] = a;
}

void AmiffyUI::abortUiEventLoop()
{
    runing = false;
}
//
// void register_ui_window_key_callback( Amiffy::Amiffy* amiffy )
//{
//    _amiffy = amiffy;
//}
//
// void register_ui_frame_update_handler( frame_update_handler handler )
//{
//    _frame_update_handler = handler;
//}


// void AmiffyUI::install_ui_script_module()
//{
//     lua_getglobal( lua_state, "package" );
//     lua_getfield( lua_state, -1, "preload" );
//
//     lua_pushcfunction( lua_state, Amiffy::AmiffyUIComponents::luaopen_imgui );
//     lua_setfield( lua_state, -2, "imgui" );
//     lua_pop( lua_state, 2 );
// }

void AmiffyUI::initUIEnv()
{
    //    lua_pushnumber( lua_state, 1 );
    //    lua_setglobal( lua_state, "NK_WINDOW_BORDER" );
    //    lua_pushnumber( lua_state, 2 );
    //    lua_setglobal( lua_state, "NK_WINDOW_MOVABLE" );
    //    lua_pushnumber( lua_state, 3 );
    //    lua_setglobal( lua_state, "NK_WINDOW_SCALABLE" );
    //    lua_pushnumber( lua_state, 4 );
    //    lua_setglobal( lua_state, "NK_WINDOW_CLOSABLE" );
    //    lua_pushnumber( lua_state, 5 );
    //    lua_setglobal( lua_state, "NK_WINDOW_MINIMIZABLE" );
    //    lua_pushnumber( lua_state, 6 );
    //    lua_setglobal( lua_state, "NK_WINDOW_NO_SCROLLBAR" );
    //    lua_pushnumber( lua_state, 7 );
    //    lua_setglobal( lua_state, "NK_WINDOW_TITLE" );
    //    lua_pushnumber( lua_state, 8 );
    //    lua_setglobal( lua_state, "NK_WINDOW_SCROLL_AUTO_HIDE" );
    //    lua_pushnumber( lua_state, 9 );
    //    lua_setglobal( lua_state, "NK_WINDOW_BACKGROUND" );
    //    lua_pushnumber( lua_state, 11 );
    //    lua_setglobal( lua_state, "NK_WINDOW_SCALE_LEFT" );
    //    lua_pushnumber( lua_state, 10 );
    //    lua_setglobal( lua_state, "NK_WINDOW_NO_INPUT" );
}

void AmiffyUI::closeUIModule()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void AmiffyUI::registerUIFrameUpdate( std::function<void( int, int )> const& handler )
{
    frameHandler = handler;
}

void AmiffyUI::registerKeyCallback( std::function<void( int, int, int, int )> const& handler )
{
    keyCallbackHandlers.push_back( handler );
}
}   // namespace Amiffy
