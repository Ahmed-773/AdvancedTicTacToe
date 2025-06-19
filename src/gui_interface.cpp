// gui_interface.cpp
#include "gui_interface.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFont>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QIcon> // For button icons

GUIInterface::GUIInterface(const std::string& dbPath, QWidget *parent)
    : QMainWindow(parent), dbManager(dbPath), replayMoveIndex(0) {

    applyStylesheet();
    setupUI();

    // Load data at startup
    auto loadedUsers = dbManager.loadUsers();
    userAuth.setUsers(loadedUsers);
    gameHistory.loadFromDatabase(dbManager);

    aiEngine.setDifficulty(2); // Default to medium

    switchToLoginView();
}

GUIInterface::~GUIInterface() {}

void GUIInterface::applyStylesheet() {
    QString style = R"(
        QMainWindow, QWidget {
            background-color: #2C3E50;
            font-family: 'Segoe UI', sans-serif;
            color: #ECF0F1;
        }
        QStackedWidget { background-color: transparent; }
        QGroupBox {
            background-color: #34495E;
            border: 1px solid #4A6572;
            border-radius: 8px;
            margin-top: 1ex;
            font-size: 14px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 10px;
            color: #1ABC9C;
        }
        QLabel { font-size: 14px; color: #BDC3C7; }
        QLabel#titleLabel, QLabel#historyTitleLabel {
            font-size: 32px; font-weight: bold; color: #FFFFFF; padding: 20px;
        }
        QLabel#statusLabel {
            font-size: 22px; font-weight: bold; color: #FFFFFF; padding: 12px;
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #16A085, stop:1 #1ABC9C);
            border-radius: 8px;
            min-height: 40px;
        }
        QLabel#scoreLabel { font-size: 16px; font-weight: bold; color: #ECF0F1; }
        QLineEdit {
            border: 1px solid #4A6572; padding: 10px; border-radius: 5px;
            font-size: 14px; background-color: #2C3E50; color: #ECF0F1;
        }
        QLineEdit:focus { border: 1px solid #1ABC9C; }
        QPushButton {
            background-color: #1ABC9C; color: white; border: none;
            padding: 12px 20px; font-size: 14px; font-weight: bold;
            border-radius: 5px; min-width: 100px;
        }
        QPushButton:hover { background-color: #16A085; }
        QPushButton:pressed { background-color: #117A65; }
        QPushButton:disabled { background-color: #566573; }
        QPushButton#gameBoardButton {
            font-size: 52px; font-weight: bold; background-color: #34495E;
            border: 2px solid #4A6572; border-radius: 8px;
        }
        QPushButton#gameBoardButton:hover { border-color: #1ABC9C; }
        QPushButton#replayButton {
             background-color: #3498DB; min-width: 40px;
        }
        QPushButton#replayButton:hover { background-color: #2980B9; }
        QTableWidget {
            border: 1px solid #4A6572; border-radius: 8px;
            background-color: #34495E; gridline-color: #4A6572;
        }
        QHeaderView::section {
            background-color: #1ABC9C; color: white; padding: 8px;
            font-size: 14px; font-weight: bold; border: none;
        }
        QRadioButton::indicator {
            width: 15px; height: 15px;
            border: 2px solid #BDC3C7; border-radius: 9px;
        }
        QRadioButton::indicator:checked {
            background-color: #1ABC9C; border: 2px solid #1ABC9C;
        }
        QComboBox {
            border: 1px solid #4A6572; border-radius: 5px; padding: 8px;
            background-color: #34495E; color: #ECF0F1;
        }
        QComboBox::drop-down { border: none; }
        QComboBox QAbstractItemView {
            border: 1px solid #4A6572; background-color: #34495E;
            selection-background-color: #1ABC9C; color: #ECF0F1;
        }
    )";
    qApp->setStyleSheet(style);
}

