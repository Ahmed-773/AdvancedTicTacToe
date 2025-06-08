// gui_interface.cpp
#include "gui_interface.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFont>

GUIInterface::GUIInterface(const std::string& dbPath,QWidget *parent) : QMainWindow(parent), dbManager(dbPath) {
    setupUI();

    auto loadedUsers = dbManager.loadUsers();
    userAuth.setUsers(loadedUsers);
    gameHistory.loadFromDatabase(dbManager);
    // Initialize components
    aiEngine.setDifficulty(2);  // Default medium difficulty
    
    // Load saved data
    auto users = dbManager.loadUsers();
    auto games = dbManager.loadGameHistory();
    
    // Show login screen initially
    switchToLoginView();
}

GUIInterface::~GUIInterface() {
    // Save any unsaved data before exit
}

void GUIInterface::setupUI() {
    setWindowTitle("Advanced Tic Tac Toe");
    resize(800, 600);
    
    mainStack = new QStackedWidget(this);
    setCentralWidget(mainStack);
    
    setupAuthentication();
    setupGameBoard();
    setupHistoryView();
}

void GUIInterface::setupAuthentication() {
    loginWidget = new QWidget();
    QVBoxLayout *loginLayout = new QVBoxLayout(loginWidget);
    
    QLabel *titleLabel = new QLabel("Advanced Tic Tac Toe");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    
    QGroupBox *authBox = new QGroupBox("User Authentication");
    QVBoxLayout *authLayout = new QVBoxLayout(authBox);
    
    QLabel *usernameLabel = new QLabel("Username:");
    usernameInput = new QLineEdit();
    
    QLabel *passwordLabel = new QLabel("Password:");
    passwordInput = new QLineEdit();
    passwordInput->setEchoMode(QLineEdit::Password);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    loginButton = new QPushButton("Login");
    registerButton = new QPushButton("Register");
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);
    
    authLayout->addWidget(usernameLabel);
    authLayout->addWidget(usernameInput);
    authLayout->addWidget(passwordLabel);
    authLayout->addWidget(passwordInput);
    authLayout->addLayout(buttonLayout);
    
    loginLayout->addWidget(titleLabel);
    loginLayout->addWidget(authBox);
    
    // Connect signals
    connect(loginButton, &QPushButton::clicked, this, &GUIInterface::onLoginButtonClicked);
    connect(registerButton, &QPushButton::clicked, this, &GUIInterface::onRegisterButtonClicked);
    
    mainStack->addWidget(loginWidget);
}

