/*
================================================================================
File: tests/test_suite.cpp
Description: A comprehensive test suite using the Qt Test framework. This suite
             covers core game logic, AI strategy, and user authentication to
             ensure the reliability and correctness of the application's backend.
================================================================================
*/
#include <QtTest>
#include <QObject>

// Include the headers for the classes we want to test
#include "game_logic.h"
#include "ai_engine.h"
#include "user_auth.h"

// In Qt Test, a test suite is a class that inherits from QObject.
class TestSuite : public QObject
{
    Q_OBJECT // This macro is required for Qt's meta-object system.

private slots:
    // This function is called automatically before each test function.
    void init();

    // --- GameLogic Tests ---
    void testInitialState();
    void testValidMove();
    void testInvalidMove_OccupiedCell();
    void testInvalidMove_OutOfBounds();
    void testWinCondition_Horizontal();
    void testWinCondition_Vertical();
    void testWinCondition_Diagonal();
    void testDrawCondition();

    // --- AIEngine Tests ---
    void testAI_TakesWinningMove();
    void testAI_BlocksOpponentWin();

    // --- UserAuth Tests ---
    void testUser_SuccessfulRegistrationAndLogin();
    void testUser_DuplicateRegistrationFails();
    void testUser_FailedLogin();
    void testUser_PasswordHashing();

private:
    // Member variables to hold instances of the classes under test.
    // They are re-initialized for each test by the init() function.
    GameLogic game;
    AIEngine aiEngine;
    UserAuth userAuth;
};

// This function is called before each test case, ensuring a clean state.
void TestSuite::init()
{
    game = GameLogic();
    aiEngine = AIEngine();
    userAuth = UserAuth();
}

// =============================================================================
// --- GameLogic Test Implementations ---
// =============================================================================

void TestSuite::testInitialState()
{
    // QCOMPARE is the Qt equivalent of ASSERT_EQ. It checks if two values are equal.
    QCOMPARE(game.getCurrentPlayer(), Player::X);
    QCOMPARE(game.checkGameResult(), GameResult::IN_PROGRESS);
}

void TestSuite::testValidMove()
{
    // QVERIFY is the Qt equivalent of ASSERT_TRUE. It checks if a condition is true.
    QVERIFY(game.makeMove(1, 1)); // Make a move at the center
    QCOMPARE(game.getCurrentPlayer(), Player::O);
    QCOMPARE(game.getCell(1, 1), Player::X);
}

void TestSuite::testInvalidMove_OccupiedCell()
{
    game.makeMove(1, 1); // X plays at (1, 1)
    // QVERIFY2 takes a boolean and a message to display on failure.
    QVERIFY2(!game.makeMove(1, 1), "Should not be able to play on an occupied cell.");
    QCOMPARE(game.getCurrentPlayer(), Player::O); // Player should not have changed
}

void TestSuite::testInvalidMove_OutOfBounds()
{
    QVERIFY2(!game.makeMove(3, 3), "Should not be able to play outside the board.");
}

void TestSuite::testWinCondition_Horizontal()
{
    game.makeMove(0, 0); // X
    game.makeMove(1, 0); // O
    game.makeMove(0, 1); // X
    game.makeMove(1, 1); // O
    game.makeMove(0, 2); // X wins on the top row
    
    QCOMPARE(game.checkGameResult(), GameResult::X_WINS);
}

void TestSuite::testWinCondition_Vertical()
{
    game.makeMove(0, 0); // X
    game.makeMove(0, 1); // O
    game.makeMove(1, 0); // X
    game.makeMove(1, 1); // O
    game.makeMove(2, 0); // X wins on the left column
    
    QCOMPARE(game.checkGameResult(), GameResult::X_WINS);
}

void TestSuite::testWinCondition_Diagonal()
{
    game.makeMove(0, 0); // X
    game.makeMove(1, 0); // O
    game.makeMove(1, 1); // X
    game.makeMove(1, 2); // O
    game.makeMove(2, 2); // X wins on the main diagonal
    
    QCOMPARE(game.checkGameResult(), GameResult::X_WINS);
}