void GUIInterface::setupUI() {
    setWindowTitle("Advanced Tic Tac Toe");
    resize(900, 750);
    setMinimumSize(800, 700);

    mainStack = new QStackedWidget(this);
    setCentralWidget(mainStack);

    // Create a central widget and layout for the game screen
    QWidget *gameScreenWidget = new QWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(gameScreenWidget);

    // --- LEFT SIDE: GAME BOARD & STATUS ---
    QVBoxLayout *leftLayout = new QVBoxLayout();
    statusLabel = new QLabel("Welcome!");
    statusLabel->setObjectName("statusLabel");
    statusLabel->setAlignment(Qt::AlignCenter);
    
    QGridLayout* boardLayout = new QGridLayout();
    boardLayout->setSpacing(10);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            boardButtons[i][j] = new QPushButton("");
            boardButtons[i][j]->setObjectName("gameBoardButton");
            boardButtons[i][j]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            boardButtons[i][j]->setProperty("row", i);
            boardButtons[i][j]->setProperty("col", j);
            connect(boardButtons[i][j], &QPushButton::clicked, this, &GUIInterface::onCellClicked);
            boardLayout->addWidget(boardButtons[i][j], i, j);
        }
    }
    leftLayout->addWidget(statusLabel);
    leftLayout->addLayout(boardLayout);

    // --- RIGHT SIDE: CONTROLS & SCORES ---
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(20);
    
    // Score Box
    QGroupBox* scoreBox = new QGroupBox("Scoreboard");
    QVBoxLayout* scoreLayout = new QVBoxLayout(scoreBox);
    playerXScoreLabel = new QLabel("Player X: W: 0 | L: 0 | T: 0");
    playerXScoreLabel->setObjectName("scoreLabel");
    playerOScoreLabel = new QLabel("Opponent: W: 0 | L: 0 | T: 0");
    playerOScoreLabel->setObjectName("scoreLabel");
    scoreLayout->addWidget(playerXScoreLabel);
    scoreLayout->addWidget(playerOScoreLabel);

    // Game Mode Box
    QGroupBox* modeBox = new QGroupBox("Game Mode");
    QVBoxLayout* modeLayout = new QVBoxLayout(modeBox);
    pvpModeRadio = new QRadioButton("Player vs Player");
    aiModeRadio = new QRadioButton("Player vs AI");
    aiModeRadio->setChecked(true); // Default to AI
    modeLayout->addWidget(pvpModeRadio);
    modeLayout->addWidget(aiModeRadio);

    // AI Difficulty Box
    QGroupBox* difficultyBox = new QGroupBox("AI Difficulty");
    QVBoxLayout* difficultyLayout = new QVBoxLayout(difficultyBox);
    difficultyCombo = new QComboBox();
    difficultyCombo->addItem("Easy", 1);
    difficultyCombo->addItem("Medium", 3);
    difficultyCombo->addItem("Hard (Unbeatable)", 9);
    difficultyCombo->setCurrentIndex(1);
    difficultyLayout->addWidget(difficultyCombo);

    // Main Buttons
    QPushButton* newGameButton = new QPushButton("New Game");
    QPushButton* viewHistoryButton = new QPushButton("View History");
    QPushButton* logoutButton = new QPushButton("Logout");

    connect(newGameButton, &QPushButton::clicked, this, &GUIInterface::onNewGameButtonClicked);
    connect(viewHistoryButton, &QPushButton::clicked, this, &GUIInterface::onViewHistoryClicked);
    connect(logoutButton, &QPushButton::clicked, this, &GUIInterface::onLogoutButtonClicked);

    rightLayout->addWidget(scoreBox);
    rightLayout->addWidget(modeBox);
    rightLayout->addWidget(difficultyBox);
    rightLayout->addStretch();
    rightLayout->addWidget(newGameButton);
    rightLayout->addWidget(viewHistoryButton);
    rightLayout->addWidget(logoutButton);

    mainLayout->addLayout(leftLayout, 2); // Game board takes 2/3 of space
    mainLayout->addLayout(rightLayout, 1); // Controls take 1/3 of space

    // --- REPLAY CONTROLS (Initially Hidden) ---
    QHBoxLayout *replayLayout = new QHBoxLayout();
    replayStartButton = new QPushButton("Start Replay");
    replayPrevButton = new QPushButton("<< Prev");
    replayNextButton = new QPushButton("Next >>");
    replayStartButton->setObjectName("replayButton");
    replayPrevButton->setObjectName("replayButton");
    replayNextButton->setObjectName("replayButton");
    replayLayout->addWidget(replayStartButton);
    replayLayout->addStretch();
    replayLayout->addWidget(replayPrevButton);
    replayLayout->addWidget(replayNextButton);
    leftLayout->addLayout(replayLayout);
    connect(replayStartButton, &QPushButton::clicked, this, &GUIInterface::onReplayStartClicked);
    connect(replayPrevButton, &QPushButton::clicked, this, &GUIInterface::onReplayPrevClicked);
    connect(replayNextButton, &QPushButton::clicked, this, &GUIInterface::onReplayNextClicked);

    // --- Create Login and History Widgets ---
    // (This part of your setup code can remain largely the same)
    QWidget *loginScreenWidget = new QWidget;
    // ... build your login UI here ...

    QWidget *historyScreenWidget = new QWidget;
    // ... build your history UI here ...

    mainStack->addWidget(loginScreenWidget); // Index 0
    mainStack->addWidget(gameScreenWidget);  // Index 1
    mainStack->addWidget(historyScreenWidget); // Index 2
}


