/*
================================================================================
File: tests/benchmark.cpp
Purpose: A dedicated command-line executable for performance benchmarking.
         This version fixes the C2275 error by declaring all variables at the
         top of the function block, complying with older C++ standard rules.
================================================================================
*/
#include "ai_engine.h"
#include "game_logic.h"
#include <iostream>
#include <chrono>

int main() {
    // --- Variable Declarations ---
    // Declare all variables at the top of the block to fix C2275 error.
    AIEngine ai_engine;
    GameLogic game_logic;
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    std::chrono::milliseconds duration;

    // --- Execution ---
    // Print a header for our CSV output.
    std::cout << "TestName,Duration(ms)" << std::endl;

    // --- Benchmark Scenario 1: Early-Game Move ---
    game_logic.resetBoard();
    game_logic.makeMove(0, 0); // Player X makes the first move.

    start_time = std::chrono::high_resolution_clock::now();
    ai_engine.getBestMove(game_logic);
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Early-Game-Scenario," << duration.count() << std::endl;


    // --- Benchmark Scenario 2: Mid-Game Blocking Move ---
    game_logic.resetBoard();
    game_logic.makeMove(0, 0); // X
    game_logic.makeMove(2, 2); // O (AI)
    game_logic.makeMove(0, 1); // X (Player is threatening a win on the top row)
    
    start_time = std::chrono::high_resolution_clock::now();
    ai_engine.getBestMove(game_logic);
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Mid-Game-Blocking-Scenario," << duration.count() << std::endl;

    return 0;
}
