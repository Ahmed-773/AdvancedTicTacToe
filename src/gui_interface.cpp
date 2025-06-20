// // gui_interface.cpp
// #include "gui_interface.h"
// #include <QApplication>
// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QGroupBox>
// #include <QHeaderView>
// #include <QFont>
// #include <QMessageBox>
// #include <QPropertyAnimation>
// #include <QGraphicsOpacityEffect>
// #include <QDebug> // For debugging output
// #include <QIcon> // For button icons

// GUIInterface::GUIInterface(const std::string& dbPath, QWidget *parent)
//     : QMainWindow(parent), dbManager(dbPath), replayMoveIndex(0) {

//     applyStylesheet();
//     setupUI();

//     try {
//         auto loadedUsers = dbManager.loadUsers();
//         userAuth.setUsers(loadedUsers);
//         gameHistory.loadFromDatabase(dbManager);
//     } catch (const std::exception& e) {
//         qWarning() << "Could not load initial data: " << e.what();
//     }

//     aiEngine.setDifficulty(3);
//     switchToLoginView();
// }

// GUIInterface::~GUIInterface() {}

// void GUIInterface::applyStylesheet() {
//     QString style = R"(
//         QMainWindow, QWidget {
//             background-color: #2C3E50;
//             font-family: 'Segoe UI', sans-serif;
//             color: #ECF0F1;
//         }
//         QStackedWidget { background-color: transparent; }
//         QGroupBox {
//             background-color: #34495E;
//             border: 1px solid #4A6572;
//             border-radius: 8px;
//             margin-top: 1ex;
//             font-size: 14px;
//             font-weight: bold;
//         }
//         QGroupBox::title {
//             subcontrol-origin: margin;
//             subcontrol-position: top center;
//             padding: 0 10px;
//             color: #1ABC9C;
//         }
//         QLabel { font-size: 14px; color: #BDC3C7; }
//         QLabel#titleLabel, QLabel#historyTitleLabel {
//             font-size: 32px; font-weight: bold; color: #FFFFFF; padding: 20px;
//         }
//         QLabel#statusLabel {
//             font-size: 22px; font-weight: bold; color: #FFFFFF; padding: 12px;
//             background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #16A085, stop:1 #1ABC9C);
//             border-radius: 8px;
//             min-height: 40px;
//         }
//         QLabel#scoreLabel { font-size: 16px; font-weight: bold; color: #ECF0F1; }
//         QLineEdit {
//             border: 1px solid #4A6572; padding: 10px; border-radius: 5px;
//             font-size: 14px; background-color: #2C3E50; color: #ECF0F1;
//         }
//         QLineEdit:focus { border: 1px solid #1ABC9C; }
//         QPushButton {
//             background-color: #1ABC9C; color: white; border: none;
//             padding: 12px 20px; font-size: 14px; font-weight: bold;
//             border-radius: 5px; min-width: 100px;
//         }
//         QPushButton:hover { background-color: #16A085; }
//         QPushButton:pressed { background-color: #117A65; }
//         QPushButton:disabled { background-color: #566573; color: #95A5A6; }
//         QPushButton#gameBoardButton {
//             font-size: 52px; font-weight: bold; background-color: #34495E;
//             border: 2px solid #4A6572; border-radius: 8px;
//         }
//         QPushButton#gameBoardButton:hover { border-color: #1ABC9C; }
//         QPushButton#replayButton {
//              background-color: #3498DB; min-width: 40px;
//         }
//         QPushButton#replayButton:hover { background-color: #2980B9; }
//         QTableWidget {
//             border: 1px solid #4A6572; border-radius: 8px;
//             background-color: #34495E; gridline-color: #4A6572;
//             alternate-background-color: #3A5064;
//         }
//         QHeaderView::section {
//             background-color: #1ABC9C; color: white; padding: 8px;
//             font-size: 14px; font-weight: bold; border: none;
//         }
//         QRadioButton::indicator { width: 15px; height: 15px; }
//         QComboBox {
//             border: 1px solid #4A6572; border-radius: 5px; padding: 8px;
//             background-color: #34495E; color: #ECF0F1;
//         }
//         QComboBox::drop-down { border: none; }
//         QComboBox QAbstractItemView {
//             border: 1px solid #4A6572; background-color: #34495E;
//             selection-background-color: #1ABC9C; color: #ECF0F1;
//         }
//     )";
//     qApp->setStyleSheet(style);
// }

// void GUIInterface::setupUI() {
//     setWindowTitle("Advanced Tic Tac Toe");
//     resize(900, 750);
//     setMinimumSize(800, 700);

//     mainStack = new QStackedWidget(this);
//     setCentralWidget(mainStack);

//     setupAuthentication();
//     setupGameBoard();
//     setupHistoryView();
// }

// void GUIInterface::setupAuthentication() {
//     loginWidget = new QWidget();
//     QVBoxLayout *loginLayout = new QVBoxLayout(loginWidget);
//     loginLayout->setAlignment(Qt::AlignCenter);
//     loginLayout->setSpacing(20);

//     QLabel *titleLabel = new QLabel("Advanced Tic Tac Toe");
//     titleLabel->setObjectName("titleLabel");
//     titleLabel->setAlignment(Qt::AlignCenter);

//     usernameInput = new QLineEdit();
//     usernameInput->setPlaceholderText("Enter your username");
//     passwordInput = new QLineEdit();
//     passwordInput->setPlaceholderText("Enter your password");
//     passwordInput->setEchoMode(QLineEdit::Password);

