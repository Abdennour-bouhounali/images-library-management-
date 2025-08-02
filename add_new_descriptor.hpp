#ifndef ADD_NEW_DESCRIPTOR_HPP
#define ADD_NEW_DESCRIPTOR_HPP

#include <QDialog>

namespace Ui {
class Add_New_Descriptor;
}

class Add_New_Descriptor : public QDialog
{
    Q_OBJECT

public:
    explicit Add_New_Descriptor(QString Librarypath,QWidget *parent = nullptr);
    ~Add_New_Descriptor();
    void setLibraryPath(QString Librarypath);

private slots:
    void on_loadImageButton_clicked();

    void on_save_the_descriptor_clicked();

private:
    Ui::Add_New_Descriptor *ui;
    QString Librarypath;

};

#endif // ADD_NEW_DESCRIPTOR_HPP
