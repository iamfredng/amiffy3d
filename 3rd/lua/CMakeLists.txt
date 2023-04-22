add_library(lua ${LIBRARY_BUILD_TYPE}
        src/lapi.c src/lauxlib.c src/lbaselib.c src/lcode.c src/lcorolib.c
        src/lctype.c src/ldblib.c src/ldebug.c src/ldo.c src/ldump.c src/lfunc.c
        src/lgc.c src/linit.c src/liolib.c src/llex.c src/lmathlib.c
        src/lmem.c src/loadlib.c src/lobject.c src/lopcodes.c src/loslib.c
        src/lparser.c src/lstate.c src/lstring.c src/lstrlib.c src/ltable.c
        src/ltablib.c src/ltests.c src/ltm.c src/lundump.c src/lutf8lib.c
        src/lvm.c src/lzio.c
        )

target_include_directories(lua PRIVATE "include/lua")

if (MSVC OR CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    target_compile_definitions(logc PRIVATE _CRT_SECURE_NO_WARNINGS)
endif ()

set_target_properties(lua PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
        C_STANDARD 99)

target_compile_definitions(lua PUBLIC LUA_BUILD_AS_DLL)