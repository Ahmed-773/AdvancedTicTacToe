# Recommended replacement for your CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

project(the_final_game VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find required packages ---
# Single, clean call to find Qt6 and its components
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Sql)
find_package(OpenSSL REQUIRED)

# --- Google Test Integration ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# --- Main Application Executable ---
# We use the standard add_executable and list all sources directly.
# Including the header with Q_OBJECT (gui_interface.h) here is the key
# to making AUTOMOC work reliably.
add_executable(the_final_game
    main.cpp
    gui_interface.h
    gui-interface-impl.cpp
    game-logic-impl.cpp
    ai-engine-impl.cpp
    user-auth-impl.cpp
    database-manager-impl.cpp
    game-history-impl.cpp
)

target_link_libraries(the_final_game
    PRIVATE
    Qt6::Widgets
    Qt6::Sql
    OpenSSL::SSL
)

# --- Test Suite Executable ---
add_executable(run_tests
    tests/test_suite.cpp
    game-logic-impl.cpp
    ai-engine-impl.cpp
    user-auth-impl.cpp
)

target_link_libraries(run_tests PRIVATE gtest_main OpenSSL::SSL)

include(GoogleTest)
gtest_discover_tests(run_tests)

# --- Finalize Qt Executable ---
set_target_properties(the_final_game PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE)

qt_finalize_executable(the_final_game)