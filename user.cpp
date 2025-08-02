#include "user.hpp"
#include "librarymanagement.hpp"
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QIODevice>
#include <QDebug>
#include <QPushButton>
#include <QCoreApplication>

User::User(bool access):access(access) {}

ManageLibrary User::loadLibrary(const QString& path) const {
    // Load the file that contains the information of the library and create the ManageLibrary object
    // and display the library
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Could not open file";
        exit(1);
    }
    qDebug() << "In load library : " << path;

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj = doc.object();
    QJsonArray array = obj["library"].toArray();

    if (array.isEmpty()) {
        qDebug() << "The library is empty.";
        file.close();
        ManageLibrary library(1, nullptr,path);

        return library; // Return an empty ManageLibrary object
    }
    

    Descriptor* head = nullptr;
    Descriptor* current = nullptr;

    for (int i = 0; i < array.size(); i++) {
        QJsonObject obj = array[i].toObject();
        qDebug() << "ID: " << obj["id"].toInt();
        // qDebug() << "Library ID: " << obj["libraryID"].toInt();
        qDebug() << "Cost: " << obj["cost"].toDouble();
        qDebug() << "Title: " << obj["title"].toString();
        qDebug() << "Source: " << obj["source"].toString();
        qDebug() << "Access: " << obj["access"].toString();
        qDebug() << "Imagepath: " << obj["Imagepath"].toString();
        

        Descriptor* newDescriptor = new Descriptor(
            obj["id"].toInt(),
            obj["cost"].toDouble(),
            obj["title"].toString(),
            obj["source"].toString(),
            obj["access"].toString().toStdString().c_str()[0],
            Image(obj["Imagepath"].toString())
            ); 
            if (head == nullptr) {
            head = newDescriptor;
            current = head;
        } else {
            current->setNextDescriptor(newDescriptor);
            current = newDescriptor;
        }
    }

    file.close();

    ManageLibrary library(1, head,path);
    qDebug() << "Displaying Library second time";
    library.display();
    return library;
}




QJsonArray User::loadLibraries(const QString& librariesFilePath) {
    qDebug() << "in loadLibraries function";
    // Load the file that contains the libraries path
    qDebug() << "Loading libraries from: " << librariesFilePath;
    QFile file(librariesFilePath);

    if (!file.exists()) {
        qDebug() << "Error: File does not exist";
        exit(1);
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Could not open file";
        qDebug() << "Error details: " << file.errorString();
        exit(1);
    }

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj = doc.object();
    QJsonArray array = obj["libraries"].toArray();

    return array;
}


// create a new library method
void User::createLibrary(QString libraryName) {
  // create a new library json file and save it in the libraries folder
  // the file will contain an empty array
    QString appPath = QCoreApplication::applicationDirPath();
    qDebug() << "Creating library: " << libraryName;
    qDebug() << appPath;

  QJsonObject library;
  library["name"] = libraryName;
  library["path"] = "/Libraries/" + libraryName + ".json";

  //add to the libraries.json file
  QFile file(appPath + "/libraries.json");

  if (!file.open(QIODevice::ReadOnly)) {
      qDebug() << "Error: Could not open file";
      exit(1);
  }


  QByteArray data = file.readAll();
  // add library to data
  QJsonDocument doc(QJsonDocument::fromJson(data));
  QJsonObject obj = doc.object();
  QJsonArray array = obj["libraries"].toArray();
  array.append(library);
  obj["libraries"] = array;
  file.close();
  // write the updated data to the file
  if (!file.open(QIODevice::WriteOnly)) {
      qDebug() << "Error: Could not open file";
      exit(1);
  }
  file.write(QJsonDocument(obj).toJson());
  file.close();
  // create the library file
  qDebug() << "Creating the library file";
  qDebug() << appPath + library["path"].toString();

  QFile libraryFile(appPath + library["path"].toString());
  if (!libraryFile.open(QIODevice::WriteOnly)) {
      qDebug() << "Error: Could not open file";
      exit(1);
  }

  libraryFile.write(QJsonDocument(QJsonObject{{"library", QJsonArray()}}).toJson());
  libraryFile.close();


}
// create the delete library method that delete the library file and the librray from the libraries.json file
void User::deleteLibrary(QString libraryToDelete){
    qDebug() << "Deleting library: " << libraryToDelete;
    QString appPath = QCoreApplication::applicationDirPath();

    QString libraryPath = appPath + "/Libraries/" + libraryToDelete + ".json";
    // delete the file
    qDebug() << "Deleting the library file at : " << libraryPath;
    QFile file(libraryPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Could not open file";
        exit(1);
    }
    file.remove();
    file.close();
    // delete the name and path of the library from the libraries.json file
    QFile librariesFile(appPath + "/libraries.json");
    if (!librariesFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Could not open file";
        exit(1);
    }
    QByteArray data = librariesFile.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj = doc.object();
    QJsonArray array = obj["libraries"].toArray();
    QJsonArray newArray;
    for (int i = 0; i < array.size(); i++) {
        QJsonObject libraryObj = array[i].toObject();
        if (libraryObj["name"].toString() != libraryToDelete) {
            newArray.append(libraryObj);
        }
    }
    obj["libraries"] = newArray;
    librariesFile.close();
    if (!librariesFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Could not open file";
        exit(1);
    }
    librariesFile.write(QJsonDocument(obj).toJson());
    librariesFile.close();

}