//     authLayout->addWidget(new QLabel("Username:"), 0, 0);
//     authLayout->addWidget(usernameInput, 0, 1);
//     authLayout->addWidget(new QLabel("Password:"), 1, 0);
//     authLayout->addWidget(passwordInput, 1, 1);

//     QHBoxLayout *buttonLayout = new QHBoxLayout();
//     QPushButton* loginButton = new QPushButton("Login");
//     QPushButton* registerButton = new QPushButton("Register");
//     buttonLayout->addWidget(loginButton);
//     buttonLayout->addWidget(registerButton);
//     authLayout->addLayout(buttonLayout, 2, 0, 1, 2);

//     loginLayout->addWidget(titleLabel);
//     loginLayout->addWidget(authBox, 0, Qt::AlignHCenter);

//     connect(loginButton, &QPushButton::clicked, this, &GUIInterface::onLoginButtonClicked);
//     connect(registerButton, &QPushButton::clicked, this, &GUIInterface::onRegisterButtonClicked);

//     mainStack->addWidget(loginWidget);
// }

// void GUIInterface::setupGameBoard() {
//     gameWidget = new QWidget();
//     QVBoxLayout *mainGameLayout = new QVBoxLayout(gameWidget);
//     mainGameLayout->setSpacing(20);
//     mainGameLayout->setContentsMargins(20, 20, 20, 20);

//     statusLabel = new QLabel("Welcome!");
//     statusLabel->setObjectName("statusLabel");
//     statusLabel->setAlignment(Qt::AlignCenter);

//     QGridLayout* boardLayout = new QGridLayout();
//     boardLayout->setSpacing(10);
//     for (int i = 0; i < 3; ++i) {
//         for (int j = 0; j < 3; ++j) {
//             boardButtons[i][j] = new QPushButton("");
//             boardButtons[i][j]->setObjectName("gameBoardButton");
//             boardButtons[i][j]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//             boardButtons[i][j]->setProperty("row", i);
//             boardButtons[i][j]->setProperty("col", j);
//             connect(boardButtons[i][j], &QPushButton::clicked, this, &GUIInterface::onCellClicked);
//             boardLayout->addWidget(boardButtons[i][j], i, j);
//         }
//     }

//     // Replay Controls Widget
//     replayControlsWidget = new QWidget();
//     QHBoxLayout *replayLayout = new QHBoxLayout(replayControlsWidget);
//     replayLayout->setContentsMargins(0, 10, 0, 0);
//     replayStartButton = new QPushButton("<< Reset");
//     replayPrevButton = new QPushButton("< Prev");
//     replayNextButton = new QPushButton("Next >");
//     replayStartButton->setObjectName("replayButton");
//     replayPrevButton->setObjectName("replayButton");
//     replayNextButton->setObjectName("replayButton");
//     replayLayout->addWidget(replayStartButton);
//     replayLayout->addStretch();
//     replayLayout->addWidget(replayPrevButton);
//     replayLayout->addWidget(replayNextButton);
//     connect(replayStartButton, &QPushButton::clicked, this, &GUIInterface::onReplayStartClicked);
//     connect(replayPrevButton, &QPushButton::clicked, this, &GUIInterface::onReplayPrevClicked);
//     connect(replayNextButton, &QPushButton::clicked, this, &GUIInterface::onReplayNextClicked);

//     // Main bottom controls
//     QHBoxLayout *bottomControlsLayout = new QHBoxLayout();
    
//     QGroupBox* scoreBox = new QGroupBox("Scoreboard");
//     QVBoxLayout* scoreLayout = new QVBoxLayout(scoreBox);
//     playerXScoreLabel = new QLabel("Player X (You): W: 0 | L: 0 | T: 0");
//     playerXScoreLabel->setObjectName("scoreLabel");
//     playerOScoreLabel = new QLabel("Opponent: W: 0 | L: 0 | T: 0");
//     playerOScoreLabel->setObjectName("scoreLabel");
//     scoreLayout->addWidget(playerXScoreLabel);
//     scoreLayout->addWidget(playerOScoreLabel);

//     QGroupBox* modeBox = new QGroupBox("Game Mode");
//     QVBoxLayout* modeLayout = new QVBoxLayout(modeBox);
//     pvpModeRadio = new QRadioButton("Player vs Player");
//     aiModeRadio = new QRadioButton("Player vs AI");
//     aiModeRadio->setChecked(true);
//     modeLayout->addWidget(pvpModeRadio);
//     modeLayout->addWidget(aiModeRadio);

//     QGroupBox* difficultyBox = new QGroupBox("AI Difficulty");
//     QVBoxLayout* difficultyLayout = new QVBoxLayout(difficultyBox);
//     difficultyCombo = new QComboBox();
//     difficultyCombo->addItem("Easy", 1);
//     difficultyCombo->addItem("Medium", 3);
//     difficultyCombo->addItem("Hard (Unbeatable)", 9);
//     difficultyCombo->setCurrentIndex(1);
//     difficultyLayout->addWidget(difficultyCombo);

//     bottomControlsLayout->addWidget(scoreBox, 1);
//     bottomControlsLayout->addWidget(modeBox);
//     bottomControlsLayout->addWidget(difficultyBox);
    
