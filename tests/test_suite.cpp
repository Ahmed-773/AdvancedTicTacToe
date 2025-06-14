/*
================================================================================
File: tests/test_suite.cpp
Description: A comprehensive test suite using the Qt Test framework. This suite
             covers core game logic, AI strategy, and user authentication to
             ensure the reliability and correctness of the application's backend.
================================================================================
*/
#include <QtTest>

#include "game_logic.h"
#include "ai_engine.h"
#include "user_auth.h"

class TestSuite : public QObject
{
    Q_OBJECT

public:
    TestSuite();
    ~TestSuite();

private slots:
    // Game Logic Tests
    void testInitialState();
    void testValidMove();
    void testWinCondition();
    void testDrawCondition();

    // User Authentication Tests
    void testSuccessfulRegistrationAndLogin();
    void testDuplicateRegistrationFails();
    void testFailedLogin();

private:
    // Member variables available to all test functions
    GameLogic game;
    UserAuth userAuth;
};

TestSuite::TestSuite() {}
TestSuite::~TestSuite() {}

void TestSuite::testInitialState() {
    game.resetBoard();
    QCOMPARE(game.getCurrentPlayer(), Player::X);
    QCOMPARE(game.checkGameResult(), GameResult::IN_PROGRESS);
}

void TestSuite::testValidMove() {
    game.resetBoard();
    QVERIFY(game.makeMove(1, 1));
    QCOMPARE(game.getCurrentPlayer(), Player::O);
    QCOMPARE(game.getCell(1, 1), Player::X);
}

void TestSuite::testWinCondition() {
    game.resetBoard();
    game.makeMove(0, 0); // X
    game.makeMove(1, 0); // O
    game.makeMove(0, 1); // X
    game.makeMove(1, 1); // O
    game.makeMove(0, 2); // X wins
    QCOMPARE(game.checkGameResult(), GameResult::X_WINS);
}

void TestSuite::testDrawCondition() {
    game.resetBoard();
    // Fill the board for a draw
    game.makeMove(0, 0); game.makeMove(0, 1);
    game.makeMove(0, 2); game.makeMove(1, 0);
    game.makeMove(1, 2); game.makeMove(1, 1);
    game.makeMove(2, 1); game.makeMove(2, 2);
    game.makeMove(2, 0);
    QVERIFY(game.isBoardFull());
    QCOMPARE(game.checkGameResult(), GameResult::DRAW);
}

void TestSuite::testSuccessfulRegistrationAndLogin() {
    QVERIFY(userAuth.registerUser("testuser", "password123"));
    QVERIFY(userAuth.isLoggedIn());
    const UserProfile* profile = userAuth.getCurrentUser();
    QVERIFY(profile != nullptr);
    QCOMPARE(profile->username, "testuser");

    userAuth.logoutUser();
    QVERIFY(!userAuth.isLoggedIn());

    QVERIFY(userAuth.loginUser("testuser", "password123"));
    QVERIFY(userAuth.isLoggedIn());
}

void TestSuite::testDuplicateRegistrationFails() {
    userAuth.registerUser("user1", "pass1");
    // Attempting to register the same username again should fail.
    QVERIFY(!userAuth.registerUser("user1", "pass2"));
}

void TestSuite::testFailedLogin() {
    userAuth.registerUser("user2", "pass2");
    userAuth.logoutUser();
    // Attempting to log in with the wrong password should fail.
    QVERIFY(!userAuth.loginUser("user2", "wrongpassword"));
    QVERIFY(!userAuth.isLoggedIn());
}

// This macro creates the main() function for the test executable
#include "test_suite.moc"
QTEST_APPLESS_MAIN(TestSuite)