# Final
cmake_minimum_required(VERSION 3.16)

project(the_final_game VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find required packages ---
# FIXED: Added "Test" to the list of required Qt components
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Sql Test)

# --- Google Test Integration (REMOVED) ---
# The entire FetchContent block for googletest has been removed.

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
    tests/test_suite.cpp
    src/game_logic.cpp
    src/ai_engine.cpp
    src/user_auth.cpp
)

target_include_directories(run_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link libraries to the main target
target_link_libraries(the_final_game
    PRIVATE
    Qt6::Widgets
    Qt6::Sql
    OpenSSL::SSL
)

# --- Test Suite Executable ---
add_executable(run_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_suite.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game_logic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ai_engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/user_auth.cpp
)

# Tell the test target where to find header files
target_include_directories(run_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link libraries to the test target
target_link_libraries(run_tests PRIVATE gtest_main OpenSSL::SSL)

include(GoogleTest)
gtest_discover_tests(run_tests)

# --- Finalize Qt Executable ---
set_target_properties(the_final_game PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE)
qt_finalize_executable(the_final_game)