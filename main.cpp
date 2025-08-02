#include "mainwindow.h"
#include "loginwindow.hpp"
#include "user.hpp"
#include <QApplication>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QLoggingCategory::setFilterRules("*.debug=false");

    User user; // Create a User object
    LoginWindow loginWindow(user); // Create the login window

    // Loop until user logs in or cancels login
    while (true) {
        // Show the login window and check authentication
        if (loginWindow.exec() == QDialog::Accepted) {
            // If login is successful, create the MainWindow with the user object
            MainWindow mainWindow(user);

            // Connect the logout signal from MainWindow to show the login window again
            QObject::connect(&mainWindow, &MainWindow::logoutRequested, [&]() {
                mainWindow.close();  // Close the MainWindow
                loginWindow.show();  // Show the LoginWindow again
            });

            // Show the main window
            mainWindow.show();

            // Run the application event loop for the MainWindow
            a.exec();

            // After logout, reset the login window (optional step to ensure no state persists)
            loginWindow.hide(); // Hide the LoginWindow (to ensure it doesn't reappear in the background)
        } else {
            // If login fails or the user cancels, exit the application
            return 0;
        }
    }
}