void GUIInterface::setupGameBoard() {
    gameWidget = new QWidget();
    QVBoxLayout *gameLayout = new QVBoxLayout(gameWidget);
    
    // Game status display
    statusLabel = new QLabel("Player X's Turn");
    QFont statusFont = statusLabel->font();
    statusFont.setPointSize(16);
    statusFont.setBold(true);
    statusLabel->setFont(statusFont);
    statusLabel->setAlignment(Qt::AlignCenter);
    
    // Game board
    QGroupBox *boardBox = new QGroupBox("Game Board");
    boardLayout = new QGridLayout(boardBox);
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            boardButtons[i][j] = new QPushButton("");
            boardButtons[i][j]->setFixedSize(100, 100);
            QFont buttonFont = boardButtons[i][j]->font();
            buttonFont.setPointSize(24);
            buttonFont.setBold(true);
            boardButtons[i][j]->setFont(buttonFont);
            boardLayout->addWidget(boardButtons[i][j], i, j);
            
            // Store row and column as property to identify which button was clicked
            boardButtons[i][j]->setProperty("row", i);
            boardButtons[i][j]->setProperty("col", j);
            
            connect(boardButtons[i][j], &QPushButton::clicked, this, &GUIInterface::onCellClicked);
        }
    }
    
    // Game controls
    QGroupBox *controlBox = new QGroupBox("Game Controls");
    QVBoxLayout *controlLayout = new QVBoxLayout(controlBox);
    
    // Game mode selection
    QGroupBox *modeBox = new QGroupBox("Game Mode");
    QHBoxLayout *modeLayout = new QHBoxLayout(modeBox);
    pvpModeRadio = new QRadioButton("Player vs Player");
    aiModeRadio = new QRadioButton("Player vs AI");
    modeLayout->addWidget(pvpModeRadio);
    modeLayout->addWidget(aiModeRadio);
    
    // AI difficulty selection
    QGroupBox *difficultyBox = new QGroupBox("AI Difficulty");
    QHBoxLayout *difficultyLayout = new QHBoxLayout(difficultyBox);
    difficultyCombo = new QComboBox();
    difficultyCombo->addItem("Easy", 1);
    difficultyCombo->addItem("Medium", 2);
    difficultyCombo->addItem("Hard", 3);
    difficultyCombo->setCurrentIndex(1);  // Medium by default
    difficultyLayout->addWidget(difficultyCombo);
    
    // Control buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    newGameButton = new QPushButton("New Game");
    viewHistoryButton = new QPushButton("View History");
    logoutButton = new QPushButton("Logout");
    buttonLayout->addWidget(newGameButton);
    buttonLayout->addWidget(viewHistoryButton);
    buttonLayout->addWidget(logoutButton);
    
    controlLayout->addWidget(modeBox);
    controlLayout->addWidget(difficultyBox);
    controlLayout->addLayout(buttonLayout);
    
    gameLayout->addWidget(statusLabel);
    gameLayout->addWidget(boardBox);
    gameLayout->addWidget(controlBox);
    
    // Set default game mode
    pvpModeRadio->setChecked(true);
    
    // Connect signals
    connect(newGameButton, &QPushButton::clicked, this, &GUIInterface::onNewGameButtonClicked);
    connect(viewHistoryButton, &QPushButton::clicked, this, &GUIInterface::onViewHistoryClicked);
    connect(logoutButton, &QPushButton::clicked, this, &GUIInterface::onLogoutButtonClicked);
    connect(pvpModeRadio, &QRadioButton::toggled, this, &GUIInterface::onGameModeChanged);
    connect(difficultyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &GUIInterface::onDifficultyChanged);
    
    mainStack->addWidget(gameWidget);
}

