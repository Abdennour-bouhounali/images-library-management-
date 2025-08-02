#ifndef DESCRIPTORDETAILS_HPP
#define DESCRIPTORDETAILS_HPP

#include <QDialog>
#include <QString>
#include <QJsonArray>
#include <QLabel>

#include "descriptor.hpp"

namespace Ui {
class DescriptorDetails;
}

class DescriptorDetails : public QDialog
{
    Q_OBJECT

public:
    explicit DescriptorDetails(QWidget *parent = nullptr , bool access = false,QString LibraryPath = "");
    ~DescriptorDetails();
    void setDescriptor(Descriptor* descriptor);
    void setLibraryPath(QString libraryPath);
    QString getLibraryPath();
    bool access;

private slots:
    void on_filtreButton_clicked();
    void onFilterSelectionChanged(int index);
    void on_SaveChanges_clicked();
    void onLabelClicked(QLabel *clickedLabel); 

private:
    Ui::DescriptorDetails *ui;
    Descriptor* currentDescriptor;
    QString LibraryPath;

};

#endif // DESCRIPTORDETAILS_HPP
