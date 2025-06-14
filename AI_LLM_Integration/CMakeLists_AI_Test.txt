cmake_minimum_required(VERSION 3.16)

# ========================================================================
# 項目配置 - AI決策核心系統測試版
# ========================================================================

project(RANOnline_AI_Decision_Core_Test
    VERSION 4.0.0
    LANGUAGES CXX
    DESCRIPTION "RANOnline AI決策核心系統測試"
)

# ========================================================================
# 基本設置
# ========================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 設置調試信息
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")

# ========================================================================
# Qt6 配置
# ========================================================================

find_package(Qt6 REQUIRED COMPONENTS 
    Core
    Widgets
    Network
    WebSockets
    Concurrent
)

# ========================================================================
# 包含目录
# ========================================================================

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ========================================================================
# AI決策核心系統源文件
# ========================================================================

set(AI_CORE_SOURCES
    AIDecisionCore.cpp
    AIBehaviorSystems.cpp
    AIPlayerBrain.cpp
    AISystemIntegration.cpp
    AIDecisionEngine.cpp
    AIPlayerGenerator.cpp
    AIManagementWidget.cpp
    GameAIProtocol.cpp
    GameWebSocketServer.cpp
    GameWebSocketClient.cpp
)

# AI決策核心系統頭文件  
set(AI_CORE_HEADERS
    AIDecisionCore.h
    AIPlayerBrain.h
    AISystemIntegration.h
    AIDecisionEngine.h
    AIPlayerGenerator.h
    AIManagementWidget.h
    GameAIProtocol.h
    GameWebSocketServer.h
    GameWebSocketClient.h
)

# ========================================================================
# 測試程序源文件
# ========================================================================

set(TEST_SOURCES
    AIDecisionCoreTest.cpp
)

# ========================================================================
# 檢查源文件是否存在
# ========================================================================

message(STATUS "========================================")
message(STATUS "RANOnline AI決策核心系統測試")
message(STATUS "========================================")

foreach(source ${AI_CORE_SOURCES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
        message(STATUS "✅ Found: ${source}")
    else()
        message(WARNING "❌ Missing: ${source}")
    endif()
endforeach()

foreach(header ${AI_CORE_HEADERS})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${header}")
        message(STATUS "✅ Found: ${header}")
    else()
        message(WARNING "❌ Missing: ${header}")
    endif()
endforeach()

foreach(test ${TEST_SOURCES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${test}")
        message(STATUS "✅ Found: ${test}")
    else()
        message(WARNING "❌ Missing: ${test}")
    endif()
endforeach()

# ========================================================================
# 創建AI決策核心測試執行檔
# ========================================================================

add_executable(ai_decision_core_test
    ${AI_CORE_SOURCES}
    ${TEST_SOURCES}
)

# 鏈接Qt6庫
target_link_libraries(ai_decision_core_test
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::WebSockets
    Qt6::Concurrent
)

# 設置編譯選項
target_compile_options(ai_decision_core_test PRIVATE
    -Wall
    -Wextra
    -DDEBUG
    -DTEST_MODE
)

# ========================================================================
# 創建集成測試執行檔
# ========================================================================

add_executable(ai_integration_test
    ${AI_CORE_SOURCES}
    AIIntegrationTest.cpp
)

target_link_libraries(ai_integration_test
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::WebSockets
    Qt6::Concurrent
)

target_compile_options(ai_integration_test PRIVATE
    -Wall
    -Wextra
    -DDEBUG
    -DTEST_MODE
)

# ========================================================================
# 配置文件複製
# ========================================================================

# 複製配置文件到構建目錄
configure_file(ai_decision_config.json ai_decision_config.json COPYONLY)
configure_file(behavior_tree_config.json behavior_tree_config.json COPYONLY)
configure_file(qlearning_config.json qlearning_config.json COPYONLY)

# ========================================================================
# 構建信息
# ========================================================================

message(STATUS "========================================")
message(STATUS "Build Configuration:")
message(STATUS "  Project: ${PROJECT_NAME}")
message(STATUS "  Version: ${PROJECT_VERSION}")  
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Qt6 Core: ${Qt6Core_VERSION}")
message(STATUS "========================================")
message(STATUS "Available targets:")
message(STATUS "  - ai_decision_core_test (核心決策系統測試)")
message(STATUS "  - ai_integration_test (系統集成測試)")
message(STATUS "========================================")
