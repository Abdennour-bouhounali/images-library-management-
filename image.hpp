#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <opencv2/opencv.hpp>
#include <QString>
#include <QPixmap>
class Image {
public:
    Image(const QString& imgPath);

    void loadImage(const QString& imgPath);
    double calculateCompressionRatio(const QString& imgPath) const;
    void showImage(const QString& imgPath) const;

    QString getFormat() const;
    QString getPath() const;
    double getCompressionRatio() const;
    int getId() const;

    void setPath(const QString& newPath);
    void setId(const int newID);
    cv::Mat getContent() const;
    QPixmap getPixmap() const;



private:
    QString path;
    QString format;
    double compressionRatio;
    int idImage;
    cv::Mat content;
};

#endif // IMAGE_HPP
