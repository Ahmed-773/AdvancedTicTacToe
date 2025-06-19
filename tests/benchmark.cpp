/*
================================================================================
File: tests/benchmark.cpp
Purpose: A dedicated command-line executable for performance benchmarking.
         This corrected version measures time in microseconds to get a more
         precise and meaningful result for a very fast algorithm.
================================================================================
*/
#include "ai_engine.h"
#include "game_logic.h"
#include <iostream>
#include <chrono>

int main() {
    // --- Variable Declarations ---
    AIEngine ai_engine;
    GameLogic game_logic;
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    
    // CHANGE #1: We will now store the duration in microseconds.
    std::chrono::microseconds duration;

    // --- Execution ---
    // CHANGE #2: Update the header to reflect the new unit (us for microseconds).
    std::cout << "TestName,Duration(us)" << std::endl;

    // --- Benchmark Scenario 1: Early-Game Move ---
    game_logic.resetBoard();
    game_logic.makeMove(0, 0); // Player X makes the first move.

    start_time = std::chrono::high_resolution_clock::now();
    ai_engine.getBestMove(game_logic);
    end_time = std::chrono::high_resolution_clock::now();
    
    // CHANGE #3: Cast the result to microseconds instead of milliseconds.
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Early-Game-Scenario," << duration.count() << std::endl;


    // --- Benchmark Scenario 2: Mid-Game Blocking Move ---
    game_logic.resetBoard();
    game_logic.makeMove(0, 0); // X
    game_logic.makeMove(2, 2); // O (AI)
    game_logic.makeMove(0, 1); // X (Player is threatening a win on the top row)
    
    start_time = std::chrono::high_resolution_clock::now();
    ai_engine.getBestMove(game_logic);
    end_time = std::chrono::high_resolution_clock::now();
    
    // Also cast this result to microseconds.
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Mid-Game-Blocking-Scenario," << duration.count() << std::endl;

    return 0;
}