void GUIInterface::updateBoard(bool isReplay) {
    if (!isReplay) {
        statusLabel->setText(gameLogic.getCurrentPlayer() == Player::X ? "Player X's Turn" : "Player O's Turn");
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Player cell = gameLogic.getCell(i, j);
            QPushButton* button = boardButtons[i][j];

            // Only animate if the text is changing from empty to something new
            if (button->text().isEmpty() && cell != Player::NONE) {
                if (cell == Player::X) {
                    button->setText("X");
                    button->setStyleSheet("color: #3498DB;");
                } else {
                    button->setText("O");
                    button->setStyleSheet("color: #E74C3C;");
                }
                QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(button);
                button->setGraphicsEffect(effect);
                QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
                animation->setDuration(250);
                animation->setStartValue(0.0);
                animation->setEndValue(1.0);
                animation->start(QAbstractAnimation::DeleteWhenStopped);
            } else if (cell == Player::NONE) {
                button->setText("");
                button->setGraphicsEffect(nullptr);
                button->setStyleSheet(""); // Reset stylesheet
            }
        }
    }
}

void GUIInterface::updateScoreDisplay() {
    if (userAuth.isLoggedIn()) {
        const UserProfile* user = userAuth.getCurrentUser();
        QString scoreText = QString("Player X (You): W: %1 | L: %2 | T: %3")
            .arg(user->gamesWon).arg(user->gamesLost).arg(user->gamesTied);
        playerXScoreLabel->setText(scoreText);
    } else {
        playerXScoreLabel->setText("Player X: W: 0 | L: 0 | T: 0");
    }
    // For now, opponent score is static as we don't track non-logged-in users
    playerOScoreLabel->setText("Opponent: W: 0 | L: 0 | T: 0");
}

void GUIInterface::onLoginButtonClicked() {
    // ... your existing login logic ...
    if (userAuth.loginUser(username.toStdString(), password.toStdString())) {
        updateScoreDisplay(); // Update score on login
        switchToGameView();
    } // ...
}

void GUIInterface::handleGameOver(GameResult result) {
    // ... your existing logic to get resultMessage ...

    statusLabel->setText(resultMessage);
    
    // Highlight winning cells
    if (result == GameResult::X_WINS || result == GameResult::O_WINS) {
        auto winningCells = gameLogic.findWinningCombination();
        for (const auto& cell : winningCells) {
            boardButtons[cell.row][cell.col]->setStyleSheet("background-color: #1ABC9C;");
        }
    }

    // Disable board
    for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) boardButtons[i][j]->setEnabled(false);

    if (userAuth.isLoggedIn()) {
        const UserProfile* currentUser = userAuth.getCurrentUser();
        gameHistory.saveGame(currentUser->userId, "Opponent", aiModeRadio->isChecked(), gameLogic.getMoveHistory(), result);
        dbManager.saveGameHistory(gameHistory.getAllGames());
        userAuth.updateUserStats(result);
        dbManager.saveUser(*currentUser);
        updateScoreDisplay(); // Update score after game ends
    }

    QMessageBox::information(this, "Game Over", resultMessage);
}

void GUIInterface::onNewGameButtonClicked() {
    gameLogic.resetBoard();
    setupReplayControls(false); // Hide replay controls
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            boardButtons[i][j]->setEnabled(true);
            boardButtons[i][j]->setStyleSheet(""); // Reset highlight
        }
    }
    updateBoard();
}

void GUIInterface::displayGameForReplay(const GameState& game) {
    replayHistory = game.moveHistory;
    replayMoveIndex = 0;
    
    gameLogic.resetBoard();
    updateBoard(true); // Update board without changing status label
    
    statusLabel->setText("Game Replay: " + QString::fromStdString(game.timestamp));
    setupReplayControls(true); // Show replay controls
    switchToGameView();
}

void GUIInterface::onGameHistoryItemClicked(int row, int column) {
    // ... your logic to get gameId ...
    GameState game = gameHistory.getGameById(gameId.toStdString());
    if (!game.gameId.empty()) {
        displayGameForReplay(game);
    }
}

void GUIInterface::setupReplayControls(bool show) {
    replayStartButton->setVisible(show);
    replayPrevButton->setVisible(show);
    replayNextButton->setVisible(show);
    // Disable main game board buttons during replay
    for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) boardButtons[i][j]->setEnabled(!show);
}

void GUIInterface::onReplayStartClicked() {
    replayMoveIndex = 0;
    gameLogic.resetBoard();
    updateBoard(true);
}

void GUIInterface::onReplayPrevClicked() {
    if (replayMoveIndex > 0) {
        replayMoveIndex--;
        gameLogic.undoLastMove();
        updateBoard(true);
    }
}

void GUIInterface::onReplayNextClicked() {
    if (replayMoveIndex < replayHistory.size()) {
        const auto& move = replayHistory[replayMoveIndex];
        gameLogic.makeMove(move.row, move.col);
        replayMoveIndex++;
        updateBoard(true);
    }
}
