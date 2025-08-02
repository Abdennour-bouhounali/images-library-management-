#ifndef DESCRIPTOR_HPP
#define DESCRIPTOR_HPP
#include "image.hpp"
#include <QJsonObject>

class Descriptor {

private:
    unsigned int idDes;
    Descriptor* nextDescriptor;
    double cost;
    QString title;
    QString source;
    char access;
    Image image;

public:
    Descriptor(const Image& img);
    Descriptor(int idDesc, const Image& img);
    Descriptor(int idDesc, double costValue, const Image& img);
    Descriptor(int idDesc, double costValue, const QString& descTitle, const Image& img);
    Descriptor(int idDesc, double costValue, const QString& descTitle, const QString& descSource, const Image& img);
    Descriptor(int idDesc, double costValue, const QString& descTitle, const QString& descSource, const char descAccess, const Image& img);

    unsigned int getIdDescriptor() const;
    double getCost() const;
    QString getTitle() const;
    QString getSource() const;
    char getAccess() const;
    Image getImage() const;
    Descriptor* getNextDescriptor() const;

    void setIdDescriptor(int newIdDes);
    void setCost(double newCost);
    void setTitle(const QString& descTitle);
    void setSource(const QString& descSource);
    void setAccess(const char& descAccess);
    void setImage(const Image& img);
    void setNextDescriptor(Descriptor* nextDesc);

    void display() const;
    QPixmap cvMatToQPixmap(const cv::Mat& mat) const;
    cv::Mat QPixmapToCvMat(const QPixmap& pixmap) const;
    QJsonObject toJson() const;
};

#endif

