// gui_interface.cpp - Part 4 of 4: Final Implementation and Logic
#include "gui_interface.h"

// All necessary Qt includes
#include <QApplication>
#include <QSettings>
#include <QGridLayout>
#include <QFormLayout>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include <QTime>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QIcon>
#include <QTimer>
#include <QHeaderView>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QComboBox>
#include <QMessageBox>
#include <QSlider>
#include <QCheckBox>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

// Define the time limit for each turn in seconds.
static const int TURN_TIME_LIMIT_S = 15;

// =====================================================================================
// --- Constructor & Destructor
// =====================================================================================

GUIInterface::GUIInterface(const std::string& dbPath, QWidget *parent)
    : QMainWindow(parent),
      dbManager(dbPath),
      auth(),
      gameLogic(),
      aiEngine(),
      gameHistory(),
      currentTheme(DARK),
      animationsEnabled(true),
      animationSpeed(300),
      isGameInProgress(false),
      isReplayMode(false),
      gameTimeSeconds(0),
      replayMoveIndex(0),
      replayAutoMode(false) {

    gameTimer = new QTimer(this);
    replayAutoTimer = new QTimer(this);

    connect(gameTimer, &QTimer::timeout, this, &GUIInterface::onGameTimerUpdate);
    connect(replayAutoTimer, &QTimer::timeout, this, &GUIInterface::onReplayAutoStep);

    auth.setUsers(dbManager.loadUsers());
    gameHistory.loadFromDatabase(dbManager);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout* rootLayout = new QHBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);
    centralWidget->setObjectName("centralWidget");
    
    createNavigationBar(rootLayout);

    stackedWidget = new QStackedWidget(this);
    rootLayout->addWidget(stackedWidget);

    createLoginView();
    createGameSetupView();
    createGameView();
    createHistoryView();
    createStatsView();
    createSettingsView();

    switchToLoginView();
    updateNavigationButtons();

    setWindowTitle("Advanced Tic-Tac-Toe");
    setMinimumSize(900, 700);
    
    loadSettings();
    applyTheme(currentTheme);
}

GUIInterface::~GUIInterface() {
    saveSettings();
}

// =====================================================================================
// --- UI Creation and Styling
// =====================================================================================

