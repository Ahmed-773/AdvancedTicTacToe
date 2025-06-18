# Testing Documentation for Advanced Tic Tac Toe Game

---

## Table of Contents

1.  **Introduction**
    * 1.1. Purpose
    * 1.2. Testing Strategy
    * 1.3. Testing Framework
2.  **Test Environment**
3.  **Test Cases and Execution**
    * 3.1. GameLogic Module Tests
    * 3.2. UserAuth Module Tests
4.  **Test Results**
5.  **Test Coverage Analysis**
6.  **Conclusion and Future Work**

---

## 1. Introduction

### 1.1. Purpose

This document outlines the testing strategy, test cases, and results for the Advanced Tic Tac Toe project. We're not just building a game – we're building a **reliable** game!

The purpose of this documentation is to:
- Verify that the software meets its functional requirements as defined in the SRS.
- Ensure code quality and reliability through automated checks.
- Provide confidence in the correctness of the implemented backend logic.

### 1.2. Testing Strategy

Our testing strategy is centered on **Unit Testing** the core backend modules of the application. Why unit testing? Because it's like checking each ingredient before cooking a meal – we want to validate the correctness of each module in isolation before they're integrated into the final GUI application.

This approach allows us to:
- Pinpoint bugs with precision.
- Ensure that the foundational components are robust.
- Build confidence in our core logic.

**The modules prioritized for testing were:**
- **`GameLogic`**: The most critical component, responsible for enforcing the rules of Tic Tac Toe.
- **`UserAuth`**: To ensure the security-sensitive user authentication and password hashing mechanisms function correctly.

### 1.3. Testing Framework

The project was implemented using the **Qt Test framework**. Although the initial project specification mentioned Google Test, we made the strategic decision to switch to Qt Test for these practical reasons:

- **Seamless Integration**: As the GUI is built with Qt, using Qt Test eliminates the need for additional dependencies and complex build system configurations. It's like using matching kitchen appliances – everything works together smoothly!

- **Consistency**: It allows the entire project, including the tests, to be built and maintained within a single, consistent Qt ecosystem.

- **Sufficient Functionality**: Qt Test provides all the necessary features we need, including assertions (`QCOMPARE`, `QVERIFY`), test organization capabilities, and a reliable test runner required to meet the project's unit testing objectives.

---

## 2. Test Environment

All tests are executed automatically as part of our **Continuous Integration (CI) pipeline**, configured using GitHub Actions. This means every code change gets automatically tested – like having a quality control robot that never sleeps!

**Technical Specifications:**
- **Platform**: Windows-latest virtual machine
- **Compiler**: MSVC from Visual Studio 2022
- **Execution**: The `run_tests.exe` executable is built and run on every push and pull request to the `main` branch.
- **Quality Gate**: A failure in any test case will cause the entire CI workflow to fail, preventing buggy code from being merged. This ensures our `main` branch always contains working, reliable code.

---

## 3. Test Cases and Execution

The following test cases are defined in `tests/test_suite.cpp` and represent the core of our automated quality assurance.

### 3.1. GameLogic Module Tests

#### `testInitialState()`
- **What it does**: Verifies that a `GameLogic` object is initialized correctly, ensuring a new game board is set up properly.
- **Assertions**: Checks that the current player is `Player::X` and the game result is `IN_PROGRESS`.

#### `testValidMove()`
- **What it does**: Ensures that a valid move is processed correctly.
- **Assertions**: Verifies that `makeMove(1, 1)` returns `true`, the current player switches to `Player::O`, and the cell at (1, 1) contains `Player::X`.

#### `testWinCondition()`
- **What it does**: Verifies that the system can correctly detect a horizontal win condition.
- **Assertions**: Simulates a sequence of moves for a win and checks that `checkGameResult()` returns `GameResult::X_WINS`.

#### `testDrawCondition()`
- **What it does**: Verifies that the system can correctly detect a draw when the board is full.
- **Assertions**: Simulates a full game and checks that `isBoardFull()` is `true` and `checkGameResult()` returns `GameResult::DRAW`.

### 3.2. UserAuth Module Tests

#### `testSuccessfulRegistrationAndLogin()`
- **What it does**: Tests the complete, successful user lifecycle: registration, logout, and login.

#### `testDuplicateRegistrationFails()`
- **What it does**: Ensures a username cannot be used by more than one user.

#### `testFailedLogin()`
- **What it does**: Ensures the system rejects login attempts with incorrect credentials.

---

## 4. Test Results

**Current Status**: All 7 implemented tests pass successfully during automated execution in our CI pipeline. 🎉

The successful completion of the "Run tests" step in the GitHub Actions workflow serves as the official record of the test results. Every green checkmark represents working, reliable code.

---

## 5. Test Coverage Analysis

### High Coverage 📈
- **`GameLogic` Class**: Core rule-enforcing functions are well-tested.
- **`UserAuth` Class**: Critical user management workflows are thoroughly covered, implicitly testing the security model.

### Moderate Coverage 📊
- **`AIEngine` Class**: Correctness is currently verified through manual gameplay and the performance benchmark, but not yet by automated unit tests.

### Not Covered (By Design) 📋
- **`GUIInterface` Class**: UI components are verified through manual, exploratory testing.
- **`DatabaseManager` Class**: File I/O operations are inferred as correct through successful integration during manual testing.

---

## 6. Conclusion and Future Work

The current test suite provides strong confidence in the correctness of the critical backend logic.

For future development, the test suite could be expanded with the following tests:
- **`testInvalidMove()`**: To verify rejection of illegal moves.
- **`testAIBlocksOpponentWin()`**: To ensure the AI makes necessary defensive moves.
- **`testAITakesWinningMove()`**: To ensure the AI capitalizes on opportunities to win.

Implementing these additional tests would provide comprehensive automated coverage for all backend modules.
