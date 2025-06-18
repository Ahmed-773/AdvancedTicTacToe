# Software Design Specification (SDS) for Advanced Tic Tac Toe Game 

---

## Table of Contents  
1.  **Introduction**
    * 1.1 Purpose
2.  **System Architecture**
    * 2.1 Architectural Design
3.  **Detailed Component Design**
    * 3.1 GUIInterface Class
    * 3.2 GameLogic Class
    * 3.3 AIEngine Class
    * 3.4 UserAuth Class
    * 3.5 DatabaseManager Class
    * 3.6 GameHistory Class
4.  **UML Diagrams**
    * 4.1 Class Diagram
    * 4.2 Sequence Diagram: User Login
    * 4.3 Sequence Diagram: Player vs. AI Move
5.  **Data Design**
    * 5.1 Key Data Structures
    * 5.2 Database Schema
---

## 1. Introduction

### 1.1 Purpose  
The goal of this document is to describe how the Advanced Tic Tac Toe game is structured and designed. We’ve broken everything down into modules and explained how each part works and communicates with the others. This guide is mainly for the development team — to help with implementation and keep everyone on the same page.

---

## 2. System Architecture

### 2.1 Architectural Design  
We followed a modular and layered architecture that resembles the Model-View-Controller (MVC) pattern. The idea is to keep things clean and easy to maintain.

- **View/Controller (the GUI):**  
  The `GUIInterface` class acts both as the view (what users see) and the controller (handling their input). It's built with Qt and is the main way users interact with the game. It also connects to the backend components.

- **Model (the backend logic):**  
  The logic is split into different classes, each with a clear role:
  - `GameLogic`: Handles the game rules and board state.
  - `AIEngine`: Makes smart decisions for the AI player.
  - `UserAuth`: Manages login, registration, and session tracking.
  - `GameHistory`: Keeps track of past games.
  - `DatabaseManager`: Deals with reading/writing data to files.

Each module does one job really well, and they don’t depend on the GUI, which makes it easier to test and update things.

---

## 3. Detailed Component Design

### 3.1 GUIInterface Class  
**What it does:**  
It builds the interface, responds to clicks and other inputs, and passes tasks to the backend logic. It also holds instances of the main backend classes.

**Main attributes:**  
- `mainStack` (QStackedWidget)  
- `boardButtons` (array of QPushButton)  
- `gameLogic`, `aiEngine`, etc.

**Important methods:**  
- `setupUI()`  
- `onLoginButtonClicked()`  
- `onCellClicked()`  
- `handleGameOver()`

---

### 3.2 GameLogic Class  
**What it does:**  
Tracks the board state, which player’s turn it is, and the move history for a game.

**Main attributes:**  
- `board` (2D array of players)  
- `currentPlayer` (enum)  
- `moveHistory` (vector of moves)

**Important methods:**  
- `makeMove()`  
- `checkGameResult()`  
- `isValidMove()`  
- `resetBoard()`

---

### 3.3 AIEngine Class  
**What it does:**  
Picks the best move for the AI player based on the current game state.

**Main attribute:**  
- `difficulty` (how deep the AI searches)

**Important methods:**  
- `getBestMove()`  
- `minimax()` (used internally, with alpha-beta pruning)

---

### 3.4 UserAuth Class  
**What it does:**  
Handles creating new users, logging them in, and managing active sessions securely.

**Main attributes:**  
- `users` (map of usernames to profiles)  
- `currentUser` (points to the logged-in user)

**Important methods:**  
- `registerUser()`  
- `loginUser()`  
- `hashPassword()`  
- `verifyPassword()`

---

### 3.5 DatabaseManager Class  
**What it does:**  
Takes care of saving and loading data to and from files, using simple string serialization.

**Main attribute:**  
- `db_file_path_` (string path to the file)

**Important methods:**  
- `saveUsers()`  
- `loadUsers()`  
- `writeToFile()`  
- `readFromFile()`

---

### 3.6 GameHistory Class  
**What it does:**  
Stores completed games for users and allows them to view or replay them.

**Main attribute:**  
- `gameHistory` (vector of finished games)

**Important methods:**  
- `saveGame()`  
- `getUserGames()`  
- `replayGame()`

---

## 4. UML Diagrams
### 4.1 Class Diagram  
This diagram would show how all the major classes relate to each other. `GUIInterface` uses (has-a) the backend classes like `GameLogic`, `AIEngine`, `UserAuth`, etc.
> ![alt text](<Editor _ Mermaid Chart-2025-06-17-231449.png>)

### 4.2 Sequence Diagram: User Login  
Here’s what happens when a user logs in:
1. User clicks the login button on the interface.
2. `onLoginButtonClicked()` is triggered.
3. Username and password are read from the inputs.
4. The GUI calls `loginUser()` on `UserAuth`.
5. `UserAuth` checks the password.
6. Result is returned to the GUI.
7. GUI either goes to the main game screen or shows an error.

### 4.3 Sequence Diagram: Player vs. AI Move  
Here’s the process when it’s the user vs. AI:
1. User clicks a cell.
2. GUI calls `makeMove()` in `GameLogic`.
3. Board updates and checks game state.
4. Now it’s AI’s turn — GUI calls `getBestMove()` in `AIEngine`.
5. AI runs its algorithm to find the best move.
6. The move is sent back to the GUI.
7. GUI makes the AI’s move and updates the board again.

> ![alt text](<Editor _ Mermaid Chart-2025-06-17-232507.png>)

---

## 5. Data Design

### 5.1 Key Data Structures  
- **Game Board:**  
  We use `std::array<std::array<Player, 3>, 3>` — a fixed 3x3 grid for the game.  
- **Move History:**  
  Stored in a `std::vector<Move>` so we can grow it as moves happen.  
- **User Profiles:**  
  Stored in an `unordered_map<std::string, UserProfile>` for quick lookups.

---

### 5.2 Database Schema  
Instead of using a full database, we store data in plain text files using `|` as a delimiter.

#### User Data File  
Path: `/path/tictactoe_data.db.users`  
- Each line = one user  
- Format:  
  `userId|username|passwordHash|gamesPlayed|gamesWon|gamesLost|gamesTied`

#### Game Data File  
Path: `/path/tictactoe_data.db.games`  
- Each line = one game  
- Format:  
  `gameId|player1Id|player2Id|isAIOpponent|result|timestamp|moveHistory`  
- **Move History Example:**  
  Moves are saved as coordinate pairs: `1,1;0,2;2,0;...`

---
