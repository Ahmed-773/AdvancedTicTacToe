// main.cpp
#include "gui_interface.h"
#include "game_logic.h"
#include "ai_engine.h"
#include <QApplication>
#include <iostream>
#include <QStandardPaths>
#include <QDir>

int main(int argc, char *argv[]) {
    // Initialize the Qt Application
    QApplication app(argc, argv);

    // Set up a standard path for storing application data (like the database)
    // This ensures the application has write permissions.
    QString writablePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(writablePath); // Create the directory if it doesn't exist

    // Construct the full path for the database file
    std::string dbFullPath = (writablePath + "/tictactoe_data.db").toStdString();

    // Create and show the main GUI window
    GUIInterface gui(dbFullPath);
    gui.show();
    
    // Start the Qt event loop
    return app.exec();
}