#ifndef AMIFFY_SCRIPT_H_
#define AMIFFY_SCRIPT_H_

#include "amiffy.h"

namespace Amiffy {

class AmiffyScriptModule
{
public:
    void openScriptModule();
    void closeScriptModule();
    void installScriptModule();
    void reloadScriptModule();
    void initScriptEnv();
    void tick( int clientWidth, int clientHeight );
    explicit AmiffyScriptModule( Amiffy* _amiffy );
    ~AmiffyScriptModule();

private:
    Amiffy* amiffy;
    LuaVM vm;
};

}   // namespace Amiffy
//
// void open_script_module( double width, double height );
//
// void install_script_module();
//
// void init_script_env();
//
// void update_script_frame( double width, double height );
//
// void reload_script_module();
//
// void close_script_module();



#endif