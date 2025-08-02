#include "librarymanagement.hpp"
#include "descriptor.hpp"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QIODevice>
#include <QTextStream>
#include <QJsonObject>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QCoreApplication>
#include <QDir>

ManageLibrary::ManageLibrary(int acces, Descriptor* head,QString libraryPath): acces(acces), head(head) , libraryPath(libraryPath) {};

Descriptor* ManageLibrary::getDescriptor(unsigned int idDesc) const {
    Descriptor* current = head;

    while(current != nullptr) {
        if(current->getIdDescriptor() == idDesc){
            return current;
        }
        current = current->getNextDescriptor();
    }

    //cout << "Error: Descriptor with ID" << idDesc << "not found." << endl ;
    return nullptr;
}

int ManageLibrary::getAcces() const {return this->acces;}

void ManageLibrary::addDescriptor() const {

}

void ManageLibrary::deleteDescriptor() const {}

Descriptor* ManageLibrary::searchDescriptor(unsigned int id) const {

    Descriptor* current = head;

    while(current != nullptr) {
        if(current->getIdDescriptor() == id){
            return current;
        }
        current = current->getNextDescriptor();
    }

    //cout << "Error: Image with ID" << id << "not found." <<endl ;
    return nullptr;

}

int ManageLibrary::totalDescriptors() const {

    int count = 0;
    Descriptor* current = head;
    while(current != nullptr){
        ++count;
        current = current->getNextDescriptor();
    }

    return count;
}

double ManageLibrary::displayCost(unsigned int id) const {

    Descriptor* current = head;

    while(current != nullptr) {
        if(current->getIdDescriptor() == id){
            return current->getCost();
        }
        current = current->getNextDescriptor();
    }

    // cout << "Error: Image with ID" << id << "not found." <<endl ;
    return -1.0;

}

Descriptor* ManageLibrary::getHead() const {
    return head;
}
void ManageLibrary::display() const {

    Descriptor* current = this->getHead();
    int i = 1;

    qDebug() << "Displaying ... ";
    while(current != nullptr){
        qDebug() << "Desc = " << i;
        current->display();
        current = current->getNextDescriptor();
        i++;
    }


}

double ManageLibrary::getMaxDescriptorCost() {
    double maxCost = 0.0;
    Descriptor* current = this->getHead();
    while (current != nullptr) {
        if (current->getCost() > maxCost) {
            maxCost = current->getCost();
        }
        current = current->getNextDescriptor();
    }
    return maxCost;
}

double ManageLibrary::getMinDescriptorCost() {
    double minCost = INFINITY;
    Descriptor* current = this->getHead();
    while (current != nullptr) {
        if (current->getCost() < minCost) {
            minCost = current->getCost();
        }
        current = current->getNextDescriptor();
    }
    return minCost;
}

void ManageLibrary::deleteDescriptor(Descriptor* descriptorToDelete) {
    if (!descriptorToDelete) {
        qDebug() << "The descriptor to delete is null. Operation aborted.";
        return;
    }

    qDebug() << "Deleting descriptor: " << descriptorToDelete->getIdDescriptor();

    QFile file(libraryPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Could not open file";
        return;
    }
    QString appPath = QCoreApplication::applicationDirPath();

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
        if (obj["id"].toInt() != descriptorToDelete->getIdDescriptor()) {
            newArray.append(obj);
        } else {
            imagePathToDelete = appPath +  obj["Imagepath"].toString();
        }
    }
    obj["library"] = newArray;

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Could not open file";
        return;
    }
    file.write(QJsonDocument(obj).toJson());
    file.close();

    // Delete the image file associated with the descriptor
    if (!imagePathToDelete.isEmpty()) {
        QFile imageFile(imagePathToDelete);
        if (imageFile.exists()) {
            if (!imageFile.remove()) {
                qDebug() << "Error: Could not delete image file";
            } else {
                qDebug() << "Image file deleted successfully";
            }
        } else {
            qDebug() << "Image file does not exist";
        }
    }

    // Remove the descriptor from the in-memory library object
    Descriptor* current = head;
    Descriptor* previous = nullptr;

    while (current != nullptr) {
        if (current->getIdDescriptor() == descriptorToDelete->getIdDescriptor()) {
            if (previous == nullptr) {
                // The descriptor to delete is the head of the list
                head = current->getNextDescriptor();
            } else {
                previous->setNextDescriptor(current->getNextDescriptor());
            }
            delete current;
            qDebug() << "Descriptor removed from in-memory library";
            return;
        }
        previous = current;
        current = current->getNextDescriptor();
    }

    qDebug() << "Descriptor not found in in-memory library";
}

ManageLibrary ManageLibrary::orderDescriptorsByCostDescending() {
    // Create a new library to hold the ordered descriptors
    ManageLibrary orderedLibrary = ManageLibrary(0, nullptr,"");

    // Traverse the current library and insert each descriptor into the new library in sorted order
    Descriptor* current = head;
    while (current != nullptr) {
        Descriptor* next = current->getNextDescriptor();
        insertDescriptorInOrder(orderedLibrary, current);
        current = next;
    }

    // Return the ordered library
    return orderedLibrary;
}