void GUIInterface::setupHistoryView() {
    historyWidget = new QWidget();
    QVBoxLayout *historyLayout = new QVBoxLayout(historyWidget);
    
    QLabel *historyTitle = new QLabel("Game History");
    QFont titleFont = historyTitle->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    historyTitle->setFont(titleFont);
    historyTitle->setAlignment(Qt::AlignCenter);
    
    gameHistoryTable = new QTableWidget(0, 4);  // Rows will be added dynamically
    gameHistoryTable->setHorizontalHeaderLabels({"Date", "Opponent", "Result", "Mode"});
    gameHistoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    backToGameButton = new QPushButton("Back to Game");
    
    historyLayout->addWidget(historyTitle);
    historyLayout->addWidget(gameHistoryTable);
    historyLayout->addWidget(backToGameButton);
    
    // Connect signals
    connect(backToGameButton, &QPushButton::clicked, this, &GUIInterface::onBackToGameClicked);
    connect(gameHistoryTable, &QTableWidget::cellClicked, 
            this, &GUIInterface::onGameHistoryItemClicked);
    
    mainStack->addWidget(historyWidget);
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
        QMessageBox::information(this, "Registration Successful", 
                                "User registered successfully. You can now login.");
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
    if (!button) return;
    
    int row = button->property("row").toInt();
    int col = button->property("col").toInt();
    
    // Make player move
    if (gameLogic.makeMove(row, col)) {
        updateBoard();
        
        // Check game result after player move
        GameResult result = gameLogic.checkGameResult();
        if (result != GameResult::IN_PROGRESS) {
            handleGameOver(result);
            return;
        }
        
        // Make AI move if in AI mode
        if (isAIMode() && gameLogic.getCurrentPlayer() == Player::O) {
            makeAIMove();
            
            // Check game result after AI move
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
}

void GUIInterface::onGameModeChanged() {
    // If switching to AI mode during a game in progress, reset the game
    if (isAIMode() && gameLogic.checkGameResult() == GameResult::IN_PROGRESS) {
        gameLogic.resetBoard();
        updateBoard();
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
    // Get the game ID from the hidden data
    QString gameId = gameHistoryTable->item(row, 0)->data(Qt::UserRole).toString();
    
    // Get the game from history
    GameState game = gameHistory.getGameById(gameId.toStdString());
    
    // Display the game
    displayGame(game);
}

void GUIInterface::onBackToGameClicked() {
    switchToGameView();
}

void GUIInterface::updateBoard() {
    // Update the status label
    if (gameLogic.getCurrentPlayer() == Player::X) {
        statusLabel->setText("Player X's Turn");
    } else {
        statusLabel->setText("Player O's Turn");
    }
    
    // Update the board buttons
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Player cell = gameLogic.getCell(i, j);
            
            if (cell == Player::X) {
                boardButtons[i][j]->setText("X");
                boardButtons[i][j]->setStyleSheet("color: blue;");
            } else if (cell == Player::O) {
                boardButtons[i][j]->setText("O");
                boardButtons[i][j]->setStyleSheet("color: red;");
            } else {
                boardButtons[i][j]->setText("");
                boardButtons[i][j]->setStyleSheet("");
            }
        }
    }
}

void GUIInterface::handleGameOver(GameResult result) {
    QString resultMessage;
    
    switch (result) {
        case GameResult::X_WINS:
            resultMessage = "Player X wins!";
            break;
        case GameResult::O_WINS:
            resultMessage = "Player O wins!";
            break;
        case GameResult::DRAW:
            resultMessage = "Game ended in a draw!";
            break;
        default:
            return;
    }
    
    // Update the status label
    statusLabel->setText(resultMessage);
    
    // Save game to history if user is logged in
    if (userAuth.isLoggedIn()) {
        const UserProfile* currentUser = userAuth.getCurrentUser();
        std::string player1Id = currentUser->userId;
        std::string player2Id = isAIMode() ? "AI" : "Player2";
        
        gameHistory.saveGame(player1Id, player2Id, isAIMode(), 
                            gameLogic.getMoveHistory(), result,dbManager);
        
        // Update user stats
        userAuth.updateUserStats(result);
    }
    
    // Show game over message
    QMessageBox::information(this, "Game Over", resultMessage);
}

void GUIInterface::switchToLoginView() {
    mainStack->setCurrentWidget(loginWidget);
    usernameInput->clear();
    passwordInput->clear();
}

void GUIInterface::switchToGameView() {
    mainStack->setCurrentWidget(gameWidget);
    gameLogic.resetBoard();
    updateBoard();
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
    
    // Clear table
    gameHistoryTable->setRowCount(0);
    
    // Populate table with game history
    for (const auto& game : userGames) {
        int row = gameHistoryTable->rowCount();
        gameHistoryTable->insertRow(row);
        
        // Store game ID as hidden data
        QTableWidgetItem* dateItem = new QTableWidgetItem(QString::fromStdString(game.timestamp));
        dateItem->setData(Qt::UserRole, QString::fromStdString(game.gameId));
        gameHistoryTable->setItem(row, 0, dateItem);
        
        // Opponent
        QString opponent = game.isAIOpponent ? 
                          "AI" : QString::fromStdString(game.player2Id);
        gameHistoryTable->setItem(row, 1, new QTableWidgetItem(opponent));
        
        // Result
        QString result;
        switch (game.result) {
            case GameResult::X_WINS:
                result = "X Wins";
                break;
            case GameResult::O_WINS:
                result = "O Wins";
                break;
            case GameResult::DRAW:
                result = "Draw";
                break;
            default:
                result = "Unknown";
        }
        gameHistoryTable->setItem(row, 2, new QTableWidgetItem(result));
        
        // Mode
        QString mode = game.isAIOpponent ? "vs AI" : "vs Player";
        gameHistoryTable->setItem(row, 3, new QTableWidgetItem(mode));
    }
}

void GUIInterface::displayGame(const GameState& game) {
    // Load the game from history to the current game state
    gameLogic = gameHistory.replayGame(game.gameId);
    
    // Switch back to game view to show the replayed game
    switchToGameView();
    
    // Update status to show this is a replay
    statusLabel->setText("Game Replay: " + QString::fromStdString(game.timestamp));
    
    // Disable board buttons during replay
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            boardButtons[i][j]->setEnabled(false);
        }
    }
}