//     QHBoxLayout* actionButtonsLayout = new QHBoxLayout();
//     QPushButton* newGameButton = new QPushButton("New Game");
//     QPushButton* viewHistoryButton = new QPushButton("View History");
//     QPushButton* logoutButton = new QPushButton("Logout");
//     actionButtonsLayout->addStretch();
//     actionButtonsLayout->addWidget(newGameButton);
//     actionButtonsLayout->addWidget(viewHistoryButton);
//     actionButtonsLayout->addWidget(logoutButton);

//     mainGameLayout->addWidget(statusLabel);
//     mainGameLayout->addLayout(boardLayout);
//     mainGameLayout->addWidget(replayControlsWidget);
//     mainGameLayout->addLayout(bottomControlsLayout);
//     mainGameLayout->addLayout(actionButtonsLayout);
    
//     connect(newGameButton, &QPushButton::clicked, this, &GUIInterface::onNewGameButtonClicked);
//     connect(viewHistoryButton, &QPushButton::clicked, this, &GUIInterface::onViewHistoryClicked);
//     connect(logoutButton, &QPushButton::clicked, this, &GUIInterface::onLogoutButtonClicked);
//     connect(difficultyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
//         aiEngine.setDifficulty(difficultyCombo->itemData(index).toInt());
//     });

//     mainStack->addWidget(gameWidget);
// }

// void GUIInterface::setupHistoryView() {
//     historyWidget = new QWidget();
//     QVBoxLayout *historyLayout = new QVBoxLayout(historyWidget);
//     historyLayout->setContentsMargins(20, 20, 20, 20);
//     historyLayout->setSpacing(15);

//     QLabel *historyTitle = new QLabel("Game History");
//     historyTitle->setObjectName("historyTitleLabel");
//     historyTitle->setAlignment(Qt::AlignCenter);

//     gameHistoryTable = new QTableWidget(0, 4);
//     gameHistoryTable->setHorizontalHeaderLabels({"Date", "Opponent", "Result", "Mode"});
//     gameHistoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//     gameHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
//     gameHistoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

//     backToGameButton = new QPushButton("Back to Game");
    
//     historyLayout->addWidget(historyTitle);
//     historyLayout->addWidget(gameHistoryTable);
//     historyLayout->addWidget(backToGameButton, 0, Qt::AlignRight);
    
//     connect(backToGameButton, &QPushButton::clicked, this, &GUIInterface::onBackToGameClicked);
//     connect(gameHistoryTable, &QTableWidget::cellDoubleClicked, this, &GUIInterface::onGameHistoryItemClicked);

//     mainStack->addWidget(historyWidget);
// }

// void GUIInterface::updateBoard(bool isReplay) {
//     if (!isReplay) {
//         statusLabel->setText(gameLogic.getCurrentPlayer() == Player::X ? "Player X's Turn" : "Player O's Turn");
//     }

//     for (int i = 0; i < 3; i++) {
//         for (int j = 0; j < 3; j++) {
//             Player cell = gameLogic.getCell(i, j);
//             QPushButton* button = boardButtons[i][j];

//             if (button->text().isEmpty() && cell != Player::NONE) {
//                 if (cell == Player::X) {
//                     button->setText("X");
//                     button->setStyleSheet("color: #3498DB;");
//                 } else {
//                     button->setText("O");
//                     button->setStyleSheet("color: #E74C3C;");
//                 }
//                 QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(button);
//                 button->setGraphicsEffect(effect);
//                 QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
//                 animation->setDuration(250);
//                 animation->setStartValue(0.0);
//                 animation->setEndValue(1.0);
//                 animation->start(QAbstractAnimation::DeleteWhenStopped);
//             } else if (cell == Player::NONE) {
//                 button->setText("");
//                 button->setGraphicsEffect(nullptr);
//                 button->setStyleSheet("");
//             }
//         }
//     }
// }

// void GUIInterface::updateScoreDisplay() {
//     if (userAuth.isLoggedIn()) {
//         const UserProfile* user = userAuth.getCurrentUser();
//         QString scoreText = QString("Player X (You): W: %1 | L: %2 | T: %3")
//             .arg(user->gamesWon).arg(user->gamesLost).arg(user->gamesTied);
//         playerXScoreLabel->setText(scoreText);
//     } else {
//         playerXScoreLabel->setText("Player X: W: 0 | L: 0 | T: 0");
//     }
//     playerOScoreLabel->setText("Opponent: W: 0 | L: 0 | T: 0");
// }

// void GUIInterface::onLoginButtonClicked() {
//     std::string username = usernameInput->text().toStdString();
//     std::string password = passwordInput->text().toStdString();

//     if (username.empty() || password.empty()) {
//         QMessageBox::warning(this, "Login Error", "Username and password cannot be empty.");
//         return;
//     }

//     if (userAuth.loginUser(username, password)) {
//         updateScoreDisplay();
//         switchToGameView();
//     } else {
//         QMessageBox::critical(this, "Login Failed", "Invalid username or password.");
//     }
// }

// void GUIInterface::onRegisterButtonClicked() {
//     std::string username = usernameInput->text().toStdString();
//     std::string password = passwordInput->text().toStdString();

//     if (username.empty() || password.empty()) {
//         QMessageBox::warning(this, "Registration Error", "Username and password cannot be empty.");
//         return;
//     }

