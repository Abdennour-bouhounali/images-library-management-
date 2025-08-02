#include "descriptordetails.hpp"
#include "ui_descriptordetails.h"
#include <QMessageBox>
#include "imageproccessing.hpp"
#include "ClickableLabel.hpp"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFileDialog>

DescriptorDetails::DescriptorDetails(QWidget *parent , bool access,QString LibraryPath )

    : QDialog(parent)
    , ui(new Ui::DescriptorDetails)
    , currentDescriptor(nullptr)
    , access(access)
{
    ui->setupUi(this);

        ui->comboBox->addItem("Gaussien Filter");
        ui->comboBox->addItem("Median Filter");
        ui->comboBox->addItem("To GrayScale");
        ui->comboBox->addItem("Edge Detection");
        ui->comboBox->addItem("Seuillage");
        ui->comboBox->addItem("Rotation");
        ui->comboBox_2->addItem("Left");  
        ui->comboBox_2->addItem("Right");
        ui->comboBox_2->addItem("Down");
        ui->comboBox_2->addItem("Up");
        ui->comboBox->addItem("SIFT");
        ui->comboBox->addItem("Histogram");
        ui->comboBox->addItem("Erosion");


        ui->filtreButton->setVisible(true);
        ui->comboBox->setVisible(true);
        ui->FilteredImageLabel->setVisible(true);

        ui->thresholdLabel->setText("Threshold:");
        ui->thresholdLabel->setVisible(false); 
        ui->thresholdInput->setVisible(false);
        ui->comboBox_2->setVisible(false); 

        ui->kernelsizelabel->setText("Kernel size");
        ui->kernelsizelabel->setVisible(false); 
        ui->Kernelsizeinput->setVisible(false);

        connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DescriptorDetails::onFilterSelectionChanged);
        connect(ui->FilteredImageLabel, &ClickableLabel::clicked, this, [this]() {onLabelClicked(ui->FilteredImageLabel);});
        connect(ui->ImageLabel, &ClickableLabel::clicked, this, [this]() {onLabelClicked(ui->ImageLabel);});
        
        QPixmap pixmap(":/AppImages/traiter.png"); 
        pixmap = pixmap.scaled(ui->label_icone_1->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation); // Redimensionne l'image tout en gardant les proportions
        ui->label_icone_1->setPixmap(pixmap);
        ui->label_icone_2->setPixmap(pixmap);

}


DescriptorDetails::~DescriptorDetails()
{
    delete ui;
}

void DescriptorDetails::setLibraryPath(QString libraryPath){
    this->LibraryPath = libraryPath;
}

QString DescriptorDetails::getLibraryPath(){
    return this->LibraryPath;
}

