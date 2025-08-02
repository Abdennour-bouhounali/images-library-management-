#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include "user.hpp"
#include "descriptordetails.hpp"
#include "add_new_descriptor.hpp"
#include "librarymanagement.hpp"
#include <QVBoxLayout>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class Home; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(User user, QWidget *parent = nullptr,ManageLibrary mainlibrary = ManageLibrary(0, nullptr,""), ManageLibrary sublibrary = ManageLibrary(0, nullptr,""));
    ~MainWindow();
    int currentLibraryId;
    QString currentLibraryPath;
    ManageLibrary mainlibrary;
    ManageLibrary sublibrary;
protected:
    bool eventFilter(QObject *obj, QEvent *event) override; // Add this method

private slots:


    void on_actionLoad_a_Library_triggered();
    // void on_costSlider_valueChanged(int value);


    void on_CreateNewLibrary_triggered();

    void on_actionAdd_New_Descriptor_triggered();

    void on_actionDelete_a_library_triggered();

    void on_SearchButton_clicked();
    void on_returnButton_clicked();

    void on_SubListButton_MaxMin_clicked();

    void on_ClearFilterButton_clicked();

    void on_DescendingButton_clicked();

    void on_AscendingButton_clicked();

    void on_saveSubListButton_clicked();

    void on_SubListButton_Max_clicked();

    void on_SubListButton_Min_clicked();

    void SaveChanges_clicked(Descriptor *currentDescriptor, unsigned int originalId);

    void on_SubListButton_Gratuit_clicked();

    void on_LogoutButton_clicked();
signals:
    void logoutRequested();  // Signal to request logout

private:
    Ui::Home *ui;
    User currentUser;
    QGridLayout *gridLayout;
    QVBoxLayout *gridLayout_Buttons;
    DescriptorDetails *descriptorDetails;
    QMap<QWidget*, Descriptor*> widgetDescriptorMap;


    // int getCurrentLibraryId();
    // void setCurrentLibraryId(int currentLibraryId);
    void refreshLibrary();
    void setCurrentLibraryPath(QString currentLibraryPath);
    void loadLibraries();
    void LoadTheLibrary(QString path);
    void loadLibrariesButtons();
    void ShowTheLibrary(ManageLibrary library);
    void clearGridLayout();
    void populateGridLayout(Descriptor* head);
    void cleanUpDescriptors(Descriptor* head);
    User getCurrentUser();


};

#endif // MAINWINDOW_H