//     // This is the simplified and correct logic.
//     // We call registerUser directly on our main userAuth object.
//     if (userAuth.registerUser(username, password)) {
//         // If registration is successful, the userAuth object now holds the new user's
//         // data. We can now save this new user to our database file.
//         dbManager.saveUser(*userAuth.getCurrentUser());

//         QMessageBox::information(this, "Success", "Registration successful! You can now log in.");
        
//         // After registration, the user is auto-logged in by the registerUser function,
//         // so we can switch to the login view for them to proceed.
//         switchToLoginView();
//     } else {
//         // If registerUser returns false, it means the username was already taken.
//         QMessageBox::warning(this, "Registration Failed", "This username is already taken.");
//     }
// }

// void GUIInterface::onLogoutButtonClicked() {
//     userAuth.logoutUser();
//     updateScoreDisplay();
//     switchToLoginView();
// }

// void GUIInterface::onCellClicked() {
//     QPushButton *button = qobject_cast<QPushButton*>(sender());
//     if (!button || gameLogic.checkGameResult() != GameResult::IN_PROGRESS) return;

//     int row = button->property("row").toInt();
//     int col = button->property("col").toInt();

//     if (gameLogic.makeMove(row, col)) {
//         updateBoard();

//         GameResult result = gameLogic.checkGameResult();
//         if (result != GameResult::IN_PROGRESS) {
//             handleGameOver(result);
//         } else if (aiModeRadio->isChecked() && gameLogic.getCurrentPlayer() != Player::X) {
//             makeAIMove();
//         }
//     }
// }

// void GUIInterface::makeAIMove() {
//     QApplication::processEvents();
//     Move aiMove = aiEngine.getBestMove(gameLogic);
//     if (gameLogic.makeMove(aiMove.row, aiMove.col)) {
//         updateBoard();
//         GameResult result = gameLogic.checkGameResult();
//         if (result != GameResult::IN_PROGRESS) {
//             handleGameOver(result);
//         }
//     }
// }

// void GUIInterface::onNewGameButtonClicked() {
//     gameLogic.resetBoard();
//     setupReplayControls(false); // Hide replay controls
//     for (int i = 0; i < 3; ++i) {
//         for (int j = 0; j < 3; ++j) {
//             boardButtons[i][j]->setEnabled(true);
//             boardButtons[i][j]->setStyleSheet(""); // Reset any winning highlights
//         }
//     }
//     updateBoard();
// }

// void GUIInterface::onViewHistoryClicked() {
//     loadUserGames();
//     switchToHistoryView();
// }

// void GUIInterface::onGameHistoryItemClicked(int row, int) {
//     if(!gameHistoryTable->item(row, 0)) return;

//     QVariant data = gameHistoryTable->item(row, 0)->data(Qt::UserRole);
//     std::string gameId = data.toString().toStdString();
//     GameState game = gameHistory.getGameById(gameId);
    
//     if (!game.gameId.empty()) {
//         displayGameForReplay(game);
//     }
// }

// void GUIInterface::onBackToGameClicked() {
//     setupReplayControls(false);
//     onNewGameButtonClicked();
//     switchToGameView();
// }

// void GUIInterface::handleGameOver(GameResult result) {
//     QString resultMessage;
//     switch (result) {
//         case GameResult::X_WINS: resultMessage = "Player X wins!"; break;
//         case GameResult::O_WINS: resultMessage = "Player O wins!"; break;
//         case GameResult::DRAW: resultMessage = "Game ended in a draw!"; break;
//         default: return;
//     }

//     statusLabel->setText(resultMessage);

//     if (result == GameResult::X_WINS || result == GameResult::O_WINS) {
//         std::vector<Move> winningCells = gameLogic.findWinningCombination();
//         for (const auto& cell : winningCells) {
//             boardButtons[cell.row][cell.col]->setStyleSheet("background-color: #1ABC9C;");
//         }
//     }

//     if (userAuth.isLoggedIn()) {
//         const UserProfile* currentUser = userAuth.getCurrentUser();
//         gameHistory.saveGame(currentUser->userId, "Opponent", aiModeRadio->isChecked(), gameLogic.getMoveHistory(), result);
//         dbManager.saveGameHistory(gameHistory.getAllGames());
//         userAuth.updateUserStats(result);
//         dbManager.saveUser(*currentUser);
//         updateScoreDisplay();
//     }
    
//     for(int i = 0; i < 3; ++i) for(int j = 0; j < 3; ++j) boardButtons[i][j]->setEnabled(false);
    
//     QMessageBox::information(this, "Game Over", resultMessage);
// }

// void GUIInterface::switchToLoginView() { mainStack->setCurrentWidget(loginWidget); }
// void GUIInterface::switchToGameView() { mainStack->setCurrentWidget(gameWidget); }
// void GUIInterface::switchToHistoryView() { mainStack->setCurrentWidget(historyWidget); }

// void GUIInterface::displayGameForReplay(const GameState& game) {
//     replayHistory = game.moveHistory;
//     replayMoveIndex = 0;
    
//     gameLogic.resetBoard();
//     updateBoard(true);
    
//     statusLabel->setText("Game Replay: " + QString::fromStdString(game.timestamp));
//     setupReplayControls(true);
//     switchToGameView();
// }

// void GUIInterface::setupReplayControls(bool show) {
//     if (replayControlsWidget) {
//         replayControlsWidget->setVisible(show);
//     }
//     for (int i = 0; i < 3; ++i) {
//         for (int j = 0; j < 3; ++j) {
//             // Disable board buttons if showing replay controls, enable otherwise
//             boardButtons[i][j]->setEnabled(!show);
//         }
//     }
// }