void GUIInterface::applyTheme(Theme theme) {
    currentTheme = theme;
    QString styleSheet;

    if (theme == DARK) {
        styleSheet = R"(
            /* General */
            QMainWindow, QWidget {
                font-family: 'Segoe UI', sans-serif;
            }
            QWidget#centralWidget {
                background-color: #1e1e1e;
            }
            QWidget#navBar {
                background-color: #2c3e50;
            }
            QWidget#loginFormContainer, QGroupBox {
                background-color: rgba(44, 62, 80, 0.8);
                border-radius: 15px;
                padding: 20px;
            }
            QGroupBox {
                font-size: 16px;
                font-weight: bold;
                color: #ffffff;
            }

            /* Labels */
            QLabel {
                color: #ffffff;
                font-size: 14px;
            }
            QLabel#titleLabel {
                font-size: 48px;
                font-weight: bold;
                color: #ffffff;
                qproperty-alignment: 'AlignCenter';
            }
            QLabel#statusLabel {
                font-size: 20px;
                font-weight: bold;
                color: #3498db;
            }
            QLabel#scoreLabel, QLabel.statValue {
                font-size: 18px;
                color: #ecf0f1;
                font-weight: bold;
            }
            QLabel.statLabel {
                font-size: 14px;
                color: #bdc3c7;
            }
            QLabel#errorLabel {
                color: #e74c3c;
                font-size: 12px;
                qproperty-alignment: 'AlignCenter';
            }

            /* Buttons */
            QPushButton, QToolButton {
                background-color: #3498db;
                color: #ffffff;
                border: none;
                padding: 12px 24px;
                font-size: 16px;
                border-radius: 8px;
                min-width: 120px;
            }
            QPushButton:hover, QToolButton:hover {
                background-color: #5dade2;
            }
            QPushButton:pressed, QToolButton:pressed {
                background-color: #217dbb;
            }
            QPushButton:disabled {
                background-color: #566573;
            }
            QPushButton.navButton {
                background-color: transparent;
                border: none;
                color: #bdc3c7;
                font-size: 14px;
                text-align: left;
                padding: 15px;
                border-left: 3px solid transparent;
            }
            QPushButton.navButton:checked, QPushButton.navButton:hover {
                background-color: #34495e;
                color: #ffffff;
                border-left: 3px solid #3498db;
            }
            QPushButton.game-cell {
                font-size: 48px;
                font-weight: bold;
                background-color: rgba(255, 255, 255, 0.1);
                border-radius: 12px;
                color: #ffffff;
            }
            QPushButton.game-cell:hover {
                background-color: rgba(255, 255, 255, 0.2);
            }
            QPushButton.X { color: #3498db; }
            QPushButton.O { color: #e74c3c; }
            QPushButton.winning-cell { border: 4px solid #f1c40f; }

            /* Line Edits, Combo Boxes, Sliders */
            QLineEdit, QComboBox, QSlider {
                background-color: #2c3e50;
                color: #ffffff;
                border: 1px solid #34495e;
                padding: 10px;
                border-radius: 8px;
                font-size: 14px;
            }
            QLineEdit:focus, QComboBox:focus {
                border: 1px solid #3498db;
            }
            QComboBox::drop-down { border: none; }
            QComboBox::down-arrow { image: url(:/icons/down_arrow.png); }
            QSlider::groove:horizontal {
                border: 1px solid #34495e;
                height: 8px;
                background: #2c3e50;
                margin: 2px 0;
                border-radius: 4px;
            }
            QSlider::handle:horizontal {
                background: #3498db;
                border: 1px solid #3498db;
                width: 18px;
                margin: -2px 0;
                border-radius: 9px;
            }

            /* Radio & Check Boxes */
            QRadioButton, QCheckBox { color: #ffffff; spacing: 10px; }
            QRadioButton::indicator, QCheckBox::indicator { width: 20px; height: 20px; }
            QRadioButton::indicator::unchecked { image: url(:/icons/radio_unchecked.png); }
            QRadioButton::indicator::checked { image: url(:/icons/radio_checked.png); }
            QCheckBox::indicator::unchecked { image: url(:/icons/check_unchecked.png); }
            QCheckBox::indicator::checked { image: url(:/icons/check_checked.png); }

            /* Tables */
            QTableWidget {
                background-color: #2c3e50;
                color: #ecf0f1;
                border: none;
                gridline-color: #34495e;
                selection-background-color: #3498db;
            }
            QHeaderView::section {
                background-color: #34495e;
                color: #ffffff;
                padding: 10px;
                border: none;
                font-weight: bold;
            }
        )";
    } else { // LIGHT THEME
        // Styles for the light theme would go here
    }

    qApp->setStyleSheet(styleSheet);
}

// ... (createNavigationBar, createLoginView, createGameSetupView, createGameView remain the same as Part 3)

void GUIInterface::createHistoryView() {
    historyView = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(historyView);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    QLabel* title = new QLabel("Game History");
    title->setObjectName("titleLabel");
    title->setStyleSheet("font-size: 32px;");
    layout->addWidget(title, 0, Qt::AlignLeft);

    historyTable = new QTableWidget();
    historyTable->setColumnCount(4);
    historyTable->setHorizontalHeaderLabels({"Date", "Opponent", "Result", "Game ID"});
    historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    historyTable->verticalHeader()->setVisible(false);
    layout->addWidget(historyTable);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    replayButton = new QPushButton("Replay Selected Game");
    exportButton = new QPushButton("Export History");
    buttonLayout->addStretch();
    buttonLayout->addWidget(replayButton);
    buttonLayout->addWidget(exportButton);
    layout->addLayout(buttonLayout);

    connect(replayButton, &QPushButton::clicked, this, &GUIInterface::onReplayGameClicked);
    connect(exportButton, &QPushButton::clicked, this, &GUIInterface::onExportHistoryClicked);

    stackedWidget->addWidget(historyView);
}

void GUIInterface::createStatsView() {
    statsView = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(statsView);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(50, 50, 50, 50);

    QLabel* title = new QLabel("Your Statistics");
    title->setObjectName("titleLabel");
    layout->addWidget(title);
    layout->addSpacing(30);

    QGroupBox* statsGroup = new QGroupBox();
    statsGroup->setFixedWidth(500);
    QGridLayout* grid = new QGridLayout(statsGroup);
    grid->setSpacing(20);

    // Labels
    grid->addWidget(new QLabel("Games Played:", statsGroup), 0, 0);
    grid->addWidget(new QLabel("Games Won:", statsGroup), 1, 0);
    grid->addWidget(new QLabel("Games Lost:", statsGroup), 2, 0);
    grid->addWidget(new QLabel("Games Tied:", statsGroup), 3, 0);
    grid->addWidget(new QLabel("Win Rate:", statsGroup), 4, 0);

    // Values
    gamesPlayedLabel = new QLabel("0", statsGroup);
    gamesWonLabel = new QLabel("0", statsGroup);
    gamesLostLabel = new QLabel("0", statsGroup);
    gamesTiedLabel = new QLabel("0", statsGroup);
    winRateLabel = new QLabel("0%", statsGroup);
    
    QLabel* valueLabels[] = {gamesPlayedLabel, gamesWonLabel, gamesLostLabel, gamesTiedLabel, winRateLabel};
    for(int i=0; i < 5; ++i) {
        valueLabels[i]->setClassName("statValue");
        grid->addWidget(valueLabels[i], i, 1, Qt::AlignRight);
    }
    
    layout->addWidget(statsGroup);
    layout->addStretch();
    
    stackedWidget->addWidget(statsView);
}

void GUIInterface::createSettingsView() {
    settingsView = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(settingsView);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(25);
    layout->setContentsMargins(50, 50, 50, 50);

    QLabel* title = new QLabel("Settings");
    title->setObjectName("titleLabel");
    layout->addWidget(title, 0, Qt::AlignCenter);
    
    QGroupBox* settingsGroup = new QGroupBox();
    settingsGroup->setFixedWidth(500);
    QFormLayout* form = new QFormLayout(settingsGroup);
    form->setSpacing(20);
    
    // Theme
    themeCombo = new QComboBox();
    themeCombo->addItem("Dark Theme");
    themeCombo->addItem("Light Theme");
    form->addRow("Theme:", themeCombo);

    // Animations
    animationsCheckbox = new QCheckBox("Enable Animations");
    form->addRow(animationsCheckbox);
    
    // Animation Speed
    animationSpeedSlider = new QSlider(Qt::Horizontal);
    animationSpeedSlider->setRange(100, 1000); // 100ms to 1s
    animationSpeedSlider->setSingleStep(50);
    form->addRow("Animation Speed:", animationSpeedSlider);

    layout->addWidget(settingsGroup);
    layout->addStretch();
    
    connect(themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &GUIInterface::onSettingsChanged);
    connect(animationsCheckbox, &QCheckBox::stateChanged, this, &GUIInterface::onSettingsChanged);
    connect(animationSpeedSlider, &QSlider::valueChanged, this, &GUIInterface::onSettingsChanged);
    
    stackedWidget->addWidget(settingsView);
}


// =====================================================================================
// --- Slot Implementations (Event Handlers)
// =====================================================================================

void GUIInterface::onLoginClicked() {
    if (auth.loginUser(usernameLineEdit->text().toStdString(), passwordLineEdit->text().toStdString())) {
        switchToGameSetupView();
        updateGameStats();
        loadUserGames();
    } else {
        errorLabel->setText("Invalid username or password.");
    }
}

void GUIInterface::onRegisterClicked() {
    if (auth.registerUser(usernameLineEdit->text().toStdString(), passwordLineEdit->text().toStdString())) {
        dbManager.saveUsers(auth.getUsers());
        errorLabel->setText("Registration successful! Please login.");
    } else {
        errorLabel->setText("Username already exists.");
    }
}

void GUIInterface::onLogoutClicked() {
    auth.logoutUser();
    switchToLoginView();
}

void GUIInterface::onStartGameClicked() {
    gameLogic.resetBoard();
    isGameInProgress = true;
    updateBoard();
    updateScoreDisplay(); // Reset score display for new match
    gameTimeSeconds = 0;
    gameTimer->start(1000);
    switchToGameView();
}

void GUIInterface::onNewGameClicked() {
    if (isGameInProgress) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "New Game", "Are you sure you want to abandon the current game?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
    }
    isGameInProgress = false;
    gameTimer->stop();
    switchToGameSetupView();
}

