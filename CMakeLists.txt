cmake_minimum_required(VERSION 3.28)
project(gbemu VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(GBEMU_ENABLE_WARNINGS "Enable stricter compiler warnings" ON)

# Add external dependencies
find_package(SDL2 CONFIG REQUIRED)

# Add the executable
add_executable(gbemu src/main.cpp src/memory.cpp src/registers.cpp src/stack.cpp src/screen.cpp src/idu.cpp src/alu.cpp src/bmi.cpp src/ppu.cpp src/timer.cpp src/joypad.cpp src/cpu.cpp src/cpu_cb.cpp src/gb.cpp)

# Link libraries
target_link_libraries(gbemu PRIVATE SDL2::SDL2 SDL2::SDL2main)

# Include directories
target_include_directories(gbemu PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)

if(GBEMU_ENABLE_WARNINGS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(gbemu PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    elseif(MSVC)
        target_compile_options(gbemu PRIVATE /W4 /permissive-)
    endif()
endif()

# Enable compile-time debug flag in Debug builds
target_compile_definitions(gbemu PRIVATE $<$<CONFIG:Debug>:GBEMU_DEBUG=1>)

# Set the working directory for the debugger
set_target_properties(gbemu PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