// void GUIInterface::onReplayStartClicked() {
//     replayMoveIndex = 0;
//     gameLogic.resetBoard();
//     updateBoard(true);
// }

// void GUIInterface::onReplayPrevClicked() {
//     if (replayMoveIndex > 0) {
//         // To go back one move, we need to reset and replay up to the previous index
//         gameLogic.resetBoard();
//         replayMoveIndex--;
//         for (int i = 0; i < replayMoveIndex; ++i) {
//             const auto& move = replayHistory[i];
//             gameLogic.makeMove(move.row, move.col);
//         }
//         updateBoard(true);
//     }
// }

// void GUIInterface::onReplayNextClicked() {
//     if (replayMoveIndex < replayHistory.size()) {
//         const auto& move = replayHistory[replayMoveIndex];
//         gameLogic.makeMove(move.row, move.col);
//         replayMoveIndex++;
//         updateBoard(true);
//     }
// }

// void GUIInterface::loadUserGames() {
//     if (!userAuth.isLoggedIn()) return;
    
//     const UserProfile* currentUser = userAuth.getCurrentUser();
//     std::vector<GameState> userGames = gameHistory.getUserGames(currentUser->userId);
    
//     gameHistoryTable->setRowCount(0);
    
//     for (const auto& game : userGames) {
//         int row = gameHistoryTable->rowCount();
//         gameHistoryTable->insertRow(row);
        
//         QTableWidgetItem* dateItem = new QTableWidgetItem(QString::fromStdString(game.timestamp));
//         dateItem->setData(Qt::UserRole, QVariant(QString::fromStdString(game.gameId)));
//         gameHistoryTable->setItem(row, 0, dateItem);
        
//         QString opponent = game.isAIOpponent ? "AI" : "Player2";
//         gameHistoryTable->setItem(row, 1, new QTableWidgetItem(opponent));
        
//         QString resultStr;
//         if(game.result == GameResult::X_WINS) resultStr = "You Won";
//         else if (game.result == GameResult::O_WINS) resultStr = "You Lost";
//         else resultStr = "Draw";
//         gameHistoryTable->setItem(row, 2, new QTableWidgetItem(resultStr));
        
//         QString mode = game.isAIOpponent ? "vs AI" : "vs Player";
//         gameHistoryTable->setItem(row, 3, new QTableWidgetItem(mode));
//     }
// }

/*============================================================================================================
/*============================================================================================================
/*============================================================================================================
/*============================================================================================================
/*============================================================================================================
/*============================================================================================================
*/


// File: gui_interface.cpp
// This is the complete and integrated implementation based on your concept.
#include "gui_interface.h"
#include <QApplication> // For applyStylesheet
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFont>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QDebug>

// Define the time limit for each turn in seconds.
static const int TURN_TIME_LIMIT = 15;

GUIInterface::GUIInterface(const std::string& dbPath, QWidget *parent)
    : QMainWindow(parent),
      dbManager(dbPath),
      replayMoveIndex(0)
{
    // The constructor sets up the entire application.
    applyStylesheet();
    setupUI();

    // Load all user and game data from the database at startup.
    try {
        auto loadedUsers = dbManager.loadUsers();
        userAuth.setUsers(loadedUsers);
        gameHistory.loadFromDatabase(dbManager);
    } catch (const std::exception& e) {
        qWarning() << "Could not load initial data: " << e.what();
    }
    
    // Set a default AI difficulty.
    aiEngine.setDifficulty(3);

    // The first screen the user sees is the login view.
    switchToLoginView();
}

GUIInterface::~GUIInterface() {}

void GUIInterface::applyStylesheet() {
    // A modern, dark-themed stylesheet for a professional look.
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
        QPushButton:disabled { background-color: #566573; color: #95A5A6; }
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
            alternate-background-color: #3A5064;
        }
        QHeaderView::section {
            background-color: #1ABC9C; color: white; padding: 8px;
            font-size: 14px; font-weight: bold; border: none;
        }
        QProgressBar {
            border: 1px solid #4A6572; border-radius: 5px; text-align: center;
            color: #ECF0F1; font-weight: bold;
        }
        QProgressBar::chunk {
            background-color: #1ABC9C; border-radius: 4px;
        }
    )";
    qApp->setStyleSheet(style);
}

void GUIInterface::setupUI() {
    setWindowTitle("Advanced Tic Tac Toe");
    setMinimumSize(800, 700);

    mainStack = new QStackedWidget(this);
    setCentralWidget(mainStack);

    // Call helper methods to create each screen.
    setupAuthentication();
    setupGameBoard();
    setupHistoryView();
}

