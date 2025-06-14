# RANOnline EP7 AI Simulation System - C++ Ultimate Edition
# 终极 C++ + Qt6 科技感 AI应用程序主构建配置
# 开发团队: Jy技术团队
# 创建日期: 2025年6月14日

cmake_minimum_required(VERSION 3.20)
project(RANOnline_CPP_Ultimate 
    VERSION 2.0.0 
    DESCRIPTION "RANOnline EP7 AI Simulation System - C++ Ultimate Edition"
    LANGUAGES CXX
)

# ================================================
# 🔧 编译器和标准设置
# ================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Windows特定设置
if(WIN32)
    # 启用多核编译
    add_compile_options("/MP")
    # 静态运行时库（可选）
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # Unicode支持
    add_definitions(-DUNICODE -D_UNICODE)
endif()

# ================================================
# 🎯 构建类型配置
# ================================================
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Debug配置
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
# Release配置
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# ================================================
# 📦 依赖包查找
# ================================================

# Qt6查找和配置
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Network 
    Sql 
    Concurrent
)

# 设置Qt6自动化工具
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Protobuf查找（可选）
find_package(Protobuf QUIET)
if(Protobuf_FOUND)
    message(STATUS "✅ Protobuf found: ${Protobuf_VERSION}")
    set(HAVE_PROTOBUF TRUE)
else()
    message(STATUS "⚠️ Protobuf not found, some features may be disabled")
    set(HAVE_PROTOBUF FALSE)
endif()

# 线程库
find_package(Threads REQUIRED)

# ODBC库（Windows，可选）
if(WIN32)
    find_library(ODBC_LIBRARY odbc32)
    if(ODBC_LIBRARY)
        message(STATUS "✅ ODBC library found: ${ODBC_LIBRARY}")
        set(HAVE_ODBC TRUE)
    else()
        message(STATUS "⚠️ ODBC library not found, database features may be limited")
        set(HAVE_ODBC FALSE)
    endif()
else()
    set(HAVE_ODBC FALSE)
endif()

# ================================================
# 🔗 全局包含目录
# ================================================
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Communication_Protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/Database_Sync_Module
    ${CMAKE_CURRENT_SOURCE_DIR}/AI_LLM_Integration
)

# 添加Protobuf包含目录（如果找到）
if(HAVE_PROTOBUF)
    include_directories(${Protobuf_INCLUDE_DIRS})
endif()

# ================================================
# 📁 子项目目录
# ================================================

# 通信协议模块（最先构建）
add_subdirectory(Communication_Protocol)

# 数据库同步模块
add_subdirectory(Database_Sync_Module)

# AI LLM集成模块
add_subdirectory(AI_LLM_Integration)

# AI后端推理引擎
add_subdirectory(AI_Backend_Engine)

# AI前端管理界面
add_subdirectory(AI_Frontend_Manager)

# ================================================
# 🧪 测试配置（可选）
# ================================================
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()

# ================================================
# 📋 安装配置
# ================================================
install(TARGETS 
    ai_frontend_manager 
    ai_backend_engine
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 安装配置文件
install(DIRECTORY Config/ DESTINATION config)

# 安装资源文件
install(DIRECTORY Resources/ DESTINATION resources)

# ================================================
# 📦 打包配置
# ================================================
set(CPACK_PACKAGE_NAME "RANOnline_CPP_Ultimate")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RANOnline EP7 AI Simulation System - C++ Ultimate Edition")
set(CPACK_PACKAGE_VENDOR "Jy技术团队")
set(CPACK_PACKAGE_CONTACT "support@jytech.team")

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "RANOnline AI System Ultimate")
    set(CPACK_NSIS_PACKAGE_NAME "RANOnline AI System Ultimate")
    set(CPACK_NSIS_CONTACT "support@jytech.team")
    set(CPACK_NSIS_HELP_LINK "https://jytech.team/ranonline")
endif()

include(CPack)

# ================================================
# 📊 构建信息输出
# ================================================
message(STATUS "")
message(STATUS "🚀 RANOnline EP7 AI Simulation System - C++ Ultimate Edition")
message(STATUS "================================================")
message(STATUS "📋 项目版本: ${PROJECT_VERSION}")
message(STATUS "🔧 构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "💻 编译器: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "🎯 C++标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "📦 Qt版本: ${Qt6_VERSION}")
message(STATUS "🔗 Protobuf版本: ${Protobuf_VERSION}")
message(STATUS "================================================")
message(STATUS "")
