/*
================================================================================
File: tests/benchmark.cpp
Purpose: A dedicated command-line executable for performance benchmarking.
         This program measures the execution time of the AI's core algorithm
         under specific, repeatable conditions.
================================================================================
*/
#include "ai_engine.h"
#include "game_logic.h"
#include <iostream>
#include <chrono>

int main() {
    // Print a header for our CSV output. This makes the results easy to parse.
    std::cout << "TestName,Duration(ms)" << std::endl;

    AIEngine ai_engine;
    GameLogic game_logic;

    // --- Benchmark Scenario 1: Early-Game Move ---
    // The AI has to make its first move. This should be very fast.
    game_logic.resetBoard();
    game_logic.makeMove(0, 0); // Player X makes the first move.

    // Now, time how long the AI takes to respond.
    [cite_start]// We pass the entire 'game_logic' object, as required by ai_engine.h [cite: 2]
    auto start_early = std::chrono::high_resolution_clock::now();
    ai_engine.getBestMove(game_logic);
    auto end_early = std::chrono::high_resolution_clock::now();
    auto duration_early = std::chrono::duration_cast<std::chrono::milliseconds>(end_early - start_early);
    std::cout << "Early-Game-Scenario," << duration_early.count() << std::endl;


    // --- Benchmark Scenario 2: Mid-Game Blocking Move ---
    // Set up a scenario where the AI must make a defensive move.
    game_logic.resetBoard();
    game_logic.makeMove(0, 0); // X
    game_logic.makeMove(2, 2); // O (AI)
    game_logic.makeMove(0, 1); // X (Player is threatening a win on the top row)
    
    // Now, time how long the AI takes to find the blocking move (0,2).
    // Again, we pass the entire 'game_logic' object.
    auto start_mid = std::chrono::high_resolution_clock::now();
    ai_engine.getBestMove(game_logic);
    auto end_mid = std::chrono::high_resolution_clock::now();
    auto duration_mid = std::chrono::duration_cast<std::chrono::milliseconds>(end_mid - start_mid);
    std::cout << "Mid-Game-Blocking-Scenario," << duration_mid.count() << std::endl;

    return 0;
}
