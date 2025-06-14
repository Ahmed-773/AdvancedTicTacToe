/*
================================================================================
File: tests/test_suite.cpp
Description: Comprehensive test suite using the Qt Test framework
================================================================================
*/
#include <QtTest>

// Include the headers for the classes we want to test
#include "game_logic.h"
#include "ai_engine.h"
#include "user_auth.h"

// In Qt Test, a test suite is a class that inherits from QObject.
class TestSuite : public QObject
{
    Q_OBJECT // This macro is required for Qt's meta-object system.

public:
    TestSuite();
    ~TestSuite();

// Test cases are defined as private slots.
private slots:
    void testInitialState();
    void testValidMove();
    void testWinCondition();

private:
    GameLogic game;
};

TestSuite::TestSuite() {}
TestSuite::~TestSuite() {}

void TestSuite::testInitialState()
{
    game.resetBoard();
    // QCOMPARE is the Qt equivalent of Google Test's ASSERT_EQ.
    QCOMPARE(game.getCurrentPlayer(), Player::X);
    QCOMPARE(game.checkGameResult(), GameResult::IN_PROGRESS);
}

void TestSuite::testValidMove()
{
    game.resetBoard();
    // QVERIFY is the Qt equivalent of ASSERT_TRUE.
    QVERIFY(game.makeMove(1, 1));
    QCOMPARE(game.getCurrentPlayer(), Player::O);
    QCOMPARE(game.getCell(1, 1), Player::X);
}

void TestSuite::testWinCondition()
{
    game.resetBoard();
    game.makeMove(0, 0); // X
    game.makeMove(1, 0); // O
    game.makeMove(0, 1); // X
    game.makeMove(1, 1); // O
    game.makeMove(0, 2); // X wins
    
    QCOMPARE(game.checkGameResult(), GameResult::X_WINS);
}

// This macro creates an executable that will run all the test slots.
QTEST_APPLESS_MAIN(TestSuite)

// This line must be included at the end of the .cpp file when Q_OBJECT
// is used within the same file. It includes the auto-generated code.
#include "test_suite.moc"