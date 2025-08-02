#include "add_new_descriptor.hpp"
#include "ui_add_new_descriptor.h"
#include <QFileDialog>
#include "descriptor.hpp"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QIODevice>


Add_New_Descriptor::Add_New_Descriptor(QString Librarypath, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Add_New_Descriptor)
{
    ui->setupUi(this);
    ui->Image_path_hidden->setVisible(false);
    qDebug() << "Add_New_Descriptor constructor called";
    qDebug() << "Library path: " << Librarypath;

    setLibraryPath(Librarypath);

    // Ensure no duplicate connections
    disconnect(ui->loadImageButton, nullptr, nullptr, nullptr);
    disconnect(ui->save_the_descriptor, nullptr, nullptr, nullptr);

    // Connect signals to slots
    connect(ui->loadImageButton, &QPushButton::clicked, this, &Add_New_Descriptor::on_loadImageButton_clicked);
    connect(ui->save_the_descriptor, &QPushButton::clicked, this, &Add_New_Descriptor::on_save_the_descriptor_clicked);

    // Apply styles to the UI elements
    ui->loadImageButton->setStyleSheet("background-color: #99c1f1; color: white; padding: 10px 20px; border: none; border-radius: 5px;");
    ui->save_the_descriptor->setStyleSheet("background-color: #99c1f1; color: white; padding: 10px 20px; border: none; border-radius: 5px;");
    ui->add_title->setStyleSheet("padding: 10px; border: 1px solid #ccc; border-radius: 5px;");
    ui->add_source->setStyleSheet("padding: 10px; border: 1px solid #ccc; border-radius: 5px;");
    
    ui->add_cost->setStyleSheet("padding: 10px; border: 1px solid #ccc; border-radius: 5px;");
    ui->add_cost->setPlaceholderText("Enter the cost");
    ui->add_title->setPlaceholderText("Enter the title");
    ui->add_source->setPlaceholderText("Enter the source");
    
    // ui->ImageLabel->setStyleSheet("border: 1px solid #ccc; padding: 5px; background-image: url(Images/galerie.jpg); background-repeat: no-repeat; background-position: center;");

    ui->ImageLabel->show(); 
    ui->comboBox_Access->addItem("L");
    ui->comboBox_Access->addItem("O");
}

Add_New_Descriptor::~Add_New_Descriptor()
{
    delete ui;
}

void Add_New_Descriptor::on_loadImageButton_clicked()
{
    static bool isProcessing = false;
    if (isProcessing) {
        qDebug() << "on_loadImageButton_clicked: Already processing!";
        return; // Prevent re-entry if already processing
    }
    isProcessing = true;

    qDebug() << "on_loadImageButton_clicked: Dialog opened";
    QString imagePath = QFileDialog::getOpenFileName(this, "Open Image", "", "Image Files (*.png *.jpg *.bmp)");

    if (!imagePath.isEmpty()) {
        QPixmap pixmap(imagePath);
        ui->Image_path_hidden->setText(imagePath);
        qDebug() << "Selected imagePath: " << imagePath;
        ui->ImageLabel->setPixmap(pixmap.scaled(210, 210, Qt::KeepAspectRatio));
    } else {
        qDebug() << "on_loadImageButton_clicked: No file selected";
    }

    isProcessing = false; // Reset the flag
}

void Add_New_Descriptor::on_save_the_descriptor_clicked()
{
    static bool isProcessing = false;
    if (isProcessing) {
        qDebug() << "on_save_the_descriptor_clicked: Already processing!";
        return; // Prevent re-entry if already processing
    }
    isProcessing = true;

    // Gather data from UI
    QString title = ui->add_title->text();
    QString source = ui->add_source->text();
    QString cost = ui->add_cost->text();
    QString imagePath = ui->Image_path_hidden->text();
    char access = ui->comboBox_Access->currentText().toStdString().c_str()[0];

    qDebug() << "on_save_the_descriptor_clicked: Gathering data from UI";
    qDebug() << "Access: " << access;
    qDebug() << "Image path: " << imagePath;

    // Verify if the image path is valid
    if (imagePath.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please load an image before saving.");
        isProcessing = false;
        return;
    }

    QString appPath = QCoreApplication::applicationDirPath();
    QString destinationDir = appPath + "/Images/";
    // QString destinationPath = destinationDir + QFileInfo(imagePath).fileName();

    // Ensure the destination directory exists
    QDir dir(destinationDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            QMessageBox::warning(this, "Directory Error", "Could not create destination directory.");
            isProcessing = false;
            return;
        }
    }

    QString fileName = QFileInfo(imagePath).fileName();
    QString uniqueFileName = fileName;
    int counter = 2;

    while (QFile::exists(destinationDir + uniqueFileName)) {
        uniqueFileName = QFileInfo(fileName).completeBaseName() + "_" + QString::number(counter) + "." + QFileInfo(fileName).suffix();
        counter++;
    }

    QString destinationPath = destinationDir + uniqueFileName;
    // Copy the image to the destination directory
    QFile file(imagePath);
    if (!file.copy(destinationPath)) {
        QMessageBox::warning(this, "File Error", "Could not copy the image file.");
        isProcessing = false;
        return;
    }

    // Image image("/Images/" + QFileInfo(imagePath).fileName());
    Image image("/Images/" + uniqueFileName);

    Descriptor descriptor(0, cost.toDouble(), title, source, access, image);

    // Save descriptor to JSON
    QFile jsonFile(Librarypath);
    if (!jsonFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "File Error", "Could not open the library file.");
        isProcessing = false;
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();

    QJsonObject obj = doc.object();
    QJsonArray array = obj["library"].toArray();

    QJsonObject newDescriptor;
    newDescriptor["id"] = array.size() + 1;
    newDescriptor["cost"] = cost.toDouble();
    newDescriptor["title"] = title;
    newDescriptor["source"] = source;
    newDescriptor["access"] = QString(access);
    // newDescriptor["Imagepath"] = "/Images/" + QFileInfo(imagePath).fileName();
    newDescriptor["Imagepath"] = "/Images/" + uniqueFileName;
    

    array.append(newDescriptor);
    obj["library"] = array;

    if (!jsonFile.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "File Error", "Could not open the library file for writing.");
        isProcessing = false;
        return;
    }

    jsonFile.write(QJsonDocument(obj).toJson());
    jsonFile.close();

    // Close the dialog
    accept();
    qDebug() << "on_save_the_descriptor_clicked: Descriptor saved successfully";

    isProcessing = false; // Reset the flag
}

void Add_New_Descriptor::setLibraryPath(QString Librarypath)
{
    this->Librarypath = Librarypath;
}
