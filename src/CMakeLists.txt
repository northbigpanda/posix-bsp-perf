cmake_minimum_required(VERSION 3.12)

option(BUILD_SRC_FRAMEWORK "Build src/framework" ON)
option(BUILD_SRC_SHARED "Build src/shared" ON)
option(BUILD_SRC_CORE "Build src/core" OFF)
option(BUILD_SRC_PROFILER "Build profiler" ON)
option(BUILD_SRC_BSP_SOCKETS "Build bsp_sockets" ON)

# 根据选项包含子项目的CMakeLists.txt
if(BUILD_SRC_FRAMEWORK)
    add_subdirectory(framework)
endif()

if(BUILD_SRC_SHARED)
    add_subdirectory(shared)
endif()

if(BUILD_SRC_CORE)
    add_subdirectory(core)
endif()

if(BUILD_SRC_PROFILER)
    add_subdirectory(profiler)
endif()

if(BUILD_SRC_BSP_SOCKETS)
    add_subdirectory(bsp_sockets)
endif()