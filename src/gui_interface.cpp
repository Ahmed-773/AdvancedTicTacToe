// gui_interface.cpp
/*
================================================================================
File: gui-interface-impl.cpp
Description: Updated GUI implementation with a modern and clean design
             using Qt Style Sheets (QSS). The layout, fonts, colors, and
             widget styles have been completely revamped for a professional look.
================================================================================
*/
#include "gui_interface.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFont>
#include <QMessageBox>

GUIInterface::GUIInterface(const std::string& dbPath, QWidget *parent)
    : QMainWindow(parent), dbManager(dbPath) {
    
    applyStylesheet();

    setupUI();

    auto loadedUsers = dbManager.loadUsers();
    userAuth.setUsers(loadedUsers);
    gameHistory.loadFromDatabase(dbManager);
    
    aiEngine.setDifficulty(2);
    
    switchToLoginView();
}

GUIInterface::~GUIInterface() {}

void GUIInterface::applyStylesheet() {
    // The stylesheet is applied once to the entire application
    QString style = R"(
        QMainWindow {
            background-color: #F0F2F5;
        }
        QWidget {
            font-family: 'Segoe UI', 'Helvetica Neue', 'Arial', sans-serif;
            color: #333;
        }
        QStackedWidget {
            background-color: transparent;
        }
        QGroupBox {
            background-color: #FFFFFF;
            border: 1px solid #D1D9E6;
            border-radius: 8px;
            margin-top: 1ex;
            font-size: 14px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 10px;
            background-color: #FFFFFF;
            color: #007BFF;
        }
        QLabel {
            font-size: 14px;
            color: #555;
        }
        QLabel#titleLabel {
            font-size: 28px;
            font-weight: bold;
            color: #007BFF;
            padding: 20px;
        }
        QLabel#statusLabel {
            font-size: 20px;
            font-weight: bold;
            color: #333;
            padding: 10px;
            background-color: #E9ECEF;
            border-radius: 8px;
        }
        QLineEdit {
            border: 1px solid #CED4DA;
            padding: 10px;
            border-radius: 5px;
            font-size: 14px;
            background-color: #FFFFFF;
        }
        QLineEdit:focus {
            border: 1px solid #007BFF;
        }
        QPushButton {
            background-color: #007BFF;
            color: white;
            border: none;
            padding: 12px 20px;
            font-size: 14px;
            font-weight: bold;
            border-radius: 5px;
        }
        QPushButton:hover {
            background-color: #0056b3;
        }
        QPushButton:pressed {
            background-color: #004085;
        }
        QPushButton#gameBoardButton {
            font-size: 48px;
            font-weight: bold;
            background-color: #FFFFFF;
            color: #333;
            border: 2px solid #D1D9E6;
            border-radius: 8px;
        }
        QPushButton#gameBoardButton:hover {
            border: 2px solid #007BFF;
        }
        QRadioButton {
            font-size: 14px;
        }
        QComboBox {
            border: 1px solid #CED4DA;
            border-radius: 5px;
            padding: 8px;
            background-color: white;
        }
        /* ======================================================= */
        /* === Fix for the dropdown list text color visibility === */
        /* ======================================================= */
        QComboBox QAbstractItemView {
            border: 1px solid #D1D9E6;
            background-color: white;
            selection-background-color: #007BFF; /* Blue highlight for selection */
            selection-color: white;             /* White text for selected item */
            color: #333;                         /* Default text color for non-selected items */
        }
        QTableWidget {
            border: 1px solid #D1D9E6;
            border-radius: 8px;
            background-color: white;
            gridline-color: #E9ECEF;
        }
        QHeaderView::section {
            background-color: #007BFF;
            color: white;
            padding: 8px;
            font-size: 14px;
            font-weight: bold;
            border: none;
        }
    )";
    qApp->setStyleSheet(style);
}

// ... All other functions remain exactly the same as before ...

void GUIInterface::setupUI() {
    setWindowTitle("Advanced Tic Tac Toe");
    resize(800, 700);
    
    mainStack = new QStackedWidget(this);
    setCentralWidget(mainStack);
    
    setupAuthentication();
    setupGameBoard();
    setupHistoryView();
}

