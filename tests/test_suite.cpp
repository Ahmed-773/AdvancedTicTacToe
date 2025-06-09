/*
================================================================================
File: tests/test_suite.cpp
Description: Comprehensive test suite using the Google Test framework as
             required by the project specifications.
================================================================================
*/
#include "gtest/gtest.h"

// Include headers for the classes we need to test
// The paths assume tests are run from the build directory
#include "../game_logic.h"
#include "../ai_engine.h"
#include "../user_auth.h"

// --- Test Fixture for GameLogic ---
class GameLogicTest : public ::testing::Test {
protected:
    GameLogic game;
};

// --- GameLogic Tests ---
TEST_F(GameLogicTest, InitialState) {
    ASSERT_EQ(game.getCurrentPlayer(), Player::X);
    ASSERT_EQ(game.checkGameResult(), GameResult::IN_PROGRESS);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            EXPECT_EQ(game.getCell(i, j), Player::NONE);
        }
    }
}

TEST_F(GameLogicTest, ValidMove) {
    ASSERT_TRUE(game.makeMove(1, 1));
    EXPECT_EQ(game.getCell(1, 1), Player::X);
    EXPECT_EQ(game.getCurrentPlayer(), Player::O);
}

TEST_F(GameLogicTest, WinConditionHorizontal) {
    game.makeMove(0, 0); // X
    game.makeMove(1, 0); // O
    game.makeMove(0, 1); // X
    game.makeMove(1, 1); // O
    ASSERT_TRUE(game.makeMove(0, 2)); // X wins
    EXPECT_EQ(game.checkGameResult(), GameResult::X_WINS);
}

// --- Test Fixture for AIEngine ---
class AIEngineTest : public ::testing::Test {
protected:
    GameLogic game;
    AIEngine aiEngine{3}; // Use hard difficulty for predictable results
};

// --- AIEngine Tests ---
TEST_F(AIEngineTest, BlocksOpponentWin) {
    game.makeMove(0, 0); // X
    game.makeMove(2, 2); // O
    game.makeMove(0, 1); // X
    game.makeMove(1, 1); // O
    // X is about to win at (0, 2). AI is O and must block.
    Move aiMove = aiEngine.getBestMove(game);
    ASSERT_EQ(aiMove.row, 0);
    ASSERT_EQ(aiMove.col, 2);
}

// --- Test Fixture for UserAuth ---
class UserAuthTest : public ::testing::Test {
protected:
    UserAuth auth;
};

// --- UserAuth Tests ---
TEST_F(UserAuthTest, RegisterAndLogin) {
    ASSERT_TRUE(auth.registerUser("testuser", "password123"));
    ASSERT_TRUE(auth.isLoggedIn());
    auth.logoutUser();
    ASSERT_FALSE(auth.isLoggedIn());
    ASSERT_TRUE(auth.loginUser("testuser", "password123"));
    ASSERT_TRUE(auth.isLoggedIn());
}

TEST_F(UserAuthTest, PasswordVerification) {
    std::string hash = auth.hashPassword("securepass");
    ASSERT_TRUE(auth.verifyPassword("securepass", hash));
    ASSERT_FALSE(auth.verifyPassword("incorrect", hash));
}