void TestSuite::testDrawCondition()
{
    game.makeMove(0, 0); game.makeMove(1, 1);
    game.makeMove(0, 1); game.makeMove(0, 2);
    game.makeMove(2, 0); game.makeMove(1, 0);
    game.makeMove(1, 2); game.makeMove(2, 2);
    game.makeMove(2, 1);
    
    QVERIFY(game.isBoardFull());
    QCOMPARE(game.checkGameResult(), GameResult::DRAW);
}

// =============================================================================
// --- AIEngine Test Implementations ---
// =============================================================================

void TestSuite::testAI_TakesWinningMove()
{
    // Board state:
    // O | X | X
    // _ | O | _
    // _ | _ | _
    // AI is O. It should play at (1, 2) to win.
    game.makeMove(0, 1); // X
    game.makeMove(1, 1); // O
    game.makeMove(0, 2); // X
    game.makeMove(0, 0); // O
    game.makeMove(2, 0); // X... Now it's O's (AI's) turn to win.
    
    // Switch sides so AI is player O
    // (A bit hacky for a test, but effective)
    GameLogic testState = game;
    if (testState.getCurrentPlayer() == Player::X) testState.makeMove(2, 1); // Make a dummy move for X

    // Now AI is O and has a winning move at (2, 2)
    // Board state:
    // O | X | X
    // _ | O | _
    // X | X | _  <-- AI (O) should play at (2,2) to win
    game.resetBoard();
    game.makeMove(0,1); game.makeMove(0,0);
    game.makeMove(0,2); game.makeMove(1,1);
    game.makeMove(2,0); game.makeMove(1,0);
    game.makeMove(2,1); // X
    // AI's turn (O). It must play at (2,2) to win
    Move bestMove = aiEngine.getBestMove(game);
    QCOMPARE(bestMove.row, 2);
    QCOMPARE(bestMove.col, 2);
}

void TestSuite::testAI_BlocksOpponentWin()
{
    // Board state:
    // X | X | _
    // _ | O | _
    // _ | _ | _
    // AI is O. It must play at (0, 2) to block X's win.
    game.makeMove(0, 0); // X
    game.makeMove(1, 1); // O
    game.makeMove(0, 1); // X
    
    Move bestMove = aiEngine.getBestMove(game);
    QCOMPARE(bestMove.row, 0);
    QCOMPARE(bestMove.col, 2);
}

// =============================================================================
// --- UserAuth Test Implementations ---
// =============================================================================

void TestSuite::testUser_SuccessfulRegistrationAndLogin()
{
    QVERIFY(userAuth.registerUser("testuser", "password123"));
    QVERIFY(userAuth.isLoggedIn());
    
    userAuth.logoutUser();
    QVERIFY(!userAuth.isLoggedIn());
    
    QVERIFY(userAuth.loginUser("testuser", "password123"));
    QVERIFY(userAuth.isLoggedIn());
}

void TestSuite::testDuplicateRegistrationFails()
{
    userAuth.registerUser("testuser", "password123");
    QVERIFY2(!userAuth.registerUser("testuser", "another_password"), "Should not allow duplicate usernames.");
}

void TestSuite::testFailedLogin()
{
    userAuth.registerUser("testuser", "password123");
    userAuth.logoutUser();
    QVERIFY2(!userAuth.loginUser("testuser", "wrong_password"), "Login should fail with an incorrect password.");
}

void TestSuite::testPasswordHashing()
{
    std::string password = "my_super_secret_password";
    std::string hash = userAuth.hashPassword(password);
    
    QVERIFY(!hash.empty());
    QVERIFY(hash != password);
    QVERIFY(userAuth.verifyPassword(password, hash));
}


// This macro creates an executable that will run all the test slots in this class.
QTEST_APPLESS_MAIN(TestSuite)

// This line must be included at the end of the .cpp file when Q_OBJECT
// is used within the same file. It includes the auto-generated code.
#include "test_suite.moc"