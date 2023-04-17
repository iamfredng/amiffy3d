# 指定库文件所在路径
FIND_LIBRARY(lua_LIBRARY lua.lib ${CMAKE_BINARY_DIR}/3rd/lua)

# 为了下游可以继续使用
set(lua_FOUND FALSE)
if (lua_LIBRARY)
	set(lua_FOUND TRUE)
endif()