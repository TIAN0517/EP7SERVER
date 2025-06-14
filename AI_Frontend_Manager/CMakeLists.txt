# RANOnline EP7 AI系统 - 前端管理界面 CMake配置
# 开发团队: Jy技术团队
# 创建日期: 2025年6月14日

cmake_minimum_required(VERSION 3.20)

# ================================================
# 📋 项目配置
# ================================================
set(TARGET_NAME "ai_frontend_manager")
set(PROJECT_DESCRIPTION "RANOnline AI Frontend Manager - Ultimate C++ Edition")

# ================================================
# 🔍 查找依赖包
# ================================================

# Qt6组件
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Network 
    Concurrent
    Svg
    Charts
)

# ================================================
# 📁 源文件收集
# ================================================

# 头文件
set(HEADERS
    MainWindow.h
    AiControlPanel.h
    StatusMonitor.h
    LogViewer.h
    NetworkManager.h
    PerformanceMonitor.h
    LoadBalancer.h
)

# 源文件
set(SOURCES
    main.cpp
    MainWindow.cpp
    AiControlPanel.cpp
    StatusMonitor.cpp
    LogViewer.cpp
    NetworkManager.cpp
    PerformanceMonitor.cpp
    LoadBalancer.cpp
)

# UI文件（如果使用Qt Designer）
set(UI_FILES
    # MainWindow.ui
    # AiControlPanel.ui
)

# 资源文件
set(RESOURCE_FILES
    resources.qrc
)

# ================================================
# 🎯 可执行文件目标
# ================================================
add_executable(${TARGET_NAME}
    ${HEADERS}
    ${SOURCES}
    ${UI_FILES}
    ${RESOURCE_FILES}
)

# ================================================
# 🔗 链接库
# ================================================
target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
        Qt6::Concurrent
        Qt6::Svg
        Qt6::Charts
        communication_protocol
        database_sync_module
)

# Windows平台特定链接
if(WIN32)
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            user32
            gdi32
            shell32
            ole32
            oleaut32
            uuid
            winmm
            imm32
            ws2_32
    )
endif()

# ================================================
# 📁 包含目录
# ================================================
target_include_directories(${TARGET_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../Communication_Protocol
        ${CMAKE_CURRENT_SOURCE_DIR}/../Database_Sync_Module
)

# ================================================
# 🔧 编译器特定设置
# ================================================

# MSVC编译器设置
if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE
        /W4                    # 警告级别4
        /WX-                   # 警告不作为错误处理
        /MP                    # 多核编译
        /permissive-           # 严格模式
        /Zc:__cplusplus        # 正确的__cplusplus宏
        /utf-8                 # UTF-8源代码编码
    )
    
    # Debug配置
    target_compile_options(${TARGET_NAME} PRIVATE
        $<$<CONFIG:Debug>:/Od>     # 禁用优化
        $<$<CONFIG:Debug>:/Zi>     # 生成调试信息
        $<$<CONFIG:Debug>:/RTC1>   # 运行时检查
    )
    
    # Release配置
    target_compile_options(${TARGET_NAME} PRIVATE
        $<$<CONFIG:Release>:/O2>   # 最大优化
        $<$<CONFIG:Release>:/GL>   # 全程序优化
        $<$<CONFIG:Release>:/LTCG> # 链接时代码生成
    )
endif()

# GCC/Clang编译器设置
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${TARGET_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
    )
    
    # Debug配置
    target_compile_options(${TARGET_NAME} PRIVATE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Debug>:-fsanitize=address>
    )
    
    # Release配置
    target_compile_options(${TARGET_NAME} PRIVATE
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-DNDEBUG>
        $<$<CONFIG:Release>:-flto>
    )
endif()

# ================================================
# 🎨 预编译定义
# ================================================
target_compile_definitions(${TARGET_NAME}
    PRIVATE
        # Qt相关定义
        QT_DEPRECATED_WARNINGS
        QT_DISABLE_DEPRECATED_BEFORE=0x060000
        QT_NO_DEBUG_OUTPUT=$<NOT:$<CONFIG:Debug>>
        
        # 应用程序信息
        APP_NAME="${TARGET_NAME}"
        APP_VERSION="${PROJECT_VERSION}"
        APP_DESCRIPTION="${PROJECT_DESCRIPTION}"
        
        # Windows平台定义
        $<$<PLATFORM_ID:Windows>:
            UNICODE
            _UNICODE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
            _CRT_SECURE_NO_WARNINGS
        >
        
        # Debug模式定义
        $<$<CONFIG:Debug>:
            DEBUG
            _DEBUG
            AI_SYSTEM_DEBUG
        >
        
        # Release模式定义
        $<$<CONFIG:Release>:
            NDEBUG
            QT_NO_DEBUG
        >
)

# ================================================
# 📄 资源文件处理
# ================================================

# 创建资源文件（如果不存在）
set(RESOURCE_QRC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc")
if(NOT EXISTS ${RESOURCE_QRC_FILE})
    file(WRITE ${RESOURCE_QRC_FILE}
        "<!DOCTYPE RCC>\n"
        "<RCC version=\"1.0\">\n"
        "  <qresource>\n"
        "    <!-- Icons -->\n"
        "    <file alias=\"icons/app_icon.ico\">icons/app_icon.ico</file>\n"
        "    <file alias=\"icons/ai_icon.png\">icons/ai_icon.png</file>\n"
        "    <file alias=\"icons/server_icon.png\">icons/server_icon.png</file>\n"
        "    \n"
        "    <!-- Styles -->\n"
        "    <file alias=\"styles/cyberpunk.qss\">styles/cyberpunk.qss</file>\n"
        "    <file alias=\"styles/neon_glow.qss\">styles/neon_glow.qss</file>\n"
        "    \n"
        "    <!-- Images -->\n"
        "    <file alias=\"images/splash.png\">images/splash.png</file>\n"
        "    <file alias=\"images/background.png\">images/background.png</file>\n"
        "  </qresource>\n"
        "</RCC>\n"
    )
endif()

# ================================================
# 📦 安装配置
# ================================================
install(TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

# 安装Qt库（Windows）
if(WIN32)
    # 查找windeployqt工具
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)
    
    if(WINDEPLOYQT_EXECUTABLE)
        # 添加部署命令
        install(CODE "
            execute_process(
                COMMAND ${WINDEPLOYQT_EXECUTABLE} --qmldir ${CMAKE_SOURCE_DIR} \$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin/${TARGET_NAME}.exe
                RESULT_VARIABLE result
            )
            if(NOT result EQUAL 0)
                message(WARNING \"windeployqt failed\")
            endif()
        ")
    endif()
endif()

# ================================================
# 🏷️ 目标属性设置
# ================================================
set_target_properties(${TARGET_NAME} PROPERTIES
    # 基本属性
    OUTPUT_NAME "RANOnline_AI_Frontend"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    
    # C++标准
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    
    # Windows特定属性
    WIN32_EXECUTABLE TRUE
    
    # 调试信息
    $<$<CONFIG:Debug>:
        COMPILE_PDB_NAME "${TARGET_NAME}_debug"
        COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    >
    
    # 输出目录
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# ================================================
# 📊 构建信息输出
# ================================================
message(STATUS "")
message(STATUS "🎯 Frontend Manager Configuration:")
message(STATUS "   Target Name: ${TARGET_NAME}")
message(STATUS "   Output Name: RANOnline_AI_Frontend")
message(STATUS "   Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "   Qt Version: ${Qt6_VERSION}")
message(STATUS "   Install Dir: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
