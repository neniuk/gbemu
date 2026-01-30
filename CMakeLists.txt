cmake_minimum_required(VERSION 3.28)
project(gbemu VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add external dependencies
find_package(Eigen3 CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

# Add the executable
add_executable(gbemu src/main.cpp src/memory.cpp src/registers.cpp src/stack.cpp src/screen.cpp src/idu.cpp src/alu.cpp src/gb.cpp)

# Link libraries
target_link_libraries(gbemu PRIVATE Eigen3::Eigen SDL2::SDL2 SDL2::SDL2main)

# Include directories
target_include_directories(gbemu PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)

# Set the working directory for the debugger
set_target_properties(gbemu PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
