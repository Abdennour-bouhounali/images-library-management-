#ifndef USER_HPP
#define USER_HPP
#include <QString>
#include "librarymanagement.hpp"
#include <QJsonArray>

class User
{
private:


public:
    bool access;
    QString libraryPath;
    User() : access("") {};
    User(bool access);
    ManageLibrary loadLibrary(const QString& path) const  ;
    QJsonArray loadLibraries(const QString& librariesFilePath);
    // create a new library method
    void createLibrary(QString libraryName);
    void deleteLibrary(QString libraryToDelete);
};

#endif // USER_HPP
