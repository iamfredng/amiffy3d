//
// Created by iamfr on 2023/4/22.
//

#include "ui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <d3d11.h>
#include <imgui.h>

extern "C" {
#include "amiffy.h"
#include "uicomponents.h"
#include <log.h>
#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
}


// Data
static ID3D11Device*           g_pd3dDevice           = nullptr;
static ID3D11DeviceContext*    g_pd3dDeviceContext    = nullptr;
static IDXGISwapChain*         g_pSwapChain           = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

static ImVec4      clear_color = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );
static WNDCLASSEXW wc;
static HWND        hwnd;
static bool        done;

static frame_update_handler _frame_update_handler;
static window_key_callback  _window_key_callback;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam,
                                                              LPARAM lParam );

Amiffy::AmiffyUIComponents uiComponents;

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

// Forward declarations of helper functions
static void Win32CleanupRenderTarget()
{
    if ( g_mainRenderTargetView ) {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

static void Win32CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer( 0, IID_PPV_ARGS( &pBackBuffer ) );
    g_pd3dDevice->CreateRenderTargetView( pBackBuffer, nullptr, &g_mainRenderTargetView );
    pBackBuffer->Release();
}

static bool Win32CreateDeviceD3D( HWND hWnd )
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
    sd.OutputWindow                       = hWnd;
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
                                                 &g_pSwapChain,
                                                 &g_pd3dDevice,
                                                 &featureLevel,
                                                 &g_pd3dDeviceContext );
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
                                             &g_pSwapChain,
                                             &g_pd3dDevice,
                                             &featureLevel,
                                             &g_pd3dDeviceContext );
    if ( res != S_OK ) return false;

    Win32CreateRenderTarget();
    return true;
}

