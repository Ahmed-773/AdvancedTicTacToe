# [cite_start]Final, corrected CMakeLists.txt for the complete project [cite: 1]
[cite_start]cmake_minimum_required(VERSION 3.16) [cite: 2]

[cite_start]project(the_final_game VERSION 0.1 LANGUAGES CXX) [cite: 2]

# Enable Qt's automatic tools
[cite_start]set(CMAKE_AUTOUIC ON) [cite: 2]
[cite_start]set(CMAKE_AUTOMOC ON) [cite: 2]
[cite_start]set(CMAKE_AUTORCC ON) [cite: 2]

# Set the C++ standard
[cite_start]set(CMAKE_CXX_STANDARD 17) [cite: 2]
[cite_start]set(CMAKE_CXX_STANDARD_REQUIRED ON) [cite: 2]

# Find required packages, including Qt Test
[cite_start]find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Sql Test) [cite: 2]
[cite_start]find_package(OpenSSL REQUIRED) [cite: 2]

# --- Main Application Executable ---
[cite_start]add_executable(the_final_game [cite: 2]
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gui_interface.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game_logic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ai_engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/user_auth.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/database_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game_history.cpp
)

[cite_start]target_include_directories(the_final_game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include) [cite: 2]
[cite_start]target_link_libraries(the_final_game PRIVATE Qt6::Widgets Qt6::Sql OpenSSL::SSL) [cite: 2]

# --- Test Suite Executable ---
[cite_start]add_executable(run_tests [cite: 2]
    # Use absolute paths for robustness, consistent with the main target
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_suite.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game_logic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ai_engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/user_auth.cpp
    # ADDED: The test suite needs the database manager to link correctly
    ${CMAKE_CURRENT_SOURCE_DIR}/src/database_manager.cpp
)

[cite_start]target_include_directories(run_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include) [cite: 2]
[cite_start]target_link_libraries(run_tests PRIVATE Qt6::Test OpenSSL::SSL) [cite: 2]

# Finalize Qt Executable
[cite_start]set_target_properties(the_final_game PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE) [cite: 2]
[cite_start]qt_finalize_executable(the_final_game) [cite: 2]