void GUIInterface::onCellClicked(int row, int col) {
    if (!isGameInProgress || !gameLogic.isValidMove(row, col) || isReplayMode) return;

    if (gameLogic.makeMove(row, col)) {
        animateCellPlacement(row, col, gameLogic.getCell(row, col));
        updateBoard();

        GameResult result = gameLogic.checkGameResult();
        if (result != GameResult::IN_PROGRESS) {
            handleGameOver(result);
        } else {
            statusLabel->setText(QString("%1's Turn").arg(getPlayerName(gameLogic.getCurrentPlayer())));
            if (vsAIRadio->isChecked() && gameLogic.getCurrentPlayer() == Player::O) {
                // Add a small delay for a more natural feel
                QTimer::singleShot(500, this, &GUIInterface::makeAIMove);
            }
        }
    }
}

void GUIInterface::handleGameOver(GameResult result) {
    isGameInProgress = false;
    gameTimer->stop();
    auth.updateUserStats(result);
    dbManager.saveUsers(auth.getUsers());
    
    std::string player1Id = auth.getCurrentUser() ? auth.getCurrentUser()->userId : "Guest";
    std::string player2Id = vsAIRadio->isChecked() ? "AI" : "Guest2";
    
    gameHistory.saveGame(player1Id, player2Id, vsAIRadio->isChecked(), gameLogic.getMoveHistory(), result);
    dbManager.saveGameHistory(gameHistory.getAllGames());

    updateGameStats();
    loadUserGames();
    
    highlightWinningCells(gameLogic.findWinningCombination());
    animateGameOver(result);
}

