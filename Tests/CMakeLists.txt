# ================================================
# 🧪 RANOnline Testing Framework
# 单元测试和集成测试
# ================================================

# 查找Google Test
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(STATUS "Google Test not found, downloading...")
    
    # 使用FetchContent下载Google Test
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50f33f9bf776f955a89db98a3a25cd.zip
    )
    
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# 启用测试
enable_testing()

# 测试源文件
set(TEST_SOURCES
    test_communication_protocol.cpp
    test_database_manager.cpp
    test_ai_engine.cpp
    test_load_balancer.cpp
    test_performance_monitor.cpp
)

# 创建测试可执行文件
add_executable(ranonline_tests
    ${TEST_SOURCES}
)

# 链接库
target_link_libraries(ranonline_tests
    PRIVATE
        gtest_main
        gtest
        communication_protocol
        database_sync_module
        ai_backend_engine_lib
        Qt6::Core
        Qt6::Test
        Threads::Threads
)

# 包含目录
target_include_directories(ranonline_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Communication_Protocol
        ${CMAKE_CURRENT_SOURCE_DIR}/../Database_Sync_Module
        ${CMAKE_CURRENT_SOURCE_DIR}/../AI_Backend_Engine
        ${CMAKE_CURRENT_SOURCE_DIR}/../AI_Frontend_Manager
)

# 添加测试
add_test(NAME RANOnlineTests COMMAND ranonline_tests)

# 编译定义
target_compile_definitions(ranonline_tests
    PRIVATE
        TESTING_MODE
        QT_TESTCASE_BUILDDIR="${CMAKE_CURRENT_BINARY_DIR}"
)

# 调试信息
message(STATUS "✅ Testing framework configured")
