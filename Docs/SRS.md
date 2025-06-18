Software Requirements Specification (SRS)
Advanced Tic Tac Toe Game
Version 1.0

Table of Contents
Introduction

1.1. Purpose

1.2. Scope

1.3. Definitions, Acronyms, and Abbreviations

Overall Description

2.1. Product Perspective

2.2. Product Functions

2.3. User Characteristics

System Features and Requirements

3.1. Functional Requirements

3.1.1. User Authentication

3.1.2. Gameplay Mechanics

3.1.3. AI Opponent

3.1.4. User Profile and Statistics

3.2. Non-Functional Requirements

3.2.1. Performance

3.2.2. Security

3.2.3. Usability

3.2.4. Reliability

3.2.5. Maintainability

External Interface Requirements

4.1. User Interfaces

4.2. Software Interfaces

1. Introduction
1.1. Purpose
This Software Requirements Specification (SRS) document provides a detailed description of the requirements for the Advanced Tic Tac Toe game. It outlines the functional and non-functional requirements of the system and serves as the foundational agreement between the development team and the stakeholders on what the product will do.

1.2. Scope
The product is a desktop application that modernizes the classic Tic Tac Toe game. It will feature a graphical user interface (GUI), a robust user account system, persistent storage of player statistics, and a challenging AI opponent. The scope includes the game logic, user management, AI engine, and data persistence layers.

1.3. Definitions, Acronyms, and Abbreviations
Term

Definition

GUI

Graphical User Interface

AI

Artificial Intelligence

SRS

Software Requirements Specification

CI/CD

Continuous Integration / Continuous Deployment

MVC

Model-View-Controller, a software design pattern

2. Overall Description
2.1. Product Perspective
The Advanced Tic Tac Toe game is a self-contained desktop application built using C++ and the Qt framework. It follows a modular architecture to separate core logic from the user interface, enhancing testability and maintainability.

2.2. Product Functions
The major functions of the system are:

Secure user registration and login.

Interactive Player vs. AI gameplay.

Automatic tracking and display of user game statistics.

A challenging AI opponent powered by the Minimax algorithm.

Persistent storage of user profiles and statistics across sessions.

2.3. User Characteristics
The target users are individuals who enjoy classic strategy games. Users are expected to be familiar with standard GUI operations (mouse clicks, typing in text fields) but require no specialized technical knowledge.

3. System Features and Requirements
3.1. Functional Requirements
3.1.1. User Authentication
FR-1 (User Registration): The system shall allow a new user to create an account by providing a unique username and a password. The system shall reject registration if the username is already taken.

FR-2 (User Login): The system shall allow a registered user to log in using their username and password. Upon successful login, the user's session shall become active.

FR-3 (User Logout): The system shall allow a logged-in user to log out, ending their session.

3.1.2. Gameplay Mechanics
FR-4 (Game Board Display): The system shall display a 3x3 grid as the game board.

FR-5 (Making a Move): The system shall allow the current player to place their mark (X or O) in an empty cell by clicking on it. The system shall not allow a move in an occupied cell.

FR-6 (Turn Alternation): The system shall alternate turns between Player X and Player O after each valid move.

FR-7 (Win Detection): The system shall automatically detect and declare a winner when a player achieves three of their marks in a horizontal, vertical, or diagonal row.

FR-8 (Draw Detection): The system shall automatically detect and declare a draw if all cells on the board are filled and no winner is detected.

FR-9 (New Game): The system shall provide an option to start a new game at any time, which resets the board to its initial state.

3.1.3. AI Opponent
FR-10 (AI Gameplay): The system shall provide a Player vs. AI game mode where the user plays against a computer-controlled opponent.

FR-11 (AI Decision Making): The AI opponent shall use the Minimax algorithm to determine its optimal move. The AI shall be capable of blocking opponent wins and securing its own wins when possible.

3.1.4. User Profile and Statistics
FR-12 (Stat Tracking): The system shall track the following statistics for each registered user: Games Played, Games Won, Games Lost, and Games Tied.

FR-13 (Stat Updates): The system shall automatically update the logged-in user's statistics at the conclusion of each game.

FR-14 (Profile View): The system shall provide a screen where the logged-in user can view their username and current statistics.

FR-15 (Data Persistence): User profiles and statistics shall be saved to local files and persist between application sessions.

3.2. Non-Functional Requirements
3.2.1. Performance
NFR-1 (AI Response Time): The AI's move calculation shall take no longer than 500 milliseconds on the target hardware to ensure a responsive user experience.

3.2.2. Security
NFR-2 (Password Hashing): User passwords shall never be stored in plaintext. The system must use a strong one-way hashing algorithm (SHA-256) to store password credentials.

3.2.3. Usability
NFR-3 (Intuitive Interface): The GUI shall be clean, intuitive, and easy to navigate for users with no prior training. All interactive elements shall be clearly labeled.

3.2.4. Reliability
NFR-4 (System Stability): The application shall be stable and handle user errors (e.g., invalid login attempts) gracefully without crashing.

NFR-5 (Continuous Integration): The project shall be supported by a CI/CD pipeline that automatically builds and runs a test suite on every code change to ensure the main branch remains stable.

3.2.5. Maintainability
NFR-6 (Modularity): The source code shall be organized into decoupled modules (e.g., GameLogic, UserAuth, AIEngine) to facilitate easier updates and maintenance.

NFR-7 (Code Documentation): All major classes and complex functions shall be clearly documented with comments explaining their purpose and behavior.

4. External Interface Requirements
4.1. User Interfaces
The application will present a graphical user interface with three primary views:

Authentication View: Fields for username/password, and buttons for Login/Register.

Game View: The 3x3 game board, a status label, and buttons for New Game, Profile, and Logout.

Profile View: Display of user statistics and a button to return to the game.

4.2. Software Interfaces
Qt 6 Framework: The application depends on the Qt 6 libraries for its GUI components and core functionalities.

OpenSSL: The application depends on the OpenSSL library for the SHA-256 hashing algorithm.