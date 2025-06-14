# ================================================
# 🤖 RANOnline AI Backend Engine
# AI后端推理引擎 - 多线程 + 负载均衡
# ================================================

# AI引擎库
add_library(ai_backend_engine_lib STATIC
    AIEngine.cpp
    AIEngine.h
    AIPlayerInstance.cpp
)

# AI引擎可执行程序
add_executable(ai_backend_engine
    main.cpp
)

# 库依赖链接
target_link_libraries(ai_backend_engine_lib
    Qt6::Core
    Qt6::Concurrent
    communication_protocol
    database_sync_module
    Threads::Threads
)

# 可执行程序依赖链接
target_link_libraries(ai_backend_engine
    ai_backend_engine_lib
    Qt6::Core
    Qt6::Concurrent
    communication_protocol
    database_sync_module
    Threads::Threads
)

# Windows特定设置
if(WIN32)
    target_compile_definitions(ai_backend_engine_lib PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    target_compile_definitions(ai_backend_engine PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    
    # 设置控制台程序
    set_target_properties(ai_backend_engine PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

# 包含目录
target_include_directories(ai_backend_engine_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(ai_backend_engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 编译器特性
target_compile_features(ai_backend_engine_lib PRIVATE cxx_std_20)
target_compile_features(ai_backend_engine PRIVATE cxx_std_20)

# 预编译头（优化编译速度）
if(MSVC)
    target_precompile_headers(ai_backend_engine_lib PRIVATE
        <QtCore/QObject>
        <QtCore/QThread>
        <QtCore/QTimer>
        <memory>
        <thread>
        <mutex>
        <atomic>
        <vector>
        <queue>
    )
endif()

# Release模式优化
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_options(ai_backend_engine_lib PRIVATE /O2 /Ob2)
        target_compile_options(ai_backend_engine PRIVATE /O2 /Ob2)
    endif()
endif()

# 安装设置
install(TARGETS ai_backend_engine ai_backend_engine_lib
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES AIEngine.h
    DESTINATION include/ai_backend_engine
)

# 调试信息
message(STATUS "✅ AI Backend Engine configured")
