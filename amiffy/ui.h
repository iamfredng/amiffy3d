#ifndef AMIFFY_UI_H_
#define AMIFFY_UI_H_

#ifdef _WIN32
#    include <d3d11.h>
#endif

#include "amiffy.h"

#include <functional>
#include <vector>

typedef void ( *frame_update_handler )( double width, double height );
typedef void ( *window_key_callback )( void*, int key, int scancode, int action, int mods );

namespace Amiffy {
#ifdef _WIN32
struct AmiffyUIData
{
    AmiffyUIData()
    {
        hwnd             = nullptr;
        wc               = nullptr;
        device           = nullptr;
        deviceContext    = nullptr;
        swapChain        = nullptr;
        renderTargetView = nullptr;
    }

    HWND                    hwnd;
    WNDCLASSEXW*            wc;
    ID3D11Device*           device;
    ID3D11DeviceContext*    deviceContext;
    IDXGISwapChain*         swapChain;
    ID3D11RenderTargetView* renderTargetView;
};
#endif

class AmiffyUI
{
public:
    void createWindow();
    void desotryWindow();
    void openUIModule() const;
    void beginUIEventLoop();
    void abortUiEventLoop();
    void registerUIFrameUpdate( std::function<void( int, int )> const& handler );
    void registerKeyCallback(
        std::function<void( int key, int scancode, int action, int mods )> const& handler );
    void initUIEnv();
    void closeUIModule();
    void changeBgColor( float r, float g, float b, float a );
    void globalKeyCallback( int key, int scancode, int action, int mods );
    explicit AmiffyUI( Amiffy* _amiffy );
    virtual ~AmiffyUI();

private:
    Amiffy*                                                amiffy = nullptr;
    bool                                                   runing;
    AmiffyUIData                                           uidata;
    float                                                  bgcolor [4]  = { 0, 0, 0, 0 };
    std::function<void( int, int )>                        frameHandler = nullptr;
    std::vector<std::function<void( int, int, int, int )>> keyCallbackHandlers;
};
}   // namespace Amiffy

#endif
