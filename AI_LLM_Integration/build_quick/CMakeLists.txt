cmake_minimum_required(VERSION 3.16)
project(RANOnline_AI_Quick VERSION 4.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Network)

add_executable(RANOnline_AI_Quick WIN32 main_ultimate_portable.cpp)

target_link_libraries(RANOnline_AI_Quick
    Qt6::Core
    Qt6::Widgets  
    Qt6::Gui
    Qt6::Network
)

target_compile_definitions(RANOnline_AI_Quick PRIVATE
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
    UNICODE
    _UNICODE
    VM_COMPATIBLE_BUILD=1
)
