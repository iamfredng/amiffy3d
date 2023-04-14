
#include "lua_print_stack.h"

#include <lua/lauxlib.h>
#include <log.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static void print_item(lua_State *L, int i, int as_key);

static int is_identifier(const char *s) {
    while (*s) {
        if (!isalnum(*s) && *s != '_') return 0;
        ++s;
    }
    return 1;
}

static int is_seq(lua_State *L, int i) {
    // stack = [..]
    lua_pushnil(L);
    // stack = [.., nil]
    int keynum = 1;
    while (lua_next(L, i)) {
        // stack = [.., key, value]
        lua_rawgeti(L, i, keynum);
        // stack = [.., key, value, t[keynum]]
        if (lua_isnil(L, -1)) {
            lua_pop(L, 3);
            // stack = [..]
            return 0;
        }
        lua_pop(L, 2);
        // stack = [.., key]
        keynum++;
    }
    // stack = [..]
    return 1;
}

static void print_seq(lua_State *L, int i) {
    log_debug("{");

    int k;
    for (k = 1;; ++k) {
        // stack = [..]
        lua_rawgeti(L, i, k);
        // stack = [.., t[k]]
        if (lua_isnil(L, -1)) break;
        if (k > 1) log_debug(", ");
        print_item(L, -1, 0);  // 0 --> as_key
        lua_pop(L, 1);
        // stack = [..]
    }
    // stack = [.., nil]
    lua_pop(L, 1);
    // stack = [..]

    log_debug("}");
}

static void print_table(lua_State *L, int i) {
    // Ensure i is an absolute index as we'll be pushing/popping things after it.
    if (i < 0) i = lua_gettop(L) + i + 1;

    const char *prefix = "{";
    if (is_seq(L, i)) {
        print_seq(L, i);  // This case includes all empty tables.
    } else {
        // stack = [..]
        lua_pushnil(L);
        // stack = [.., nil]
        while (lua_next(L, i)) {
            log_debug("%s", prefix);
            // stack = [.., key, value]
            print_item(L, -2, 1);  // 1 --> as_key
            log_debug(" = ");
            print_item(L, -1, 0);  // 0 --> as_key
            lua_pop(L, 1);  // So the last-used key is on top.
                                      // stack = [.., key]
            prefix = ", ";
        }
        // stack = [..]
        log_debug("}");
    }
}

static char *get_fn_string(lua_State *L, int i) {
    static char fn_name[1024];

    // Ensure i is an absolute index as we'll be pushing/popping things after it.
    if (i < 0) i = lua_gettop(L) + i + 1;

    // Check to see if the function has a global name.
    // stack = [..]
    lua_getglobal(L, "_G");
    // stack = [.., _G]
    lua_pushnil(L);
    // stack = [.., _G, nil]
    while (lua_next(L, -2)) {
        // stack = [.., _G, key, value]
        if (lua_rawequal(L, i, -1)) {
            log_debug("function:%s", lua_tostring(L, -2));
            lua_pop(L, 3);
            // stack = [..]
            return fn_name;
        }
        // stack = [.., _G, key, value]
        lua_pop(L, 1);
        // stack = [.., _G, key]
    }
    // If we get here, the function didn't have a global name; print a pointer.
    // stack = [.., _G]
    lua_pop(L, 1);
    // stack = [..]
    log_debug( "function:%p", lua_topointer(L, i));
    return fn_name;
}

static void print_item(lua_State *L, int i, int as_key) {
    int ltype = lua_type(L, i);
    // Set up first, last and start and end delimiters.
    const char *first = (as_key ? "[" : "");
    const char *last  = (as_key ? "]" : "");
    switch(ltype) {

    case LUA_TNIL:
        log_debug("nil");  // This can't be a key, so we can ignore as_key here.
        return;

    case LUA_TNUMBER:
        log_debug("%s%g%s", first, lua_tonumber(L, i), last);
        return;

    case LUA_TBOOLEAN:
        log_debug("%s%s%s", first, lua_toboolean(L, i) ? "true" : "false", last);
        return;

    case LUA_TSTRING:
    {
        const char *s = lua_tostring(L, i);
        if (is_identifier(s) && as_key) {
            log_debug("%s", s);
        } else {
            log_debug("%s'%s'%s", first, s, last);
        }
    }
        return;

    case LUA_TTABLE:
        log_debug("%s", first);
        print_table(L, i);
        log_debug("%s", last);
        return;

    case LUA_TFUNCTION:
        log_debug("%s%s%s", first, get_fn_string(L, i), last);
        return;

    case LUA_TUSERDATA:
    case LUA_TLIGHTUSERDATA:
        log_debug("%suserdata:", first);
        break;

    case LUA_TTHREAD:
        log_debug("%sthread:", first);
        break;

    default:
        log_debug("<internal_error_in_print_stack_item!>");
        return;
    }

    // If we reach here, then we've got a type that we print as a pointer.
    log_debug("%p%s", lua_topointer(L, i), last);
}

void print_stack(lua_State *L) {
    int n = lua_gettop(L);
    log_debug("stack:");
    int i;
    for (i = 1; i <= n; ++i) {
        log_debug(" ");
        print_item(L, i, 0);  // 0 --> as_key
    }
    if (n == 0) log_debug(" <empty>");
    log_debug("\n");
}