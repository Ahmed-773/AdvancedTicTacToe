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
#include <vector>

// Function to run a benchmark on a given board state
void run_benchmark(const std::string& test_name, const std::vector<std::vector<char>>& board) {
    AIEngine ai;
    GameState state;
    state.board = board;
    state.currentPlayer = 'O'; // Assume AI is 'O'
    state.result = GameResult::IN_PROGRESS;

    // Start the timer
    auto start_time = std::chrono::high_resolution_clock::now();

    // Run the AI's core logic
    Move bestMove = ai.findBestMove(state.board);

    // Stop the timer
    auto end_time = std::chrono::high_resolution_clock::now();
    
    // Calculate the duration in milliseconds
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Print the results in a simple CSV-like format
    std::cout << test_name << "," << duration.count() << std::endl;
}

int main() {
    // Print a header for our CSV output
    std::cout << "TestName,Duration(ms)" << std::endl;

    // --- Define Benchmark Scenarios ---

    // Scenario 1: A mid-game state with several possible moves.
    // This represents a typical, moderately complex calculation.
    std::vector<std::vector<char>> mid_game_board = {
        {'X', ' ', ' '},
        {' ', 'O', ' '},
        {'X', ' ', ' '}
    };
    run_benchmark("Mid-Game-Scenario", mid_game_board);

    // Scenario 2: An early-game state.
    // This should be very fast.
    std::vector<std::vector<char>> early_game_board = {
        {'X', ' ', ' '},
        {' ', ' ', ' '},
        {' ', ' ', ' '}
    };
    run_benchmark("Early-Game-Scenario", early_game_board);

    // Scenario 3: A more complex late-game state.
    // This will likely be the longest calculation.
     std::vector<std::vector<char>> late_game_board = {
        {'X', 'O', 'X'},
        {' ', 'O', ' '},
        {' ', 'X', ' '}
    };
    run_benchmark("Late-Game-Scenario", late_game_board);

    return 0;
}