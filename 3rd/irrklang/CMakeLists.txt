add_library(irrk STATIC IMPORTED)
#add_library(irrKlang.lib STATIC INTERFACE)

target_link_libraries(irrk "${CMAKE_SOURCE_DIR}/3rd/irrKlang/irrKlang.lib")
