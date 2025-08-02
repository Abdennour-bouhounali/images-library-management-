#include "image.hpp"
#include <opencv2/opencv.hpp>
#include <QString>
#include <iostream>
#include <fstream>
#include <QDebug>
#include <QCoreApplication>

using namespace std; 
using namespace cv; 

// Constructeur de la classe Image : initialise une image à partir d'un chemin donné.

Image::Image(const QString& imgPath) {
    if (imgPath.isEmpty()) {
        qDebug() << "Error: Image path is empty.";
        return;
    }
    QString appPath = QCoreApplication::applicationDirPath();

    this->path = imgPath;
    qDebug() << "Loading image:" << imgPath;
    loadImage(appPath+imgPath);
}

// Charge une image depuis un chemin donné et initialise ses propriétés.

void Image::loadImage(const QString& imgPath) {
    Mat image = imread(imgPath.toStdString(), IMREAD_COLOR);
    this->content = image;

    if (image.empty()) {
        cerr << "Error while loading the image: " << imgPath.toStdString() << endl;
        exit(1);
    }

    int dot = imgPath.lastIndexOf('.');
    if (dot != -1) {
        this->format = imgPath.mid(dot + 1);
    }
        // Calcule le ratio de compression de l'image.

    this->compressionRatio = calculateCompressionRatio(imgPath);
}

// Retourne le contenu de l'image sous forme d'un objet OpenCV Mat.

Mat Image::getContent() const {
    return this->content;
}

// Calcule le ratio de compression de l'image (taille compressée / taille non compressée).

double Image::calculateCompressionRatio(const QString& imgPath) const {
    Mat image = cv::imread(imgPath.toStdString(), IMREAD_COLOR);

    if (image.empty()) {
        cerr << "Error reading the image!" << endl;
        return 0.0;
    }
    // Taille non compressée basée sur les dimensions et les canaux.

    size_t uncompressedSize = image.rows * image.cols * image.channels();
    // Taille compressée basée sur la taille réelle du fichier.

    ifstream file(imgPath.toStdString(), ios::binary | ios::ate);
    if (!file.is_open()) {
        cerr << "Error while opening the file to calculate the ratio." << endl;
        return 0.0;
    }

    size_t compressedSize = file.tellg();
    file.close();

    return static_cast<double>(compressedSize) / uncompressedSize;
}

// Affiche l'image dans une fenêtre en utilisant OpenCV.

void Image::showImage(const QString& imgPath) const {
    Mat image = imread(imgPath.toStdString(), IMREAD_COLOR);
    imshow("Image", image);
    waitKey(0);
}

// Retourne le format du fichier image (extension).

QString Image::getFormat() const {
    return this->format;
}

// Retourne le chemin actuel de l'image.

QString Image::getPath() const {
    return this->path;
}

// Retourne le ratio de compression de l'image.

double Image::getCompressionRatio() const {
    return this->compressionRatio;
}
// Retourne l'identifiant de l'image.

int Image::getId() const {
    return this->idImage;
}
// Met à jour le chemin de l'image.

void Image::setPath(const QString& newPath) {
    this->path = newPath;
}
// Met à jour l'identifiant de l'image.

void Image::setId(const int newID) {
    this->idImage = newID;
}

// Retourne l'image sous forme de QPixmap (pour l'intégration avec Qt).

QPixmap Image::getPixmap() const {
    if (content.empty()) {
        return QPixmap();
    }
    // Convertit l'image OpenCV en QImage, puis en QPixmap.

    QImage qImage(content.data, content.cols, content.rows, content.step, QImage::Format_RGB888);
    return QPixmap::fromImage(qImage.rgbSwapped());
}