// Helper function to insert a descriptor into the new library in sorted order
void ManageLibrary::insertDescriptorInOrder(ManageLibrary& library, Descriptor* descriptor) {
    if (library.head == nullptr || library.head->getCost() < descriptor->getCost()) {
        // Insert at the beginning
        descriptor->setNextDescriptor(library.head);
        library.head = descriptor;
    } else {
        // Traverse the library to find the correct position
        Descriptor* current = library.head;
        while (current->getNextDescriptor() != nullptr && current->getNextDescriptor()->getCost() >= descriptor->getCost()) {
            current = current->getNextDescriptor();
        }
        descriptor->setNextDescriptor(current->getNextDescriptor());
        current->setNextDescriptor(descriptor);
    }
}
ManageLibrary ManageLibrary::orderDescriptorsByCostAscending() {
    // Create a new library to hold the ordered descriptors
    ManageLibrary orderedLibrary = ManageLibrary(0, nullptr,"");

    // Traverse the current library and insert each descriptor into the new library in sorted order
    Descriptor* current = head;
    while (current != nullptr) {
        Descriptor* next = current->getNextDescriptor();
        insertDescriptorInOrderAscending(orderedLibrary, current);
        current = next;
    }

    // Return the ordered library
    return orderedLibrary;
}

// Helper function to insert a descriptor into the new library in ascending order
void ManageLibrary::insertDescriptorInOrderAscending(ManageLibrary& library, Descriptor* descriptor) {
    if (library.head == nullptr || library.head->getCost() > descriptor->getCost()) {
        // Insert at the beginning
        descriptor->setNextDescriptor(library.head);
        library.head = descriptor;
    } else {
        // Traverse the library to find the correct position
        Descriptor* current = library.head;
        while (current->getNextDescriptor() != nullptr && current->getNextDescriptor()->getCost() <= descriptor->getCost()) {
            current = current->getNextDescriptor();
        }
        descriptor->setNextDescriptor(current->getNextDescriptor());
        current->setNextDescriptor(descriptor);
    }
}

void ManageLibrary::setHead(Descriptor* head) {
    this->head = head;
}

Descriptor* ManageLibrary::getDescriptorsByMaxCost(double maxCost) {
    Descriptor* filteredHead = nullptr;
    Descriptor* filteredTail = nullptr;
    Descriptor* current = head;

    qDebug() << "Filtering descriptors by max cost:" << maxCost;

    while (current != nullptr) {
        if (current->getCost() <= maxCost) {
            Descriptor* newDescriptor = new Descriptor(*current); // Create a deep copy
            newDescriptor->setNextDescriptor(nullptr); // Ensure it's isolated

            if (filteredHead == nullptr) {
                filteredHead = newDescriptor;
                filteredTail = newDescriptor;
            } else {
                filteredTail->setNextDescriptor(newDescriptor);
                filteredTail = newDescriptor;
            }
        }
        current = current->getNextDescriptor();
    }
    filteredTail->setNextDescriptor(nullptr);

    qDebug() << "Filtering complete. Returning filtered descriptors.";
    return filteredHead;
}

QString ManageLibrary::getLibraryPath() const {
    return libraryPath;
}

void ManageLibrary::setLibraryPath(QString path) {
    libraryPath = path;
}

void ManageLibrary::saveLibraryToJson(QString libraryName) {
    // Create the root JSON array
    QJsonArray libraryArray;
    QString appPath = QCoreApplication::applicationDirPath();

    // Traverse the linked list of descriptors
    Descriptor* current = head;
    while (current != nullptr) {
        QJsonObject descriptorObject;

        // Add each property to the JSON object
        descriptorObject["Imagepath"] = current->getImage().getPath(); // Assuming Image class has getImagePath()
        descriptorObject["access"] = QString(current->getAccess());         // Convert char to QString
        descriptorObject["cost"] = current->getCost();
        descriptorObject["id"] = static_cast<int>(current->getIdDescriptor());
        descriptorObject["source"] = current->getSource();
        descriptorObject["title"] = current->getTitle();
        

        // Append the object to the array
        libraryArray.append(descriptorObject);

        // Move to the next descriptor
        current = current->getNextDescriptor();
    }

    // Create the root JSON object
    QJsonObject rootObject;
    rootObject["library"] = libraryArray;

    // Convert to JSON document
    QJsonDocument jsonDoc(rootObject);

    // Write the JSON document to a file
    QFile file(appPath + "/../config/json_config/libraries.json" + libraryName + ".json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonDoc.toJson(QJsonDocument::Indented)); // Indented for readability
        file.close();
        qDebug() << "Library saved to library.json";
    } else {
        qWarning() << "Failed to save library: Unable to open file.";
    }
}

Descriptor* ManageLibrary::getDescriptorsBetweenMaxMinCost(double maxCost, double minCost) {
    Descriptor* current = head;
    Descriptor* newHead = nullptr;
    Descriptor* newTail = nullptr;

    while (current != nullptr) {
        // Filter descriptors by cost
        if (current->getCost() >= minCost && current->getCost() <= maxCost) {
            // Create a copy of the current descriptor
            Descriptor* newDescriptor = new Descriptor(*current);
            newDescriptor->setNextDescriptor(nullptr); // Ensure the new descriptor has no connections

            // Append the new descriptor to the new list
            if (newHead == nullptr) {
                newHead = newDescriptor;
                newTail = newDescriptor;
            } else {
                newTail->setNextDescriptor(newDescriptor);
                newTail = newDescriptor;
            }
        }
        current = current->getNextDescriptor();
    }

    return newHead;
}
