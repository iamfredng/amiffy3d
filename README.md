# amiffy3d

个人用 gl3+imgui 引擎

amiffy3d.exe 采用一些 **通用约定** 作为默认路径

lua/init.lua 作为主要初始化脚本

fonts/harmony.ttf 作为默认字体

最小目录结构

```
- amiffy/
  - amiffy3d.exe
  - lua/
    - init.lua
  - fonts/
    - harmony.ttf
```

cmake:
 cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=<vcpkg root>/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_CXX_FLAGS:STRING="-m64"