void GUIInterface::setupAuthentication() {
    loginWidget = new QWidget();
    QVBoxLayout* loginLayout = new QVBoxLayout(loginWidget);
    loginLayout->setAlignment(Qt::AlignCenter);
    loginLayout->setSpacing(20);

    QLabel* titleLabel = new QLabel("Advanced Tic Tac Toe");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    QGroupBox* authBox = new QGroupBox("User Authentication");
    authBox->setFixedWidth(400);
    QGridLayout* authLayout = new QGridLayout(authBox);
    authLayout->setSpacing(15);

    usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText("Enter your username");
    passwordInput = new QLineEdit();
    passwordInput->setPlaceholderText("Enter your password");
    passwordInput->setEchoMode(QLineEdit::Password);

    authLayout->addWidget(new QLabel("Username:"), 0, 0);
    authLayout->addWidget(usernameInput, 0, 1);
    authLayout->addWidget(new QLabel("Password:"), 1, 0);
    authLayout->addWidget(passwordInput, 1, 1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* loginButton = new QPushButton("Login");
    QPushButton* registerButton = new QPushButton("Register");
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);
    authLayout->addLayout(buttonLayout, 2, 0, 1, 2);

    loginLayout->addWidget(titleLabel);
    loginLayout->addWidget(authBox, 0, Qt::AlignHCenter);

    connect(loginButton, &QPushButton::clicked, this, &GUIInterface::onLoginButtonClicked);
    connect(registerButton, &QPushButton::clicked, this, &GUIInterface::onRegisterButtonClicked);

    mainStack->addWidget(loginWidget);
}

void GUIInterface::setupGameBoard() {
    gameWidget = new QWidget;
    QVBoxLayout *mainGameLayout = new QVBoxLayout(gameWidget);
    mainGameLayout->setSpacing(15);
    mainGameLayout->setContentsMargins(20, 20, 20, 20);

    statusLabel = new QLabel("Welcome!");
    statusLabel->setObjectName("statusLabel");
    statusLabel->setAlignment(Qt::AlignCenter);

    timerBar = new QProgressBar();
    timerBar->setRange(0, TURN_TIME_LIMIT);
    timerBar->setValue(TURN_TIME_LIMIT);
    timerBar->setTextVisible(true);
    timerBar->setFormat("%v seconds left");

    turnTimer = new QTimer(this);
    connect(turnTimer, &QTimer::timeout, [this]() {
        int val = timerBar->value() - 1;
        timerBar->setValue(val);
        if (val <= 0) {
            turnTimer->stop();
            if (aiModeRadio->isChecked()) {
                makeAIMove();
            } else {
                gameLogic.makeMove(-1, -1); // Effectively skips turn
                updateBoard();
                turnTimer->start(1000);
            }
        }
    });

    QGridLayout* boardLayout = new QGridLayout;
    boardLayout->setSpacing(10);
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            boardButtons[r][c] = new QPushButton("");
            boardButtons[r][c]->setObjectName("gameBoardButton");
            boardButtons[r][c]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            boardButtons[r][c]->setProperty("row", r);
            boardButtons[r][c]->setProperty("col", c);
            connect(boardButtons[r][c], &QPushButton::clicked, this, &GUIInterface::onCellClicked);
            boardLayout->addWidget(boardButtons[r][c], r, c);
        }
    }

    replayControlsWidget = new QWidget();
    QHBoxLayout *replayLayout = new QHBoxLayout(replayControlsWidget);
    replayLayout->setContentsMargins(0, 5, 0, 5);
    replayStartButton = new QPushButton("<< Start");
    replayPrevButton = new QPushButton("< Prev");
    replayNextButton = new QPushButton("Next >");
    replayLayout->addWidget(replayStartButton);
    replayLayout->addStretch();
    replayLayout->addWidget(replayPrevButton);
    replayLayout->addWidget(replayNextButton);
    connect(replayStartButton, &QPushButton::clicked, this, &GUIInterface::onReplayStartClicked);
    connect(replayPrevButton, &QPushButton::clicked, this, &GUIInterface::onReplayPrevClicked);
    connect(replayNextButton, &QPushButton::clicked, this, &GUIInterface::onReplayNextClicked);

    QHBoxLayout* bottomLayout = new QHBoxLayout;
    
    QGroupBox* scoreBox = new QGroupBox("Scoreboard");
    QVBoxLayout* scoreLayout = new QVBoxLayout(scoreBox);
    playerXScoreLabel = new QLabel("Player (You): W: 0 L: 0 T: 0");
    playerOScoreLabel = new QLabel("Opponent: W: 0 L: 0 T: 0");
    scoreLayout->addWidget(playerXScoreLabel);
    scoreLayout->addWidget(playerOScoreLabel);
    
    QGroupBox* modeBox = new QGroupBox("Game Mode");
    QVBoxLayout* modeLayout = new QVBoxLayout(modeBox);
    pvpModeRadio = new QRadioButton("Player vs Player");
    aiModeRadio = new QRadioButton("Player vs AI");
    aiModeRadio->setChecked(true);
    modeLayout->addWidget(pvpModeRadio);
    modeLayout->addWidget(aiModeRadio);

    QGroupBox* difficultyBox = new QGroupBox("AI Difficulty");
    QVBoxLayout* difficultyLayout = new QVBoxLayout(difficultyBox);
    difficultyCombo = new QComboBox();
    difficultyCombo->addItem("Easy", 1);
    difficultyCombo->addItem("Medium", 3);
    difficultyCombo->addItem("Hard", 9);
    difficultyCombo->setCurrentIndex(1);
    difficultyLayout->addWidget(difficultyCombo);

    bottomLayout->addWidget(scoreBox);
    bottomLayout->addWidget(modeBox);
    bottomLayout->addWidget(difficultyBox);

    QHBoxLayout* actionButtonsLayout = new QHBoxLayout;
    undoButton = new QPushButton("Undo Move");
    QPushButton* newGameButton = new QPushButton("New Game");
    QPushButton* viewHistoryButton = new QPushButton("View History");
    QPushButton* logoutButton = new QPushButton("Logout");
    actionButtonsLayout->addWidget(undoButton);
    actionButtonsLayout->addStretch();
    actionButtonsLayout->addWidget(newGameButton);
    actionButtonsLayout->addWidget(viewHistoryButton);
    actionButtonsLayout->addWidget(logoutButton);

    mainGameLayout->addWidget(statusLabel);
    mainGameLayout->addWidget(timerBar);
    mainGameLayout->addLayout(boardLayout, 1);
    mainGameLayout->addWidget(replayControlsWidget);
    mainGameLayout->addLayout(bottomLayout);
    mainGameLayout->addLayout(actionButtonsLayout);
    
    connect(newGameButton, &QPushButton::clicked, this, &GUIInterface::onNewGameButtonClicked);
    connect(viewHistoryButton, &QPushButton::clicked, this, &GUIInterface::onViewHistoryClicked);
    connect(logoutButton, &QPushButton::clicked, this, &GUIInterface::onLogoutButtonClicked);
    connect(undoButton, &QPushButton::clicked, this, &GUIInterface::onUndoClicked);
    connect(difficultyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        aiEngine.setDifficulty(difficultyCombo->itemData(index).toInt());
    });

    mainStack->addWidget(gameWidget);
}