void DescriptorDetails::setDescriptor(Descriptor* descriptor) {
    currentDescriptor = descriptor; // Store the current descriptor

    // ui->idLabel->setText(QString::number(descriptor->getIdDescriptor()));
    // ui->costLabel->setText(QString::number(descriptor->getCost()));
    // ui->titleLabel->setText(descriptor->getTitle());
    // ui->sourceLabel->setText(descriptor->getSource());
    // ui->accessLabel->setText(QString(descriptor->getAccess()));
    QString appPath = QCoreApplication::applicationDirPath();

    // Load the image directly from the file path
    QPixmap pixmap(appPath + descriptor->getImage().getPath());
    if (pixmap.isNull()) {
        QMessageBox::warning(this, "Error", "Failed to load the image. Check the file path or format.");
        return;
    }

    ui->ImageLabel->setPixmap(pixmap.scaled(ui->ImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->ImageLabel->setAlignment(Qt::AlignCenter);
 if(access){
    // Clear the filtered image label
    ui->FilteredImageLabel->clear();
    ui->FilteredImageLabel->setPixmap(QPixmap());
 }
}

void DescriptorDetails::onFilterSelectionChanged(int index) {
    QString selectedFilter = ui->comboBox->itemText(index);

    // Si le filtre "Seuillage" est sélectionné, afficher le champ de saisie du seuil
    if (selectedFilter == "Seuillage") {
        ui->thresholdLabel->setVisible(true);
        ui->thresholdInput->setVisible(true);
    } else {
        // Cacher le champ pour tous les autres filtres
        ui->thresholdLabel->setVisible(false);
        ui->thresholdInput->setVisible(false);
    }
    if (selectedFilter == "Rotation") {
        ui->comboBox_2->setVisible(true);
    } else {
        // Cacher le champ pour tous les autres filtres
        ui->comboBox_2->setVisible(false);
    }
    if (selectedFilter =="Erosion") {
        ui->Kernelsizeinput->setVisible(true);
        ui->kernelsizelabel->setVisible(true);
    } else {
        // Cacher le champ pour tous les autres filtres
        ui->Kernelsizeinput->setVisible(false);
        ui->kernelsizelabel->setVisible(false);
    }

}   


void DescriptorDetails::on_filtreButton_clicked() {
    QString filter = ui->comboBox->currentText();
    QString Rotate = ui->comboBox_2->currentText();

    ui->FilteredImageLabel->clear();

    QString imagePath = currentDescriptor->getImage().getPath();

    QString appPath = QCoreApplication::applicationDirPath();
    imagePath = appPath + imagePath;
    Mat inputImage = cv::imread(imagePath.toStdString());

    if (inputImage.empty()) {
        QMessageBox::warning(this, "Error", "Failed to convert QPixmap to cv::Mat.");
        return;
    }

    // Apply the selected filter
    ImageProccessing processor;
    try {
        Mat outputImage;

        if (filter == "Gaussien Filter") {
            // // Apply Gaussian filter with 5x5 kernel and sigma = 1.0
            outputImage = processor.applyGaussianFilter(inputImage);
            // imwrite(outputImage,"test.jpg")
        } else if (filter == "Edge Detection") {
            // Apply edge detection with 3*3 sobel filter 
            outputImage = processor.applyEdgeDetection(inputImage);

        } else if (filter == "Median Filter") {
            // Apply Median filter with kernel size = 5
            outputImage = processor.applyCustomMedianFilter(inputImage, 3);

        } else if (filter == "Rotation") {

            if(Rotate=="Down"){
                outputImage = processor.rotateImage(inputImage, 180);
            }else if(Rotate=="Up"){
                outputImage = processor.rotateImage(inputImage, 0);
            }else if(Rotate=="Left"){
                outputImage = processor.rotateImage(inputImage, 270);
            }else if(Rotate=="Right"){
                outputImage = processor.rotateImage(inputImage, 90);
            }

        } else if (filter == "To GrayScale") {
            // Convert to grayscale
            outputImage = processor.toGrayScale(inputImage);

        } else if (filter == "SIFT") {
            outputImage = processor.applySIFT(inputImage);

        } else if (filter == "Seuillage") {   

            int thresholdValue ; 
            if (ui->thresholdInput->isVisible()) {
                bool ok;
                thresholdValue = ui->thresholdInput->text().toInt(&ok);
                if (!ok) {
                    QMessageBox::warning(this, "Erreur", "Valeur de seuil invalide.");
                    return;
                }
            }
            outputImage = processor.applyThreshold(inputImage,thresholdValue);

        } else if (filter == "Histogram") {
            // Calcul de l'histogramme
            outputImage = processor.calculateHistogram(inputImage);

        } else if (filter =="Erosion") {

            int Kernelsize ; 
            if (ui->Kernelsizeinput->isVisible()) {
                bool ok;
                Kernelsize = ui->Kernelsizeinput->text().toInt(&ok);
                if (!ok) {
                    QMessageBox::warning(this, "Erreur", "Valeur de seuil invalide.");
                    return;
                }
            }
            outputImage = processor.applyErosion(inputImage,Kernelsize);

        } else if (filter =="Erosion"){ 
            outputImage = processor.applyErosion(inputImage,25);

        } else {    
            throw invalid_argument("Invalid filter selected.");
        }

        QImage filteredQImage;

        if (outputImage.channels() == 4) { // ARGB image
            cv::Mat clonedImage = outputImage.clone(); // Clone ensures memory remains valid
            filteredQImage = QImage(clonedImage.data, clonedImage.cols, clonedImage.rows, clonedImage.step, QImage::Format_ARGB32).copy();
        } else if (outputImage.channels() == 3) { // RGB image
            cv::Mat rgbImage;
            cv::cvtColor(outputImage, rgbImage, cv::COLOR_BGR2RGB); // Convert BGR to RGB
            QImage tempImage(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step, QImage::Format_RGB888);
            filteredQImage = tempImage.copy(); // Copy ensures memory remains valid
        } else if (outputImage.channels() == 1) { // Grayscale image
            QImage tempImage(outputImage.data, outputImage.cols, outputImage.rows, outputImage.step, QImage::Format_Grayscale8);
            filteredQImage = tempImage.copy(); // Copy ensures memory remains valid
        } else {
            throw std::runtime_error("Unsupported image format.");
        }

        // Display the filtered image in the QLabel
        ui->FilteredImageLabel->setPixmap(
            QPixmap::fromImage(filteredQImage).scaled(
                ui->FilteredImageLabel->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                )
            );
        ui->FilteredImageLabel->setAlignment(Qt::AlignCenter);

        //QMessageBox::information(this, "Filter Applied", "The filter has been applied successfully.");
    } catch (const exception& e) {
        QMessageBox::critical(this, "Error", QString("An error occurred: %1").arg(e.what()));
    }
}

void DescriptorDetails::on_SaveChanges_clicked()
{
    QString libraryPath = this->getLibraryPath();

    qDebug() << "Image Path";
    qDebug() << currentDescriptor->getImage().getPath();

    unsigned int CurrentIdD = currentDescriptor->getIdDescriptor();
    QString appPath = QCoreApplication::applicationDirPath();

    QJsonObject curObj =  currentDescriptor->toJson();

    // load the library
    qDebug() << "Library to edit";
    qDebug() << libraryPath;
    QFile file( libraryPath);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Could not open file";
        return;
    }

    // Read the existing JSON file
    QByteArray data = file.readAll();
    file.close();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj = doc.object();
    QJsonArray array = obj["library"].toArray();
    QJsonArray newArray;

    QString imagePathToDelete;

    for (int i = 0; i < array.size(); i++) {

        QJsonObject obj = array[i].toObject();

        if (obj["id"].toInt() == CurrentIdD) {
            newArray.append(curObj);
        }else{
            newArray.append(obj);
        }

    }
    obj["library"] = newArray;

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Could not open file";
        return;
    }

    file.write(QJsonDocument(obj).toJson());
    file.close();

    QPixmap pixmap = ui->FilteredImageLabel->pixmap(Qt::ReturnByValue);

    // Check if the pixmap is valid
    if (!pixmap.isNull()) {
        QImage filteredQImage = pixmap.toImage();

        // Open a file dialog to choose the save location
        QString savePath = QFileDialog::getSaveFileName(this, "Save Filtered Image", "", "Images (*.png *.jpg *.bmp)");

        // Save the image if a path is provided
        if (!savePath.isEmpty()) {
            if (!savePath.contains('.')) {
                    savePath.append(".png");}
    
            filteredQImage.save(savePath);
        }
        qDebug() << "Saved to path:"<<savePath;

    } else {
        QMessageBox::warning(this, "Save Error", "No filtered image to save.");
    }


}

void DescriptorDetails::onLabelClicked(QLabel *clickedLabel) {
    // Vérifiez si une image est chargée dans le QLabel cliqué
    QPixmap pixmap = clickedLabel->pixmap(Qt::ReturnByValue);
    
    if (pixmap.isNull()) {
        QMessageBox::warning(this, "Erreur", "Aucune image à afficher.");
        return;
    }

    // Créez une nouvelle fenêtre pour afficher l'image
    QDialog *imageDialog = new QDialog(this);
    imageDialog->setWindowTitle("Image Agrandie");

    // Configurez un QLabel dans la fenêtre
    QLabel *imageLabel = new QLabel(imageDialog);
    imageLabel->setPixmap(pixmap);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setScaledContents(true);

    // Ajustez la taille de la fenêtre
    imageDialog->resize(800, 600);  // Taille par défaut
    QVBoxLayout *layout = new QVBoxLayout(imageDialog);
    layout->addWidget(imageLabel);
    imageDialog->setLayout(layout);

    // Affichez la fenêtre sans bloquer l'exécution
    imageDialog->show();
}