void GUIInterface::setupAuthentication() {
    loginWidget = new QWidget();
    QVBoxLayout *loginLayout = new QVBoxLayout(loginWidget);
    loginLayout->setAlignment(Qt::AlignCenter);
    loginLayout->setSpacing(20);

    QLabel *titleLabel = new QLabel("Advanced Tic Tac Toe");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    QGroupBox *authBox = new QGroupBox("User Authentication");
    authBox->setFixedWidth(400);
    QGridLayout *authLayout = new QGridLayout(authBox);
    authLayout->setSpacing(15);
    
    QLabel *usernameLabel = new QLabel("Username:");
    usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText("Enter your username");
    
    QLabel *passwordLabel = new QLabel("Password:");
    passwordInput = new QLineEdit();
    passwordInput->setPlaceholderText("Enter your password");
    passwordInput->setEchoMode(QLineEdit::Password);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    loginButton = new QPushButton("Login");
    registerButton = new QPushButton("Register");
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);
    
    authLayout->addWidget(usernameLabel, 0, 0);
    authLayout->addWidget(usernameInput, 0, 1);
    authLayout->addWidget(passwordLabel, 1, 0);
    authLayout->addWidget(passwordInput, 1, 1);
    authLayout->addLayout(buttonLayout, 2, 0, 1, 2);

    loginLayout->addWidget(titleLabel);
    loginLayout->addWidget(authBox, 0, Qt::AlignHCenter);
    
    connect(loginButton, &QPushButton::clicked, this, &GUIInterface::onLoginButtonClicked);
    connect(registerButton, &QPushButton::clicked, this, &GUIInterface::onRegisterButtonClicked);
    
    mainStack->addWidget(loginWidget);
}

void GUIInterface::setupGameBoard() {
    gameWidget = new QWidget();
    QHBoxLayout *mainGameLayout = new QHBoxLayout(gameWidget);
    mainGameLayout->setSpacing(20);

    QVBoxLayout *boardAreaLayout = new QVBoxLayout();
    boardAreaLayout->setAlignment(Qt::AlignCenter);

    statusLabel = new QLabel("Player X's Turn");
    statusLabel->setObjectName("statusLabel");
    statusLabel->setAlignment(Qt::AlignCenter);
    
    QGroupBox *boardBox = new QGroupBox("Game Board");
    boardLayout = new QGridLayout(boardBox);
    boardLayout->setSpacing(10);
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            boardButtons[i][j] = new QPushButton("");
            boardButtons[i][j]->setObjectName("gameBoardButton");
            boardButtons[i][j]->setFixedSize(120, 120);
            boardButtons[i][j]->setProperty("row", i);
            boardButtons[i][j]->setProperty("col", j);
            boardLayout->addWidget(boardButtons[i][j], i, j);
            connect(boardButtons[i][j], &QPushButton::clicked, this, &GUIInterface::onCellClicked);
        }
    }
    
    boardAreaLayout->addWidget(statusLabel);
    boardAreaLayout->addWidget(boardBox);
    
    QVBoxLayout *controlAreaLayout = new QVBoxLayout();
    controlAreaLayout->setSpacing(15);
    controlAreaLayout->setAlignment(Qt::AlignTop);

    QGroupBox *controlBox = new QGroupBox("Game Controls");
    controlBox->setFixedWidth(250);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlBox);
    controlLayout->setSpacing(10);
    
    QGroupBox *modeBox = new QGroupBox("Game Mode");
    modeBox->setTitle("");
    QVBoxLayout *modeLayout = new QVBoxLayout(modeBox);
    pvpModeRadio = new QRadioButton("Player vs Player");
    aiModeRadio = new QRadioButton("Player vs AI");
    modeLayout->addWidget(pvpModeRadio);
    modeLayout->addWidget(aiModeRadio);
    
    QGroupBox *difficultyBox = new QGroupBox("AI Difficulty");
    difficultyBox->setTitle("");
    QVBoxLayout *difficultyLayout = new QVBoxLayout(difficultyBox);
    difficultyCombo = new QComboBox();
    difficultyCombo->addItem("Easy", 1);
    difficultyCombo->addItem("Medium", 2);
    difficultyCombo->addItem("Hard", 3);
    difficultyCombo->setCurrentIndex(1);
    difficultyLayout->addWidget(new QLabel("AI Difficulty:"));
    difficultyLayout->addWidget(difficultyCombo);

    newGameButton = new QPushButton("New Game");
    viewHistoryButton = new QPushButton("View History");
    logoutButton = new QPushButton("Logout");
    
    controlLayout->addWidget(modeBox);
    controlLayout->addWidget(difficultyBox);
    controlLayout->addStretch();
    controlLayout->addWidget(newGameButton);
    controlLayout->addWidget(viewHistoryButton);
    controlLayout->addWidget(logoutButton);
    
    controlAreaLayout->addWidget(controlBox);
    
    mainGameLayout->addLayout(boardAreaLayout);
    mainGameLayout->addLayout(controlAreaLayout);
    
    pvpModeRadio->setChecked(true);
    
    connect(newGameButton, &QPushButton::clicked, this, &GUIInterface::onNewGameButtonClicked);
    connect(viewHistoryButton, &QPushButton::clicked, this, &GUIInterface::onViewHistoryClicked);
    connect(logoutButton, &QPushButton::clicked, this, &GUIInterface::onLogoutButtonClicked);
    connect(pvpModeRadio, &QRadioButton::toggled, this, &GUIInterface::onGameModeChanged);
    connect(difficultyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &GUIInterface::onDifficultyChanged);
    
    mainStack->addWidget(gameWidget);
}