void GUIInterface::setupHistoryView() {
    historyWidget = new QWidget();
    QVBoxLayout* historyLayout = new QVBoxLayout(historyWidget);
    historyLayout->setContentsMargins(20, 20, 20, 20);
    historyLayout->setSpacing(15);

    QLabel* historyTitle = new QLabel("Game History");
    historyTitle->setObjectName("historyTitleLabel");
    historyTitle->setAlignment(Qt::AlignCenter);

    gameHistoryTable = new QTableWidget(0, 4);
    gameHistoryTable->setHorizontalHeaderLabels({"Date", "Opponent", "Result", "Mode"});
    gameHistoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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

// --- Slot Implementations ---

void GUIInterface::onLoginButtonClicked() {
    std::string username = usernameInput->text().toStdString();
    std::string password = passwordInput->text().toStdString();
    if (userAuth.loginUser(username, password)) {
        updateScoreDisplay();
        switchToGameView();
        onNewGameButtonClicked();
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
    }
}

void GUIInterface::onRegisterButtonClicked() {
    std::string username = usernameInput->text().toStdString();
    std::string password = passwordInput->text().toStdString();
    if (userAuth.registerUser(username, password)) {
        dbManager.saveUser(*userAuth.getCurrentUser());
        QMessageBox::information(this, "Success", "Registration successful! You are now logged in.");
        updateScoreDisplay();
        switchToGameView();
        onNewGameButtonClicked();
    } else {
        QMessageBox::warning(this, "Registration Failed", "Username may already be taken.");
    }
}

void GUIInterface::onLogoutButtonClicked() {
    userAuth.logoutUser();
    switchToLoginView();
}

void GUIInterface::onNewGameButtonClicked() {
    gameLogic.resetBoard();
    setupReplayControls(false);
    timerBar->setValue(TURN_TIME_LIMIT);
    timerBar->setFormat("%v seconds left");
    turnTimer->start(1000);
    undoButton->setEnabled(false);
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            boardButtons[i][j]->setEnabled(true);
            boardButtons[i][j]->setStyleSheet("");
        }
    }
    updateBoard();
}

void GUIInterface::onCellClicked() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button || gameLogic.checkGameResult() != GameResult::IN_PROGRESS) return;

    int row = button->property("row").toInt();
    int col = button->property("col").toInt();

    if (gameLogic.makeMove(row, col)) {
        updateBoard();
        undoButton->setEnabled(true); // Enable undo after a move
        
        GameResult result = gameLogic.checkGameResult();
        if (result != GameResult::IN_PROGRESS) {
            handleGameOver(result);
        } else {
            timerBar->setValue(TURN_TIME_LIMIT);
            turnTimer->start(1000);
            if (aiModeRadio->isChecked()) {
                undoButton->setEnabled(false); // Can't undo during AI's turn
                QApplication::processEvents();
                makeAIMove();
            }
        }
    }
}

void GUIInterface::onUndoClicked() {
    gameLogic.undoLastMove(); // Undo the AI's move
    gameLogic.undoLastMove(); // Undo the Player's move
    updateBoard();
    undoButton->setEnabled(false); // Can only undo once per turn
}

void GUIInterface::onViewHistoryClicked() {
    loadUserGames();
    switchToHistoryView();
}

void GUIInterface::onGameHistoryItemClicked(int row, int) {
    if (!gameHistoryTable->item(row, 0)) return;

    QVariant data = gameHistoryTable->item(row, 0)->data(Qt::UserRole);
    std::string gameId = data.toString().toStdString();
    GameState game = gameHistory.getGameById(gameId);

    if (!game.gameId.empty()) {
        displayGameForReplay(game);
    }
}

void GUIInterface::onBackToGameClicked() {
    switchToGameView();
    onNewGameButtonClicked();
}

// --- Replay Slot Implementations ---

void GUIInterface::onReplayStartClicked() {
    replayMoveIndex = 0;
    gameLogic.resetBoard();
    updateBoard(true);
}

