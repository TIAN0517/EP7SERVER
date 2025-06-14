# ================================================
# 📡 RANOnline Communication Protocol Module
# 通信协议模块 - 二进制协议 + 多传输层支持
# ================================================

# 协议库
add_library(communication_protocol STATIC
    Protocol.cpp
    Protocol.h
)

# 链接依赖
target_link_libraries(communication_protocol 
    Qt6::Core
    Qt6::Network
    Threads::Threads
)

# 包含目录
target_include_directories(communication_protocol PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 编译器特定设置
if(WIN32)
    target_compile_definitions(communication_protocol PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    # Windows Socket库
    target_link_libraries(communication_protocol ws2_32)
endif()

# 安装设置
install(TARGETS communication_protocol
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES Protocol.h
    DESTINATION include/communication_protocol
)

# 调试信息
message(STATUS "✅ Communication Protocol module configured")