void GUIInterface::setupHistoryView() {
    historyWidget = new QWidget();
    QVBoxLayout *historyLayout = new QVBoxLayout(historyWidget);
    historyLayout->setContentsMargins(20, 20, 20, 20);
    historyLayout->setSpacing(15);

    QLabel *historyTitle = new QLabel("Game History");
    historyTitle->setObjectName("titleLabel");
    historyTitle->setAlignment(Qt::AlignCenter);
    
    gameHistoryTable = new QTableWidget(0, 4);
    gameHistoryTable->setHorizontalHeaderLabels({"Date", "Opponent", "Result", "Mode"});
    gameHistoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    gameHistoryTable->setAlternatingRowColors(true);
    gameHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    gameHistoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    backToGameButton = new QPushButton("Back to Game");
    
    historyLayout->addWidget(historyTitle);
    historyLayout->addWidget(gameHistoryTable);
    historyLayout->addWidget(backToGameButton, 0, Qt::AlignRight);
    
    connect(backToGameButton, &QPushButton::clicked, this, &GUIInterface::onBackToGameClicked);
    connect(gameHistoryTable, &QTableWidget::cellDoubleClicked, this, &GUIInterface::onGameHistoryItemClicked);
    
    mainStack->addWidget(historyWidget);
}

void GUIInterface::updateBoard() {
    statusLabel->setText(gameLogic.getCurrentPlayer() == Player::X ? "Player X's Turn" : "Player O's Turn");
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Player cell = gameLogic.getCell(i, j);
            if (cell == Player::X) {
                boardButtons[i][j]->setText("X");
                boardButtons[i][j]->setStyleSheet("color: #007BFF;");
            } else if (cell == Player::O) {
                boardButtons[i][j]->setText("O");
                boardButtons[i][j]->setStyleSheet("color: #E74C3C;");
            } else {
                boardButtons[i][j]->setText("");
                boardButtons[i][j]->setStyleSheet("");
            }
        }
    }
}

void GUIInterface::onLoginButtonClicked() {
    QString username = usernameInput->text();
    QString password = passwordInput->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Error", "Username and password cannot be empty");
        return;
    }
    
    if (userAuth.loginUser(username.toStdString(), password.toStdString())) {
        switchToGameView();
    } else {
        QMessageBox::warning(this, "Login Error", "Invalid username or password");
    }
}

void GUIInterface::onRegisterButtonClicked() {
    QString username = usernameInput->text();
    QString password = passwordInput->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Registration Error", "Username and password cannot be empty");
        return;
    }
    
    if (userAuth.registerUser(username.toStdString(), password.toStdString())) {
        dbManager.saveUser(*userAuth.getCurrentUser());
        QMessageBox::information(this, "Registration Successful", "User registered successfully. You can now login.");
    } else {
        QMessageBox::warning(this, "Registration Error", "Username already exists");
    }
}

void GUIInterface::onLogoutButtonClicked() {
    userAuth.logoutUser();
    switchToLoginView();
}

void GUIInterface::onCellClicked() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button || gameLogic.checkGameResult() != GameResult::IN_PROGRESS) return;
    
    int row = button->property("row").toInt();
    int col = button->property("col").toInt();
    
    if (gameLogic.makeMove(row, col)) {
        updateBoard();
        
        GameResult result = gameLogic.checkGameResult();
        if (result != GameResult::IN_PROGRESS) {
            handleGameOver(result);
            return;
        }
        
        if (isAIMode() && gameLogic.getCurrentPlayer() == Player::O) {
            makeAIMove();
            result = gameLogic.checkGameResult();
            if (result != GameResult::IN_PROGRESS) {
                handleGameOver(result);
            }
        }
    }
}

