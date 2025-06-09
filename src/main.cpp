// main.cpp
#include "gui_interface.h"
#include <QApplication>
#include <iostream>
#include <QStandardPaths>
#include <QDir>

// Function to run the application in test mode (for CI/CD pipeline)
bool runTests() {
    std::cout << "Running tests..." << std::endl;
    
    // Test game logic
    GameLogic game;
    if (game.getCurrentPlayer() != Player::X) {
        std::cerr << "Test failed: Initial player should be X" << std::endl;
        return false;
    }
    
    // Test making moves
    if (!game.makeMove(0, 0)) {
        std::cerr << "Test failed: Failed to make a valid move" << std::endl;
        return false;
    }
    
    if (game.getCurrentPlayer() != Player::O) {
        std::cerr << "Test failed: Player should switch to O after X's move" << std::endl;
        return false;
    }
    
    if (game.getCell(0, 0) != Player::X) {
        std::cerr << "Test failed: Cell (0,0) should be X" << std::endl;
        return false;
    }
    
    // Test AI engine
    AIEngine ai;
    Move aiMove = ai.getBestMove(game);
    if (aiMove.row < 0 || aiMove.row > 2 || aiMove.col < 0 || aiMove.col > 2) {
        std::cerr << "Test failed: AI returned invalid move" << std::endl;
        return false;
    }
    
    std::cout << "All tests passed!" << std::endl;
    return true;
}