cmake_minimum_required(VERSION 3.10)
project(UDX710_UOOLS VERSION 1.0 LANGUAGES C)

# 设置编译选项
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

# 交叉编译配置
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif()

# 源文件收集
file(GLOB MAIN_SRCS src/main.c src/mongoose.c src/packed_fs.c)
file(GLOB HANDLER_SRCS src/handlers/*.c)
file(GLOB SYSTEM_SRCS src/system/*.c)
set(SOURCES ${MAIN_SRCS} ${HANDLER_SRCS} ${SYSTEM_SRCS})

# 包含目录
include_directories(
    src
    src/handlers
    src/system
    ${CMAKE_SOURCE_DIR}/share/include/glib-2.0
    ${CMAKE_SOURCE_DIR}/share/lib/glib-2.0/include
)

# 链接目录
link_directories(${CMAKE_SOURCE_DIR}/share/lib)

# 编译目标
add_executable(ofono-server ${SOURCES})

# 链接选项
target_compile_options(ofono-server PRIVATE
    -Wall -O2 -g -DMG_ENABLE_LINES=0 -DDISABLE_PRINTF
)

target_link_libraries(ofono-server PRIVATE
    gio-2.0 gobject-2.0 glib-2.0 gmodule-2.0 pthread
    -Wl,-rpath-link,${CMAKE_SOURCE_DIR}/share/lib
    -Wl,--allow-shlib-undefined
)

# 安装目标
install(TARGETS ofono-server DESTINATION bin)

# 前端编译集成
add_custom_target(frontend_build
    COMMAND cd web && npm install && npm run build
    COMMAND cd src && python3 pack_fs.py  # 假设存在打包脚本
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# 确保前端编译在后端之前完成
add_dependencies(ofono-server frontend_build)
