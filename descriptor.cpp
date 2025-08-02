#include "descriptor.hpp"
#include <QJsonArray>

Descriptor::Descriptor(const Image& img)
    : idDes(0), cost(0), title("UKNOWN"),
      source("UKNOWN"), access('L'), image(img), nextDescriptor(nullptr) {}

Descriptor::Descriptor(int idDesc, const Image& img)
    : idDes(idDesc), cost(0), title("UKNOWN"),
      source("UKNOWN"), access('L'), image(img), nextDescriptor(nullptr) {}

Descriptor::Descriptor(int idDesc, double costValue, const Image& img)
    : idDes(idDesc), cost(costValue), title("UKNOWN"),
      source("UKNOWN"), access('L'), image(img), nextDescriptor(nullptr) {}

Descriptor::Descriptor(int idDesc, double costValue, const QString& descTitle, const Image& img)
    : idDes(idDesc), cost(costValue), title(descTitle),
      source("UKNOWN"), access('L'), image(img), nextDescriptor(nullptr) {}

Descriptor::Descriptor(int idDesc, double costValue, const QString& descTitle,
                       const QString& descSource, const Image& img)
    : idDes(idDesc), cost(costValue), title(descTitle),
      source(descSource), access('L'), image(img), nextDescriptor(nullptr) {}

Descriptor::Descriptor(int idDesc, double costValue, const QString& descTitle,
                       const QString& descSource, const char descAccess, const Image& img)
    : idDes(idDesc), cost(costValue), title(descTitle),
      source(descSource), access(descAccess), image(img), nextDescriptor(nullptr) {}




unsigned int Descriptor::getIdDescriptor() const { return this->idDes; }
double Descriptor::getCost() const { return this->cost; }
QString Descriptor::getTitle() const { return this->title; }
QString Descriptor::getSource() const { return this->source; }
char Descriptor::getAccess() const { return this->access; }
Image Descriptor::getImage() const { return this->image; }

Descriptor* Descriptor::getNextDescriptor() const { return this->nextDescriptor; }

void Descriptor::setIdDescriptor(int newIdDes) { this->idDes = newIdDes; }
void Descriptor::setCost(double newCost)  { this->cost = newCost; }
void Descriptor::setTitle(const QString& descTitle) { this->title = descTitle; }
void Descriptor::setSource(const QString& descSource) { this->source = descSource; }
void Descriptor::setAccess(const char& descAccess) { this->access = descAccess; }
void Descriptor::setImage(const Image& img) { this->image = img; }

void Descriptor::setNextDescriptor(Descriptor* nextDesc) { this->nextDescriptor = nextDesc; }

QPixmap Descriptor::cvMatToQPixmap(const cv::Mat &mat) const {
    // Step 1: Convert cv::Mat to QImage
    QImage img;
    if (mat.channels() == 1) {
        // Grayscale image
        img = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
    } else if (mat.channels() == 3) {
        // Convert BGR to RGB
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        img = QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
    } else if (mat.channels() == 4) {
        // Direct BGRA to QImage with alpha
        img = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
    } else {
        throw std::invalid_argument("Unsupported cv::Mat format.");
    }

    // Ensure the QImage is valid before converting to QPixmap
    if (img.isNull()) {
        throw std::runtime_error("Failed to convert cv::Mat to QImage");
    }

    // Step 2: Convert QImage to QPixmap
    QPixmap pixmap = QPixmap::fromImage(img);
    if (pixmap.isNull()) {
        throw std::runtime_error("Failed to convert QImage to QPixmap");
    }

    return pixmap;
}
cv::Mat Descriptor::QPixmapToCvMat(const QPixmap &pixmap) const {
    QImage qImage = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);

    cv::Mat mat(qImage.height(), qImage.width(), CV_8UC4, const_cast<uchar*>(qImage.bits()), qImage.bytesPerLine());

    // Convert ARGB to BGR
    cv::Mat matBGR;
    cv::cvtColor(mat, matBGR, cv::COLOR_BGRA2BGR);

    return matBGR;
}
void Descriptor::display() const {
    qDebug()  << "Descriptor ID: " << idDes ;

}


QJsonObject Descriptor::toJson() const {
    QJsonObject json;
    json["Imagepath"] = image.getPath();
    json["access"] = QString(access);
    json["cost"] = cost;
    json["id"] = static_cast<int>(idDes);
    json["source"] = source;
    json["title"] = title;
   
    return json;
}
