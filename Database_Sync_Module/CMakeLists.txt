# ================================================
# 🗄️ RANOnline Database Sync Module
# 数据库同步模块 - SQL Server 2022 + ODBC/ADO
# ================================================

# 数据库同步库
add_library(database_sync_module STATIC
    DatabaseManager.cpp
    DatabaseManager.h
)

# 链接依赖
target_link_libraries(database_sync_module 
    Qt6::Core
    Qt6::Sql
    Qt6::Concurrent
    communication_protocol
    Threads::Threads
)

# Windows特定依赖
if(WIN32)
    target_link_libraries(database_sync_module
        ${ODBC_LIBRARY}
        ole32
        oleaut32
    )
    target_compile_definitions(database_sync_module PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _WIN32_DCOM
    )
endif()

# 包含目录
target_include_directories(database_sync_module PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 编译器特定设置
target_compile_features(database_sync_module PRIVATE cxx_std_20)

# 预编译头（优化编译速度）
if(MSVC)
    target_precompile_headers(database_sync_module PRIVATE
        <QtCore/QObject>
        <QtSql/QSqlDatabase>
        <QtSql/QSqlQuery>
        <memory>
        <thread>
        <mutex>
    )
endif()

# 安装设置
install(TARGETS database_sync_module
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES DatabaseManager.h
    DESTINATION include/database_sync_module
)

# 调试信息
message(STATUS "✅ Database Sync Module configured")