void GUIInterface::onNewGameButtonClicked() {
    gameLogic.resetBoard();
    updateBoard();
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            boardButtons[i][j]->setEnabled(true);
        }
    }
}

void GUIInterface::onGameModeChanged() {
    if (gameLogic.checkGameResult() == GameResult::IN_PROGRESS) {
        onNewGameButtonClicked();
    }
}

void GUIInterface::onDifficultyChanged(int index) {
    int difficulty = difficultyCombo->itemData(index).toInt();
    aiEngine.setDifficulty(difficulty);
}

void GUIInterface::onViewHistoryClicked() {
    loadUserGames();
    switchToHistoryView();
}

void GUIInterface::onGameHistoryItemClicked(int row, int column) {
    QString gameId = gameHistoryTable->item(row, 0)->data(Qt::UserRole).toString();
    GameState game = gameHistory.getGameById(gameId.toStdString());
    displayGame(game);
}

void GUIInterface::onBackToGameClicked() {
    switchToGameView();
}

void GUIInterface::handleGameOver(GameResult result) {
    QString resultMessage;
    switch (result) {
        case GameResult::X_WINS: resultMessage = "Player X wins!"; break;
        case GameResult::O_WINS: resultMessage = "Player O wins!"; break;
        case GameResult::DRAW: resultMessage = "Game ended in a draw!"; break;
        default: return;
    }
    
    statusLabel->setText(resultMessage);
    
    if (userAuth.isLoggedIn()) {
        const UserProfile* currentUser = userAuth.getCurrentUser();
        std::string player1Id = currentUser->userId;
        std::string player2Id = isAIMode() ? "AI" : "Player2";
        gameHistory.saveGame(player1Id, player2Id, isAIMode(), gameLogic.getMoveHistory(), result, dbManager);
        userAuth.updateUserStats(result);
    }
    
    QMessageBox::information(this, "Game Over", resultMessage);
}

void GUIInterface::switchToLoginView() {
    mainStack->setCurrentWidget(loginWidget);
    usernameInput->clear();
    passwordInput->clear();
}

void GUIInterface::switchToGameView() {
    mainStack->setCurrentWidget(gameWidget);
    if (gameLogic.checkGameResult() != GameResult::IN_PROGRESS) {
        onNewGameButtonClicked();
    }
}

void GUIInterface::switchToHistoryView() {
    mainStack->setCurrentWidget(historyWidget);
}

bool GUIInterface::isAIMode() const {
    return aiModeRadio->isChecked();
}

void GUIInterface::makeAIMove() {
    Move aiMove = aiEngine.getBestMove(gameLogic);
    gameLogic.makeMove(aiMove.row, aiMove.col);
    updateBoard();
}

void GUIInterface::loadUserGames() {
    if (!userAuth.isLoggedIn()) return;
    
    const UserProfile* currentUser = userAuth.getCurrentUser();
    std::vector<GameState> userGames = gameHistory.getUserGames(currentUser->userId);
    
    gameHistoryTable->setRowCount(0);
    
    for (const auto& game : userGames) {
        int row = gameHistoryTable->rowCount();
        gameHistoryTable->insertRow(row);
        
        QTableWidgetItem* dateItem = new QTableWidgetItem(QString::fromStdString(game.timestamp));
        dateItem->setData(Qt::UserRole, QString::fromStdString(game.gameId));
        gameHistoryTable->setItem(row, 0, dateItem);
        
        QString opponent = game.isAIOpponent ? "AI" : QString::fromStdString(game.player2Id);
        gameHistoryTable->setItem(row, 1, new QTableWidgetItem(opponent));
        
        QString result;
        switch (game.result) {
            case GameResult::X_WINS: result = "X Wins"; break;
            case GameResult::O_WINS: result = "O Wins"; break;
            case GameResult::DRAW: result = "Draw"; break;
            default: result = "Unknown";
        }
        gameHistoryTable->setItem(row, 2, new QTableWidgetItem(result));
        
        QString mode = game.isAIOpponent ? "vs AI" : "vs Player";
        gameHistoryTable->setItem(row, 3, new QTableWidgetItem(mode));
    }
}

void GUIInterface::displayGame(const GameState& game) {
    gameLogic = gameHistory.replayGame(game.gameId);
    switchToGameView();
    updateBoard();
    statusLabel->setText("Game Replay: " + QString::fromStdString(game.timestamp));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            boardButtons[i][j]->setEnabled(false);
        }
    }
}