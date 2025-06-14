// main.cpp
#include <QApplication>
#include <iostream>
#include <QStandardPaths>
#include <QDir>
#include "game_logic.h"
#include "ai_engine.h"
#include "gui_interface.h"

int main(int argc, char *argv[])
{
    // Initialize the Qt Application object.
    QApplication app(argc, argv);

    // Get a standard, writable path for the database file.
    // This ensures the application has permission to save data in a standard
    // user location (e.g., AppData on Windows).
    QString writablePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    
    // Ensure the directory exists before trying to write to it.
    QDir().mkpath(writablePath);

    // Construct the full path to the database file.
    std::string dbFullPath = (writablePath + "/tictactoe_data.db").toStdString();

    // Create and show the main GUI window.
    GUIInterface gui(dbFullPath);
    gui.show();

    // Start the Qt application event loop. This makes the window interactive
    // and waits for user input (like clicks).
    return app.exec();
}