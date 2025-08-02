#include "loginwindow.hpp"
#include "ui_loginwindow.h"
#include "user.hpp"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QScreen>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>

LoginWindow::LoginWindow(User &user, QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginWindow), user(user)
{
    ui->setupUi(this);                                 // Configurer la fenêtre
    this->setFixedSize(340 , 579);                     // Correction de la fenêtre de connexion (ne peut pas être déplacée)
    QPixmap pixmap(":/AppImages/Widget.png");          // Le chemin de l'image qui montre une personne en PNG dans la fenêtre de connexion

    // Appliquer des styles aux éléments de l'interface
    ui->LoginButton->setStyleSheet("background-color: #01042e; color: white; padding: 10px 15px; border: none; border-radius: 5px; font-size: 10px;");
    ui->LoginInput->setStyleSheet("padding: 10px; border: 1px solid #ccc; border-radius: 5px;width: 300px; height: 80px;font-size: 10px;");
    ui->label->setStyleSheet("font-size: 22px; font-weight: bold; color: #01042e;");
    // Définir le titre et la taille de la fenêtre
    setWindowTitle("Login");
    ui->label_2->setStyleSheet(" color: #01042e;");    // Donner au texte "SIGN IN" une couleur bleu foncé
    // Définir l'image de fond pour le label
    ui->backgroundImageLabel->setStyleSheet("background-image: url(:/AppImages/Back_g.jpg); background-position: center; background-repeat: no-repeat; background-size: cover;");
    ui->label_3->setPixmap(pixmap);                    // Définir l'image sur le QLabel
    ui->label_3->setScaledContents(true);              // Faire en sorte que l'image soit redimensionnée avec le label
    ui->LoginInput->setEchoMode(QLineEdit::Password);  // Masquer le code
    ui->LoginInput->setEchoMode(QLineEdit::Password);

    ui->LoginButton->raise();                          // Placer ce bouton au-dessus de la fenêtre
    ui->LoginInput->raise();                           // Placer ce bouton au-dessus de la fenêtre
    ui->label->raise();                                // Placer ce bouton au-dessus de la fenêtre
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_LoginButton_clicked()
{

    QString filePath = QCoreApplication::applicationDirPath() + "/auth.json"; // chemin vers le fichier auth.json où tous les codes sont stockés

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        QMessageBox::warning(nullptr, "Error", "Unable to open file for Authentfication!");}                // Tester si le fichier peut être ouvert

    QString codeInput = ui->LoginInput->text();                                                     // 
    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj = doc.object();
    QJsonArray array = obj["users"].toArray();


    int i(0);
    bool isAuthenticated = false;  // Indicateur pour suivre l'état de l'authentification

    for (int i = 0; i < array.size(); i++) {
        QJsonObject obj = array[i].toObject();

        if (obj["code"] == codeInput) {  // Vérifier si codeInput correspond à obj["code"]
            isAuthenticated = true;  // Marquer comme authentifié

            if (obj["access"].toInt() == 0) {  // Vérifier la valeur d'accès
                qDebug() << "access  = " << obj["access"];
                user.access = false;
            } else {
                qDebug() << "access  = " << obj["access"];
                user.access = true;
            }

            accept();  // Procéder si authentifié
            break;     // Quitter la boucle car nous avons trouvé une correspondance
        }
    }

    // Si aucune correspondance n'a été trouvée (isAuthenticated est toujours false)
    if (!isAuthenticated) {
        QMessageBox::warning(this, "Login Failed", "You are Not Authenticated");  // Afficher un message si l'utilisateur n'est pas authentifié
    }

    // Effacer le texte dans le champ LoginInput après traitement
    ui->LoginInput->clear();                          // Effacer le texte après l'avoir utilisé
}
