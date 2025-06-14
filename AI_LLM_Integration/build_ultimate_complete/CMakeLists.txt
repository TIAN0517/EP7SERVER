cmake_minimum_required(VERSION 3.22)

project(RANOnline_AI_Ultimate_Portable 
    VERSION 4.0.0 
    DESCRIPTION "RANOnline AI LLM Integration - Ultimate Portable Edition"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE TRUE)
    set(CMAKE_SYSTEM_VERSION 10.0)
endif()

# 查找Qt6
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Gui
    Network
)

# 啟用Qt6的MOC/RCC/UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 編譯器設定 - VM兼容性優化
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
    # VM兼容性標志
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_OPENGL_ES_2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_OPENGL_SOFTWARE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVM_ULTIMATE_PORTABLE")
    # 靜態鏈接運行時
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

# Windows資源文件
if(WIN32)
    set(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/ultimate_portable_resources.rc")
    set(ICO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../Resources/jy1.ico")
    
    if(NOT EXISTS "${ICO_FILE}")
        set(ICO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../jy1.ico")
    endif()
    
    if(NOT EXISTS "${ICO_FILE}")
        set(ICO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../icogui/jy1.ico")
    endif()
    
    if(EXISTS "${ICO_FILE}")
        file(WRITE "${RC_FILE}" 
"IDI_ICON1 ICON \"${ICO_FILE}\"
VS_VERSION_INFO VERSIONINFO
FILEVERSION 4,0,0,0
PRODUCTVERSION 4,0,0,0
FILEFLAGSMASK 0x17L
FILEFLAGS 0x0L
FILEOS 0x4L
FILETYPE 0x1L
FILESUBTYPE 0x0L
BEGIN
    BLOCK \"StringFileInfo\"
    BEGIN
        BLOCK \"040904b0\"
        BEGIN
            VALUE \"CompanyName\", \"Jy技術團隊\"
            VALUE \"FileDescription\", \"RANOnline AI LLM Integration - Ultimate Portable Edition\"
            VALUE \"FileVersion\", \"4.0.0.0\"
            VALUE \"InternalName\", \"RANOnline_AI_Ultimate_Portable\"
            VALUE \"LegalCopyright\", \"Copyright (C) 2025 Jy技術團隊\"
            VALUE \"OriginalFilename\", \"RANOnline_AI_Ultimate_Portable.exe\"
            VALUE \"ProductName\", \"RANOnline AI LLM Integration Ultimate Portable\"
            VALUE \"ProductVersion\", \"4.0.0.0\"
        END
    END
    BLOCK \"VarFileInfo\"
    BEGIN
        VALUE \"Translation\", 0x409, 1200
    END
END
")
        set(RESOURCE_FILES ${RC_FILE})
        message(STATUS "Windows resource file created: ${RC_FILE}")
    else()
        message(WARNING "Icon file not found, building without icon")
        set(RESOURCE_FILES "")
    endif()
endif()

# 創建可執行文件 - 只使用main_ultimate_portable.cpp
add_executable(RANOnline_AI_Ultimate_Portable
    ../main_ultimate_portable.cpp
    ${RESOURCE_FILES}
)

# 鏈接Qt6庫
target_link_libraries(RANOnline_AI_Ultimate_Portable
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Network
)

# 包含目錄
target_include_directories(RANOnline_AI_Ultimate_Portable PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}
)

# 編譯定義 - VM兼容性
target_compile_definitions(RANOnline_AI_Ultimate_Portable PRIVATE
    QT_NO_OPENGL_ES_2
    QT_OPENGL_SOFTWARE
    VM_ULTIMATE_PORTABLE
    VM_COMPATIBLE_BUILD
    ULTIMATE_PORTABLE_EDITION
)

# 設置輸出名稱
set_target_properties(RANOnline_AI_Ultimate_Portable PROPERTIES
    OUTPUT_NAME "RANOnline_AI_Ultimate_Portable"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

message(STATUS "========================================")
message(STATUS "RANOnline AI Ultimate Portable Edition")
message(STATUS "========================================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Source File: ../main_ultimate_portable.cpp")
message(STATUS "Output: RANOnline_AI_Ultimate_Portable.exe")
message(STATUS "========================================")