static void Win32CleanupDeviceD3D()
{
    Win32CleanupRenderTarget();
    if ( g_pSwapChain ) {
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if ( g_pd3dDeviceContext ) {
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = nullptr;
    }
    if ( g_pd3dDevice ) {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
}

LRESULT WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if ( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) ) return true;

    switch ( msg ) {
    case WM_SIZE:
        if ( g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED ) {
            Win32CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(
                0, (UINT) LOWORD( lParam ), (UINT) HIWORD( lParam ), DXGI_FORMAT_UNKNOWN, 0 );
            Win32CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ( ( wParam & 0xfff0 ) == SC_KEYMENU )   // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY: ::PostQuitMessage( 0 ); return 0;
    case WM_KEYDOWN:
        if ( _window_key_callback != nullptr ) {
            // key_callback( int key, int scancode, int action, int mods )
            _window_key_callback( get_char( wParam ), 0, 1, 0 );
        }
        break;
    case WM_KEYUP:
        if ( _window_key_callback != nullptr ) {
            // key_callback( int key, int scancode, int action, int mods )
            _window_key_callback( get_char( wParam ), 0, 0, 0 );
        }
        break;
    }
    return ::DefWindowProcW( hWnd, msg, wParam, lParam );
}

void open_ui_module()
{
    // Create application window
    ImGui_ImplWin32_EnableDpiAwareness();

    wc = { sizeof( wc ),
           CS_CLASSDC,
           WndProc,
           0L,
           0L,
           GetModuleHandle( nullptr ),
           nullptr,
           nullptr,
           nullptr,
           nullptr,
           L"ImGui Example",
           nullptr };
    ::RegisterClassExW( &wc );
    hwnd = ::CreateWindowW( wc.lpszClassName,
                            L"Dear ImGui DirectX11 Example",
                            WS_OVERLAPPEDWINDOW,
                            100,
                            100,
                            1280,
                            800,
                            nullptr,
                            nullptr,
                            wc.hInstance,
                            nullptr );

    // Initialize Direct3D
    if ( !Win32CreateDeviceD3D( hwnd ) ) {
        Win32CleanupDeviceD3D();
        ::UnregisterClassW( wc.lpszClassName, wc.hInstance );
        log_error( "初始化d3d失败" );
        return;
    }

    // Show the window
    ::ShowWindow( hwnd, SW_SHOWDEFAULT );
    ::UpdateWindow( hwnd );

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    io.IniFilename = ""; // 禁用imgui.ini 窗口大小配置
    io.Fonts->AddFontFromFileTTF( AMIFFY_DEFAULT_FONT_NAME,
                                  AMIFFY_DEFAULT_FONT_SIZE,
                                  nullptr,
                                  io.Fonts->GetGlyphRangesChineseSimplifiedCommon() );
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
//    ImGui::StyleColorsLight();
//    ImGui::StyleColorsClassic();
//    ImGui::StyleColorsDark();
    uiComponents.useLightTheme();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init( hwnd );
    ImGui_ImplDX11_Init( g_pd3dDevice, g_pd3dDeviceContext );
}

void begin_ui_event_loop()
{
    // Main loop
    while ( !done ) {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while ( ::PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) ) {
            ::TranslateMessage( &msg );
            ::DispatchMessage( &msg );
            if ( msg.message == WM_QUIT ) done = true;
        }
        if ( done ) break;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        RECT rect;
        GetClientRect( hwnd, &rect );
        int width  = rect.left - rect.right;
        int height = rect.top - rect.bottom;

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

        if ( nullptr != _frame_update_handler ) {
            _frame_update_handler( width, height );
        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha [4] = { clear_color.x * clear_color.w,
                                                   clear_color.y * clear_color.w,
                                                   clear_color.z * clear_color.w,
                                                   clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets( 1, &g_mainRenderTargetView, nullptr );
        g_pd3dDeviceContext->ClearRenderTargetView( g_mainRenderTargetView,
                                                    clear_color_with_alpha );
        ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );

        g_pSwapChain->Present( 1, 0 );   // Present with vsync
        //        g_pSwapChain->Present( 0, 0 );   // Present without vsync
    }
}

void abort_ui_event_loop()
{
    done = false;
}

void register_ui_window_key_callback( window_key_callback callback )
{
    _window_key_callback = callback;
}

void register_ui_frame_update_handler( frame_update_handler handler )
{
    _frame_update_handler = handler;
}


void install_ui_script_module()
{
    lua_getglobal( lua_state, "package" );
    lua_getfield( lua_state, -1, "preload" );

    lua_pushcfunction( lua_state, Amiffy::AmiffyUIComponents::luaopen_imgui );
    lua_setfield( lua_state, -2, "imgui" );
    lua_pop( lua_state, 2 );
}

void init_ui_env()
{
    lua_pushnumber( lua_state, 1 );
    lua_setglobal( lua_state, "NK_WINDOW_BORDER" );
    lua_pushnumber( lua_state, 2 );
    lua_setglobal( lua_state, "NK_WINDOW_MOVABLE" );
    lua_pushnumber( lua_state, 3 );
    lua_setglobal( lua_state, "NK_WINDOW_SCALABLE" );
    lua_pushnumber( lua_state, 4 );
    lua_setglobal( lua_state, "NK_WINDOW_CLOSABLE" );
    lua_pushnumber( lua_state, 5 );
    lua_setglobal( lua_state, "NK_WINDOW_MINIMIZABLE" );
    lua_pushnumber( lua_state, 6 );
    lua_setglobal( lua_state, "NK_WINDOW_NO_SCROLLBAR" );
    lua_pushnumber( lua_state, 7 );
    lua_setglobal( lua_state, "NK_WINDOW_TITLE" );
    lua_pushnumber( lua_state, 8 );
    lua_setglobal( lua_state, "NK_WINDOW_SCROLL_AUTO_HIDE" );
    lua_pushnumber( lua_state, 9 );
    lua_setglobal( lua_state, "NK_WINDOW_BACKGROUND" );
    lua_pushnumber( lua_state, 11 );
    lua_setglobal( lua_state, "NK_WINDOW_SCALE_LEFT" );
    lua_pushnumber( lua_state, 10 );
    lua_setglobal( lua_state, "NK_WINDOW_NO_INPUT" );
}

void close_ui_module()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    Win32CleanupDeviceD3D();
    ::DestroyWindow( hwnd );
    ::UnregisterClassW( wc.lpszClassName, wc.hInstance );
}