void GUIInterface::makeAIMove() {
    if (!isGameInProgress) return;
    AIEngine::Difficulty difficulty = static_cast<AIEngine::Difficulty>(difficultyCombo->currentIndex());
    Move aiMove = aiEngine.findBestMove(gameLogic, difficulty);
    if (gameLogic.makeMove(aiMove.row, aiMove.col)) {
        animateCellPlacement(aiMove.row, aiMove.col, gameLogic.getCell(aiMove.row, aiMove.col));
        updateBoard();

        GameResult result = gameLogic.checkGameResult();
        if (result != GameResult::IN_PROGRESS) {
            handleGameOver(result);
        } else {
            statusLabel->setText(QString("%1's Turn").arg(getPlayerName(gameLogic.getCurrentPlayer())));
        }
    }
}

// ... other slots for settings, history, etc.

// =====================================================================================
// --- UI Update and Helper Functions
// =====================================================================================

void GUIInterface::updateBoard() {
    resetBoardHighlights();
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QPushButton* button = gameBoardButtons[i][j];
            Player player = gameLogic.getCell(i, j);
            button->setText(getPlayerName(player));
            button->setProperty("class", getPlayerName(player)); // For X/O color styling
            style()->unpolish(button);
            style()->polish(button);
        }
    }
}

void GUIInterface::updateGameStats() {
    if (auth.isLoggedIn()) {
        const UserProfile* user = auth.getCurrentUser();
        gamesPlayedLabel->setText(QString::number(user->gamesPlayed));
        gamesWonLabel->setText(QString::number(user->gamesWon));
        gamesLostLabel->setText(QString::number(user->gamesLost));
        gamesTiedLabel->setText(QString::number(user->gamesTied));
        if (user->gamesPlayed > 0) {
            double winRate = (double)user->gamesWon / (user->gamesPlayed - user->gamesTied) * 100.0;
            winRateLabel->setText(QString::number(winRate, 'f', 1) + "%");
        } else {
            winRateLabel->setText("N/A");
        }
    }
}

