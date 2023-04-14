// #include <lua/lua.h>
#include <lua/lauxlib.h>
// #include <lua/lualib.h>

#include <stdio.h>

#include <windows.h>

#ifdef __cplusplus
#define EXT extern "C"
#else
#define EXT
#endif

#ifdef _WIN32 // 包含 win32和win64
#define LIB EXT __declspec(dllexport)
#else
#define LIB EXT
#endif

static int get_clipboard_content(lua_State *L) {
  HANDLE hclip;
  char *pbuf = NULL;

  if (OpenClipboard(NULL) == 0) {
    lua_pushstring(L, "发生了错误");
    return 1;
  }

  if (!IsClipboardFormatAvailable(CF_TEXT)) {
    CloseClipboard();

    lua_pushstring(L, "粘贴板中的内容不是文本");
    return 1;
  }

  hclip = GetClipboardData(CF_TEXT);
  pbuf = (char *)GlobalLock(hclip);
  GlobalUnlock(hclip);

  CloseClipboard();
  lua_pushstring(L, pbuf);
  return 1;
}

static luaL_Reg l[] = {{"get_clipboard_content", get_clipboard_content},
                       {NULL, NULL}};

LIB int luaopen_amiffy_ex(lua_State *L) {
  printf("初始化amiffy_ex模块");
  luaL_newlib(L, l);
  return 1;
}