void GUIInterface::onReplayPrevClicked() {
    if (replayMoveIndex > 0) {
        gameLogic.resetBoard();
        replayMoveIndex--;
        for (int i = 0; i < replayMoveIndex; ++i) {
            const auto& move = replayHistory[i];
            gameLogic.makeMove(move.row, move.col);
        }
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

// --- Helper Methods ---

void GUIInterface::updateBoard(bool isReplay) {
    if (!isReplay) {
        statusLabel->setText(gameLogic.getCurrentPlayer() == Player::X ? "Player X's Turn" : "Player O's Turn");
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Player cell = gameLogic.getCell(i, j);
            QPushButton* button = boardButtons[i][j];

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
                animation->setDuration(300);
                animation->setStartValue(0.0);
                animation->setEndValue(1.0);
                animation->start(QAbstractAnimation::DeleteWhenStopped);
            } else if (cell == Player::NONE) {
                button->setText("");
                button->setGraphicsEffect(nullptr);
                button->setStyleSheet("");
            }
        }
    }
}

void GUIInterface::updateScoreDisplay() {
    if (userAuth.isLoggedIn()) {
        const UserProfile* user = userAuth.getCurrentUser();
        QString scoreText = QString("Player (You): W: %1 | L: %2 | T: %3")
            .arg(user->gamesWon).arg(user->gamesLost).arg(user->gamesTied);
        playerXScoreLabel->setText(scoreText);
    } else {
        playerXScoreLabel->setText("Player (You): W: 0 | L: 0 | T: 0");
    }
    playerOScoreLabel->setText("Opponent: W: 0 | L: 0 | T: 0");
}

void GUIInterface::handleGameOver(GameResult result) {
    turnTimer->stop();
    QString resultMessage;
    switch (result) {
        case GameResult::X_WINS: resultMessage = "Player X wins!"; break;
        case GameResult::O_WINS: resultMessage = "Player O wins!"; break;
        case GameResult::DRAW: resultMessage = "Game ended in a draw!"; break;
        default: return;
    }
    statusLabel->setText(resultMessage);

    if (result == GameResult::X_WINS || result == GameResult::O_WINS) {
        auto winningCells = gameLogic.findWinningCombination();
        for (const auto& cell : winningCells) {
            boardButtons[cell.row][cell.col]->setStyleSheet("background-color: #1ABC9C;");
        }
    }

    for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) boardButtons[i][j]->setEnabled(false);
    undoButton->setEnabled(false);

    if (userAuth.isLoggedIn()) {
        const UserProfile* currentUser = userAuth.getCurrentUser();
        userAuth.updateUserStats(result);
        gameHistory.saveGame(currentUser->userId, "Opponent", aiModeRadio->isChecked(), gameLogic.getMoveHistory(), result);
        dbManager.saveGameHistory(gameHistory.getAllGames());
        dbManager.saveUser(*currentUser);
        updateScoreDisplay();
    }
    
    QMessageBox::information(this, "Game Over", resultMessage);
}

void GUIInterface::switchToLoginView() { mainStack->setCurrentWidget(loginWidget); }
void GUIInterface::switchToGameView() { mainStack->setCurrentWidget(gameWidget); }
void GUIInterface::switchToHistoryView() { mainStack->setCurrentWidget(historyWidget); }

void GUIInterface::setupReplayControls(bool show) {
    replayControlsWidget->setVisible(show);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            boardButtons[i][j]->setEnabled(!show);
        }
    }
}

void GUIInterface::makeAIMove() {
    Move aiMove = aiEngine.getBestMove(gameLogic);
    if (gameLogic.makeMove(aiMove.row, aiMove.col)) {
        updateBoard();
        undoButton->setEnabled(true);
        
        GameResult result = gameLogic.checkGameResult();
        if (result != GameResult::IN_PROGRESS) {
            handleGameOver(result);
        } else {
            timerBar->setValue(TURN_TIME_LIMIT);
            turnTimer->start(1000);
        }
    }
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
        dateItem->setData(Qt::UserRole, QVariant(QString::fromStdString(game.gameId)));
        gameHistoryTable->setItem(row, 0, dateItem);
        
        QString opponent = game.isAIOpponent ? "AI" : "Player2";
        gameHistoryTable->setItem(row, 1, new QTableWidgetItem(opponent));
        
        QString resultStr;
        bool userIsPlayer1 = (game.player1Id == currentUser->userId);
        if ((game.result == GameResult::X_WINS && userIsPlayer1) || (game.result == GameResult::O_WINS && !userIsPlayer1)) {
            resultStr = "You Won";
        } else if ((game.result == GameResult::O_WINS && userIsPlayer1) || (game.result == GameResult::X_WINS && !userIsPlayer1)) {
            resultStr = "You Lost";
        } else {
            resultStr = "Draw";
        }
        gameHistoryTable->setItem(row, 2, new QTableWidgetItem(resultStr));
        
        QString mode = game.isAIOpponent ? "vs AI" : "vs Player";
        gameHistoryTable->setItem(row, 3, new QTableWidgetItem(mode));
    }
}

void GUIInterface::displayGameForReplay(const GameState& game) {
    replayHistory = game.moveHistory;
    replayMoveIndex = 0;
    
    gameLogic.resetBoard();
    updateBoard(true);
    
    statusLabel->setText("Replay: " + QString::fromStdString(game.timestamp));
    setupReplayControls(true);
    timerBar->setVisible(false);
    undoButton->setVisible(false);
    switchToGameView();
}