cmake_minimum_required(VERSION 3.16)

project(the_final_game VERSION 0.1 LANGUAGES CXX)

# Enable Qt's automatic tools
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages, including Qt Test
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Sql Test)
find_package(OpenSSL REQUIRED)

# --- Main Application Executable ---
add_executable(the_final_game
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gui_interface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game_logic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ai_engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/user_auth.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/database_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game_history.cpp
)

target_include_directories(the_final_game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(the_final_game PRIVATE Qt6::Widgets Qt6::Sql OpenSSL::SSL)

# --- Test Suite Executable ---
add_executable(run_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_suite.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game_logic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ai_engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/user_auth.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/database_manager.cpp
)

target_include_directories(run_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(run_tests PRIVATE Qt6::Test OpenSSL::SSL)

# --- Benchmark Executable ---
# This defines a new, non-GUI executable for performance measurement.
add_executable(benchmark
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/benchmark.cpp
    # It needs the same backend logic as the tests
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game_logic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ai_engine.cpp
)
target_include_directories(benchmark PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Finalize Qt Executable
set_target_properties(the_final_game PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE)
qt_finalize_executable(the_final_game)