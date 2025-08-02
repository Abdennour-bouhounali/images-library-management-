#ifndef LIBRARYMANAGEMENT_HPP
#define LIBRARYMANAGEMENT_HPP

#include <QString>
#include <math.h>
#include "descriptor.hpp"

using namespace std;


class ManageLibrary {

private:
    int acces;
    Descriptor* head ;
    QString libraryPath;


public:
    ManageLibrary(int acces, Descriptor* head,QString libraryPath);

    Descriptor* getDescriptor(unsigned int idDesc) const ;
    int getAcces() const ;

    void addDescriptor()     const;
    void deleteDescriptor()  const;
    Descriptor* searchDescriptor(unsigned int id)  const;
    void sortDescriptors()   const;
    int totalDescriptors()   const;
    void dipslayAllimages()  const;
    void display()           const;
    double displayCost(unsigned int id)     const;
    void createCostSubList() const;
    Descriptor* getHead() const; 
    void setHead(Descriptor* head);
    void deleteDescriptor(Descriptor* descriptorToDelete);
    double getMaxDescriptorCost();
    double getMinDescriptorCost();
    ManageLibrary orderDescriptorsByCostDescending();
    ManageLibrary orderDescriptorsByCostAscending();
    void insertDescriptorInOrder(ManageLibrary& library, Descriptor* descriptor);
    void insertDescriptorInOrderAscending(ManageLibrary& library, Descriptor* descriptor);

    Descriptor* getDescriptorsByMaxCost(double maxCost);
    QString getLibraryPath() const;
    void setLibraryPath(QString path);

    void deletDescriptorFromMemory(Descriptor* descriptorToDelete);
    void saveLibraryToJson(QString libraryName);

    Descriptor* getDescriptorsBetweenMaxMinCost(double maxCost, double minCost);



};

#endif