void GUIInterface::loadUserGames() {
    historyTable->setRowCount(0);
    if (!auth.isLoggedIn()) return;
    
    auto games = gameHistory.getUserGames(auth.getCurrentUser()->userId);
    for (const auto& game : games) {
        int row = historyTable->rowCount();
        historyTable->insertRow(row);
        historyTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(game.timestamp)));
        historyTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(game.player2Id)));
        historyTable->setItem(row, 2, new QTableWidgetItem(formatGameResult(game.result)));
        historyTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(game.gameId)));
    }
}

void GUIInterface::highlightWinningCells(const std::vector<Move>& cells) {
    for (const auto& move : cells) {
        gameBoardButtons[move.row][move.col]->setClassName("winning-cell");
        style()->unpolish(gameBoardButtons[move.row][move.col]);
        style()->polish(gameBoardButtons[move.row][move.col]);
    }
}

void GUIInterface::resetBoardHighlights() {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            gameBoardButtons[i][j]->setClassName("game-cell");
        }
    }
}

void GUIInterface::animateCellPlacement(int row, int col, Player player) {
    if (!animationsEnabled) return;
    
    QPushButton* button = gameBoardButtons[row][col];
    QPropertyAnimation* anim = new QPropertyAnimation(button, "geometry");
    QRect startGeom = button->geometry();
    anim->setDuration(animationSpeed / 2);
    anim->setStartValue(QRect(startGeom.center(), QSize(0,0)));
    anim->setEndValue(startGeom);
    anim->setEasingCurve(QEasingCurve::OutElastic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void GUIInterface::animateGameOver(GameResult result) {
    QString message = formatGameResult(result);
    statusLabel->setText(message);
    if(animationsEnabled) {
        // Simple animation for the status label
        QPropertyAnimation* anim = new QPropertyAnimation(statusLabel, "geometry");
        QRect startGeom = statusLabel->geometry();
        anim->setDuration(animationSpeed);
        anim->setStartValue(startGeom.translated(0, 10));
        anim->setEndValue(startGeom);
        anim->setEasingCurve(QEasingCurve::OutBounce);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

QString GUIInterface::formatGameResult(GameResult result) {
    switch(result) {
        case GameResult::X_WINS: return auth.getCurrentUser()->username + " Wins!";
        case GameResult::O_WINS: return (vsAIRadio->isChecked() ? "AI" : "Player 2") + " Wins!";
        case GameResult::DRAW: return "It's a Draw!";
        default: return "In Progress";
    }
}

QString GUIInterface::getPlayerName(Player player) {
    if (player == Player::X) return "X";
    if (player == Player::O) return "O";
    return "";
}


// --- View Switching ---
void GUIInterface::switchToLoginView() { stackedWidget->setCurrentWidget(loginView); updateNavigationButtons(false); }
void GUIInterface::switchToGameSetupView() { stackedWidget->setCurrentWidget(gameSetupView); gameNavButton->setChecked(true); updateNavigationButtons(true); }
void GUIInterface::switchToGameView() { /* Logic handled by onStartGameClicked */ }
void GUIInterface::switchToHistoryView() { stackedWidget->setCurrentWidget(historyView); updateNavigationButtons(true); historyNavButton->setChecked(true); }
void GUIInterface::switchToStatsView() { stackedWidget->setCurrentWidget(statsView); updateNavigationButtons(true); statsNavButton->setChecked(true); }
void GUIInterface::switchToSettingsView() { stackedWidget->setCurrentWidget(settingsView); updateNavigationButtons(true); settingsNavButton->setChecked(true); }


void GUIInterface::updateNavigationButtons(bool loggedIn) {
    gameNavButton->setVisible(loggedIn);
    historyNavButton->setVisible(loggedIn);
    statsNavButton->setVisible(loggedIn);
    settingsNavButton->setVisible(loggedIn);
    logoutButton->setVisible(loggedIn);
}

// ... (Other helper functions like load/save settings, etc.)