#ifndef LOGINWINDOW_HPP
#define LOGINWINDOW_HPP

#include <QDialog>
#include "user.hpp"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(User &user, QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void on_LoginButton_clicked();
private:
    Ui::LoginWindow *ui;
    User &user; // Reference to the User object


};

#endif // LOGINWINDOW_HPP
