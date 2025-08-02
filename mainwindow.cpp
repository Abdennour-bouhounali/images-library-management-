#include "mainwindow.h"
#include "loginwindow.hpp"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include "librarymanagement.hpp"
#include "descriptor.hpp"
#include "add_new_descriptor.hpp"
#include <QJsonObject>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QCoreApplication>
#include <QJsonDocument>



MainWindow::MainWindow(User user, QWidget *parent, ManageLibrary mainlibrary, ManageLibrary sublibrary)
    : QMainWindow(parent), ui(new Ui::Home), currentUser(user), mainlibrary(ManageLibrary(0, nullptr, "")), sublibrary(ManageLibrary(0, nullptr, ""))
{
    // this->setFixedSize(1200, 800); // Width: 1200, Height: 800

    ui->setupUi(this);
    // this->mainlibrary = ManageLibrary(0, nullptr);

    descriptorDetails = new DescriptorDetails(this, currentUser.access);
    if (currentUser.access)
    {
        ui->menubar->setVisible(true);
    }
    else
    {
        ui->menubar->setVisible(false);
    }

    // Initialize the grid layout
    gridLayout = new QGridLayout();
    ui->librariesLayout->setLayout(gridLayout); 
    loadLibrariesButtons();
    ui->LogoutButton->setVisible(true);

    ui->returnButton->setVisible(false);
    connect(ui->returnButton, &QPushButton::clicked, this, &MainWindow::on_returnButton_clicked);

    ui->saveSubListButton->setEnabled(false); // Désactive le bouton

    // Apply styles to the specified UI elements
    ui ->LogoutButton->setStyleSheet("QPushButton {"
                                            "background-color: rgb(153, 193, 241);"
                                            "color: white;"
                                            "border: none;"
                                            "border-radius: 5px;"
                                            "padding: 8px 12px;"
                                            "font-size: 14px;"
                                            "font-weight: bold;"
                                            "}"
                                            "QPushButton:hover {"
                                            "background-color: rgb(123, 163, 211);"
                                            "}"
                                            "QPushButton:pressed {"
                                            "background-color: #003f7f;"
                                            "padding-left: 12px;"
                                            "padding-top: 12px;"
                                            "}");
    ui->CreateSubListLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #333;");
    ui->LoadLibraryLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #333;");
    ui->ImageIdSearchInput->setStyleSheet("padding: 10px; border: 1px solid #ccc; border-radius: 5px;");
    ui->SearchButton->setStyleSheet("QPushButton {"
                                            "background-color: rgb(153, 193, 241);"
                                            "color: white;"
                                            "border: none;"
                                            "border-radius: 5px;"
                                            "padding: 8px 12px;"
                                            "font-size: 14px;"
                                            "font-weight: bold;"
                                            "}"
                                            "QPushButton:hover {"
                                            "background-color: rgb(123, 163, 211);"
                                            "}"
                                            "QPushButton:pressed {"
                                            "background-color: #003f7f;"
                                            "padding-left: 12px;"
                                            "padding-top: 12px;"
                                            "}");
    ui->comboBox_libraries->setStyleSheet("padding: 5px; border: 1px solid #ccc; border-radius: 5px; background-color: #fff; color: #333;");
    ui->ClearFilterButton->setVisible(false);

    // Connect the combo box's currentIndexChanged signal to the slot
    connect(ui->comboBox_libraries, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
            {
        QString libraryPath = ui->comboBox_libraries->itemData(index).toString();
        if (!libraryPath.isEmpty()) {
            LoadTheLibrary(libraryPath);
        } });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadLibrariesButtons()
{

    // Clear existing items from the combo box
    ui->comboBox_libraries->clear();

    QString appPath = QCoreApplication::applicationDirPath();
    QString librariesFilePath = appPath + "/libraries.json";

    // Load libraries from the JSON file
    QJsonArray jsonArray = currentUser.loadLibraries(librariesFilePath);
    if (jsonArray.isEmpty())
    {
        QMessageBox::warning(this, "No Libraries Found",
                             "No libraries could be loaded from libraries.json. Please check the file.");
        // qDebug() << "No libraries found in the file.";
        return; // Exit if there are no libraries
    }

    // qDebug() << "Loaded libraries: ";

    // Populate the combo box with library names and paths
    for (const QJsonValue &value : jsonArray)
    {
        if (!value.isObject())
            continue; // Skip non-object entries

        QJsonObject libraryObj = value.toObject();
        QString libraryName = libraryObj.value("name").toString();
        QString libraryPath = appPath + libraryObj.value("path").toString();

        if (libraryName.isEmpty() || libraryPath.isEmpty())
        {
            // qDebug() << "Invalid library entry: " << libraryObj;
            continue; // Skip invalid entries
        }

        ui->comboBox_libraries->addItem(libraryName, libraryPath);
    }

    if (ui->comboBox_libraries->count() > 0)
    {
        QString libraryPath = ui->comboBox_libraries->itemData(0).toString();
        if (!libraryPath.isEmpty())
        {
            LoadTheLibrary(libraryPath); // Load the first library by default
        }
    }
}

void MainWindow::on_actionLoad_a_Library_triggered()
{
    // Enter the library to import
    QString path = QFileDialog::getOpenFileName(this, "Open Library", "", "JSON files (*.json)");
    // print the path in the terminal
    // qDebug() << path;
    // show the library
    LoadTheLibrary(path);
}

void MainWindow::setCurrentLibraryPath(QString path){
    this->currentLibraryPath = path;
}

void MainWindow::LoadTheLibrary(QString path)
{
    this->setCurrentLibraryPath(path);

    // Reload the library from the file system
    ManageLibrary library = currentUser.loadLibrary(path);
    mainlibrary = library;

    // qDebug() << "Library Created";

    // If the library is empty
    if (library.getHead() == nullptr)
    {
        // qDebug() << "The library is empty";
        clearGridLayout();
        // ui->emptyLibrary->setText("The library is empty.");
        return;
    }

    // ui->emptyLibrary->setText("");
    double maxCost = mainlibrary.getMaxDescriptorCost();
    double minCost = mainlibrary.getMinDescriptorCost();

    // Clear the existing items in the grid layout
    clearGridLayout();

    // Vérification du chemin de l'image
    QString imagePath = library.getLibraryPath(); // Utilise la méthode getLibraryPath pour obtenir le chemin
    if (QFile::exists(imagePath)) {
        // qDebug() << "Loading image: " << imagePath;
        // Continue to load image here
        populateGridLayout(library.getHead());
    } else {
        // Handle error if image cannot be loaded
        // qDebug() << "Error while loading the image: " << imagePath;
        QMessageBox::warning(this, "Image Error", "Unable to load the image. Please check the file path.");
        // Optionally, clear grid layout or set a default state
    }
}



void MainWindow::clearGridLayout()
{
    while (QLayoutItem *item = gridLayout->takeAt(0))
    {
        if (QWidget *widget = item->widget())
        {
            widget->deleteLater(); // Ensure proper deletion of the widget
        }
        delete item; // Delete the layout item
    }
}

void MainWindow::populateGridLayout(Descriptor *head)
{
    int row = 0;
    int col = 0;
    Descriptor *current = head;
    QString appPath = QCoreApplication::applicationDirPath();

    while (current != nullptr)
    {
        if(current->getAccess() == 'L' && !currentUser.access){
            current = current->getNextDescriptor();
            continue;
        }
        // Create a vertical layout for each cell
        QVBoxLayout *cellLayout = new QVBoxLayout();
        cellLayout->setContentsMargins(10, 10, 10, 10);
        cellLayout->setSpacing(10);

        // Create and add the image label
        QLabel *imageLabel = new QLabel();
        // qDebug() << "Loading Image : " << appPath + current->getImage().getPath();
        QPixmap pixmap(appPath + current->getImage().getPath());
        if (pixmap.isNull())
        {
            qWarning() << "Failed to load image: " << current->getImage().getPath();
        }
        imageLabel->setPixmap(pixmap.scaled(210, 210, Qt::KeepAspectRatio));
        imageLabel->setStyleSheet("border: 1px solid #ccc; padding: 5px;");
        cellLayout->addWidget(imageLabel);

        // // Create and add the information label
        QLabel *infoLabel = new QLabel();
        QString infoText = QString("ID: %1").arg(current->getIdDescriptor());
        infoLabel->setText(infoText);
        infoLabel->setStyleSheet("background-color: #f9f9f9; padding: 10px; border-radius: 5px;");
        infoLabel->setFixedSize(240, 33);

        cellLayout->addWidget(infoLabel);

        // Create an info button
            QPushButton *infoButton = new QPushButton("Show/Hide Info", this);
            infoButton->setStyleSheet(  "QPushButton {"
                                            "background-color: rgb(153, 193, 241);"
                                            "color: white;"
                                            "border: none;"
                                            "border-radius: 5px;"
                                            "padding: 8px 12px;"
                                            "font-size: 14px;"
                                            "font-weight: bold;"
                                            "}"
                                            "QPushButton:hover {"
                                            "background-color: rgb(123, 163, 211);"
                                            "}"
                                            "QPushButton:pressed {"
                                            "background-color: #003f7f;"
                                            "padding-left: 12px;"
                                            "padding-top: 12px;"
                                            "}");
            cellLayout->addWidget(infoButton);

        // If the user has access, create a delete button
        if (getCurrentUser().access)
        {
            QPushButton *deleteButton = new QPushButton("Delete", this);
            deleteButton->setStyleSheet(  "QPushButton {"
                                            "background-color: rgb(153, 193, 241);"
                                            "color: white;"
                                            "border: none;"
                                            "border-radius: 5px;"
                                            "padding: 8px 12px;"
                                            "font-size: 14px;"
                                            "font-weight: bold;"
                                            "}"
                                            "QPushButton:hover {"
                                            "background-color: rgb(123, 163, 211);"
                                            "}"
                                            "QPushButton:pressed {"
                                            "background-color: #003f7f;"
                                            "padding-left: 12px;"
                                            "padding-top: 12px;"
                                            "}");
            cellLayout->addWidget(deleteButton);

            // Use a lambda to delete the descriptor
            connect(deleteButton, &QPushButton::clicked, this, [this, descriptor = current]()
                    {
                        mainlibrary.deleteDescriptor(descriptor);
                        ShowTheLibrary(mainlibrary); // Reload the library after deletion
                    });

            QPushButton *editButton = new QPushButton("Edit", this);
            editButton->setStyleSheet(  "QPushButton {"
                                            "background-color: rgb(153, 193, 241);"
                                            "color: white;"
                                            "border: none;"
                                            "border-radius: 5px;"
                                            "padding: 8px 12px;"
                                            "font-size: 14px;"
                                            "font-weight: bold;"
                                            "}"
                                            "QPushButton:hover {"
                                            "background-color: rgb(123, 163, 211);"
                                            "}"
                                            "QPushButton:pressed {"
                                            "background-color: #003f7f;"
                                            "padding-left: 12px;"
                                            "padding-top: 12px;"
                                            "}");
            cellLayout->addWidget(editButton);

            // Connect the Edit button to display a QMessageBox
            connect(editButton, &QPushButton::clicked, this, [this, current]()
                {

                    unsigned int originalId = current->getIdDescriptor();

                    QDialog dialog(this);
                    dialog.setWindowTitle("Edit Image Info");
                    dialog.setModal(true);

                    QLineEdit *idEdit = new QLineEdit(QString::number(current->getIdDescriptor()), &dialog);
                    QLineEdit *titleEdit = new QLineEdit(current->getTitle(), &dialog);
                    QLineEdit *sourceEdit = new QLineEdit(current->getSource(), &dialog);
                    QLineEdit *costEdit = new QLineEdit(QString::number(current->getCost()), &dialog);

                    QComboBox *accessCombo = new QComboBox(&dialog);
                    accessCombo->addItem("L");
                    accessCombo->addItem("O");
                    accessCombo->setCurrentText(QString(current->getAccess()));
                    // Créer un layout pour organiser les champs
                    QFormLayout *formLayout = new QFormLayout();
                    formLayout->addRow("ID:", idEdit);
                    formLayout->addRow("Title:", titleEdit);
                    formLayout->addRow("Source:", sourceEdit);
                    formLayout->addRow("Cost:", costEdit);
                    formLayout->addRow("Access:", accessCombo);


                    // Ajouter les boutons
                    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);

                    // Connecter les boutons
                    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
                    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

                    // Organiser le tout dans un layout principal
                    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
                    mainLayout->addLayout(formLayout);
                    mainLayout->addWidget(buttonBox);

                    // Afficher la boîte de dialogue
                    if (dialog.exec() == QDialog::Accepted) {
                        // Mettre à jour les informations
                        current->setIdDescriptor(idEdit->text().toInt());                        
                        current->setTitle(titleEdit->text());
                        current->setSource(sourceEdit->text());
                        current->setCost(costEdit->text().toDouble());
                        current->setAccess(accessCombo->currentText().toStdString()[0]); // Récupérer la valeur sélectionnée

                        SaveChanges_clicked(current, originalId);

                        ShowTheLibrary(mainlibrary); // Rafraîchir l'affichage 
                    }
                });
            } 


                bool *isInfoVisible = new bool(false); // Initial state: info hidden

                // Connect the info button
                connect(infoButton, &QPushButton::clicked, this, [infoLabel, current, isInfoVisible]()
                    {
                        if (*isInfoVisible) {
                        // Hide additional information
                        infoLabel->setText(QString("ID: %1").arg(current->getIdDescriptor()));
                        infoLabel->setFixedSize(240, 33); // Revenir à la taille initiale
                        *isInfoVisible = false;

                        } else {
                            // Show additional information
                            QString additionalInfo = QString("\nCost: %1\nTitle: %2\nSource: %3\nAccess: %4")
                                             .arg(current->getCost())         // Cost
                                             .arg(current->getTitle())        // Titre
                                             .arg(current->getSource())       // Source
                                             .arg(current->getAccess());        // Access
                            infoLabel->setText(infoLabel->text() + additionalInfo);

                            infoLabel->setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
                            infoLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred); // Autoriser l'expansion
                            *isInfoVisible = true;
            }
        });

        // Create a widget to hold the cell layout and add it to the grid layout
        QWidget *cellWidget = new QWidget();
        cellWidget->setLayout(cellLayout);
        cellWidget->setFixedSize(250, 350);
        cellWidget->setStyleSheet("background-color: #ffffff; border: 1px solid #ddd; border-radius: 10px; padding: 10px;");
        gridLayout->addWidget(cellWidget, row, col);

        // Store the connection between the widget and the descriptor
        widgetDescriptorMap[cellWidget] = current;

        // Install an event filter for the widget
        cellWidget->installEventFilter(this);

        // Update row and column for grid layout
        col++;
        if (col >= 3)
        {
            col = 0;
            row++;
        }

        current = current->getNextDescriptor();
    }
}

void MainWindow::cleanUpDescriptors(Descriptor *head)
{
    Descriptor *current = head;
    while (current != nullptr)
    {
        Descriptor *temp = current;
        current = current->getNextDescriptor();
        delete temp;
    }
}

void MainWindow::ShowTheLibrary(ManageLibrary library)
{
    // qDebug() << "To show the library";

    // If the library is empty
    if (library.getHead() == nullptr)
    {
        // qDebug() << "The library is empty";
        clearGridLayout();
        QMessageBox::warning(this, "Warning", "The library is empty.");
        return;
    }

    // Clear the existing items in the grid layout
    clearGridLayout();

    // Populate the grid layout with images and their information
    populateGridLayout(library.getHead());
}

User MainWindow::getCurrentUser()
{
    return this->currentUser;
};
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QWidget *widget = qobject_cast<QWidget *>(obj);
        if (widget && widgetDescriptorMap.contains(widget))
        {
            Descriptor *descriptor = widgetDescriptorMap[widget];
            // qDebug() << "library Path in details" << this->currentLibraryPath;
            descriptorDetails->setLibraryPath(this->currentLibraryPath);
            descriptorDetails->setDescriptor(descriptor);
            descriptorDetails->show();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::refreshLibrary()
{
}

void MainWindow::on_CreateNewLibrary_triggered()
{
    bool ok;
    QString libraryName = QInputDialog::getText(this, tr("Create New Library"),
                                                tr("Library Name:"), QLineEdit::Normal, "", &ok);

    if (ok && !libraryName.isEmpty())
    {
        currentUser.createLibrary(libraryName);

        // Block signals temporarily to avoid triggering `currentIndexChanged`
        ui->comboBox_libraries->blockSignals(true);

        ui->comboBox_libraries->clear();
        QString appPath = QCoreApplication::applicationDirPath();
        QString librariesFilePath = appPath + "/libraries.json";
        QJsonArray jsonArray = currentUser.loadLibraries(librariesFilePath);

        for (const QJsonValue &value : jsonArray)
        {
            QJsonObject libraryObj = value.toObject();
            QString libraryName = libraryObj["name"].toString();
            QString libraryPath = libraryObj["path"].toString();

            // Validate and log paths
            QString fullPath = appPath + libraryPath;
            // qDebug() << "Adding to combo box: " << libraryName << " -> " << fullPath;

            ui->comboBox_libraries->addItem(libraryName, fullPath); // Store the absolute path
        }

        // Unblock signals after populating
        ui->comboBox_libraries->blockSignals(false);
    }
    else
    {
        QMessageBox::warning(this, tr("Invalid Input"), tr("Library name cannot be empty."));
    }
}

void MainWindow::on_actionAdd_New_Descriptor_triggered()
{
    // qDebug() << "In MainWindow::on_add_new_description_clicked():";
    // qDebug() << "---------------------------------------";
    // qDebug() << MainWindow::getCurrentLibraryId();
    Add_New_Descriptor addDescriptorDialog(mainlibrary.getLibraryPath(), this);
    addDescriptorDialog.exec();
    // refresh the ui to show the new descriptor
    LoadTheLibrary(mainlibrary.getLibraryPath());
}

void MainWindow::on_actionDelete_a_library_triggered()
{
    // Load the libraries from the JSON file
    QString appPath = QCoreApplication::applicationDirPath();

    QString librariesFilePath = appPath + "/libraries.json";
    QJsonArray jsonArray = currentUser.loadLibraries(librariesFilePath);

    // Prepare a list of library names
    QStringList libraryNames;
    QMap<QString, QString> libraryPaths; // Map to store library names and their paths

    for (const QJsonValue &value : jsonArray)
    {
        QJsonObject libraryObj = value.toObject();
        QString libraryName = libraryObj["name"].toString();
        QString libraryPath = libraryObj["path"].toString();
        libraryNames.append(libraryName);        // Add library name to the list
        libraryPaths[libraryName] = libraryPath; // Store the corresponding library path
    }

    // Show a dialog to let the user choose which library to delete
    bool ok;
    QString libraryNameToDelete = QInputDialog::getItem(this, tr("Delete a Library"),
                                                        tr("Select Library to Delete:"),
                                                        libraryNames, 0, false, &ok);

    // If the user selected a library and clicked OK
    if (ok && !libraryNameToDelete.isEmpty())
    {
        // Get the path of the selected library
        QString libraryPath = libraryPaths[libraryNameToDelete];

        // Call deleteLibrary to delete the library
        currentUser.deleteLibrary(libraryNameToDelete);

        // Refresh the UI after deletion
        // qDebug() << "The library has been deleted:" << libraryNameToDelete;
        loadLibrariesButtons(); // Reload the buttons

        // Optionally, refresh or reload other parts of the UI
        // refreshLibrary(); // Uncomment if needed
    }
    else
    {
        // qDebug() << "No library was selected for deletion.";
    }
}

void MainWindow::on_SearchButton_clicked()
{
    QString ImageId = ui->ImageIdSearchInput->text();
    bool imageFound = false;
    Descriptor *current = mainlibrary.getHead();
    QString appPath = QCoreApplication::applicationDirPath();

    while (current != nullptr)
    {
        if(current->getAccess() == 'L' && !currentUser.access){
            current = current->getNextDescriptor();
            continue;
        }

        // check if the current descriptor id is equal to the id entered by the user
        if (current->getIdDescriptor() == ImageId.toInt())
        {
            imageFound = true;

            // Show the return button
            ui->returnButton->setVisible(true);
            // clear the grid layout
            QLayoutItem *item;
            while ((item = gridLayout->takeAt(0)) != nullptr)
            {
                delete item->widget();
                delete item;
            }

            // Create a vertical layout for each cell
            QVBoxLayout *cellLayout = new QVBoxLayout();
            cellLayout->setContentsMargins(10, 10, 10, 10);
            cellLayout->setSpacing(10);

            // Create and add the image label
            QLabel *imageLabel = new QLabel();
            QPixmap pixmap(appPath + current->getImage().getPath());
            imageLabel->setPixmap(pixmap.scaled(210, 210, Qt::KeepAspectRatio)); // Adjust the size as needed
            imageLabel->setStyleSheet("border: 1px solid #ccc; padding: 5px;");
            cellLayout->addWidget(imageLabel);

            // // Create and add the information label
            QLabel *infoLabel = new QLabel();
            QString infoText = QString("ID: %1").arg(current->getIdDescriptor());
            infoLabel->setText(infoText);
            infoLabel->setStyleSheet("background-color: #f9f9f9; padding: 10px; border-radius: 5px;");
            infoLabel->setFixedSize(240, 33);
            cellLayout->addWidget(infoLabel);

            // Create an info button
            QPushButton *infoButton = new QPushButton("Show/Hide Info", this);
            infoButton->setStyleSheet(  "QPushButton {"
                                            "background-color: rgb(153, 193, 241);"
                                            "color: white;"
                                            "border: none;"
                                            "border-radius: 5px;"
                                            "padding: 8px 12px;"
                                            "font-size: 14px;"
                                            "font-weight: bold;"
                                            "}"
                                            "QPushButton:hover {"
                                            "background-color: rgb(123, 163, 211);"
                                            "}"
                                            "QPushButton:pressed {"
                                            "background-color: #003f7f;"
                                            "padding-left: 12px;"
                                            "padding-top: 12px;"
                                            "}");
            cellLayout->addWidget(infoButton);

            if (getCurrentUser().access)
            {
                QPushButton *deleteButton = new QPushButton("Delete", this);
                deleteButton->setStyleSheet(  "QPushButton {"
                                            "background-color: rgb(153, 193, 241);"
                                            "color: white;"
                                            "border: none;"
                                            "border-radius: 5px;"
                                            "padding: 8px 12px;"
                                            "font-size: 14px;"
                                            "font-weight: bold;"
                                            "}"
                                            "QPushButton:hover {"
                                            "background-color: rgb(123, 163, 211);"
                                            "}"
                                            "QPushButton:pressed {"
                                            "background-color: #003f7f;"
                                            "padding-left: 12px;"
                                            "padding-top: 12px;"
                                            "}");
                cellLayout->addWidget(deleteButton);

                // Use a lambda to delete the descriptor
                connect(deleteButton, &QPushButton::clicked, this, [this, descriptor = current]()
                    {
                        mainlibrary.deleteDescriptor(descriptor);
                        ShowTheLibrary(mainlibrary); // Reload the library after deletion
                    });
                QPushButton *editButton = new QPushButton("Edit", this);
                editButton->setStyleSheet(  "QPushButton {"
                                            "background-color: rgb(153, 193, 241);"
                                            "color: white;"
                                            "border: none;"
                                            "border-radius: 5px;"
                                            "padding: 8px 12px;"
                                            "font-size: 14px;"
                                            "font-weight: bold;"
                                            "}"
                                            "QPushButton:hover {"
                                            "background-color: rgb(123, 163, 211);"
                                            "}"
                                            "QPushButton:pressed {"
                                            "background-color: #003f7f;"
                                            "padding-left: 12px;"
                                            "padding-top: 12px;"
                                            "}");
            cellLayout->addWidget(editButton);

            connect(editButton, &QPushButton::clicked, this, [this, current]()
                {

                    unsigned int originalId = current->getIdDescriptor();

                    QDialog dialog(this);
                    dialog.setWindowTitle("Edit Image Info");
                    dialog.setModal(true);

                    QLineEdit *idEdit = new QLineEdit(QString::number(current->getIdDescriptor()), &dialog);
                    QLineEdit *titleEdit = new QLineEdit(current->getTitle(), &dialog);
                    QLineEdit *sourceEdit = new QLineEdit(current->getSource(), &dialog);
                    QLineEdit *costEdit = new QLineEdit(QString::number(current->getCost()), &dialog);

                    QComboBox *accessCombo = new QComboBox(&dialog);
                    accessCombo->addItem("L");
                    accessCombo->addItem("O");
                    accessCombo->setCurrentText(QString(current->getAccess()));
                    // Créer un layout pour organiser les champs
                    QFormLayout *formLayout = new QFormLayout();
                    formLayout->addRow("ID:", idEdit);
                    formLayout->addRow("Title:", titleEdit);
                    formLayout->addRow("Source:", sourceEdit);
                    formLayout->addRow("Cost:", costEdit);
                    formLayout->addRow("Access:", accessCombo);


                    // Ajouter les boutons
                    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);

                    // Connecter les boutons
                    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
                    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

                    // Organiser le tout dans un layout principal
                    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
                    mainLayout->addLayout(formLayout);
                    mainLayout->addWidget(buttonBox);

                    // Afficher la boîte de dialogue
                    if (dialog.exec() == QDialog::Accepted) {
                        // Mettre à jour les informations
                        current->setIdDescriptor(idEdit->text().toInt());                        
                        current->setTitle(titleEdit->text());
                        current->setSource(sourceEdit->text());
                        current->setCost(costEdit->text().toDouble());
                        current->setAccess(accessCombo->currentText().toStdString()[0]); // Récupérer la valeur sélectionnée

                        SaveChanges_clicked(current, originalId);

                        ShowTheLibrary(mainlibrary); // Rafraîchir l'affichage 
                    }
                });
            } 

            bool *isInfoVisible = new bool(false); 

                // Connect the info button
                connect(infoButton, &QPushButton::clicked, this, [infoLabel, current, isInfoVisible]()
                    {
                        if (*isInfoVisible) {
                        // Hide additional information
                        infoLabel->setText(QString("ID: %1").arg(current->getIdDescriptor()));
                        infoLabel->setFixedSize(240, 33); // Revenir à la taille initiale
                        *isInfoVisible = false;

                        } else {
                            // Show additional information
                            QString additionalInfo = QString("\nCost: %1\nTitle: %2\nSource: %3\nAccess: %4")
                                             .arg(current->getCost())         // Cost
                                             .arg(current->getTitle())        // Titre
                                             .arg(current->getSource())       // Source
                                             .arg(current->getAccess());        // Access
                            infoLabel->setText(infoLabel->text() + additionalInfo);

                            infoLabel->setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
                            infoLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred); // Autoriser l'expansion
                            *isInfoVisible = true;
                    }
        });

            // Create a widget to hold the cell layout and add it to the grid layout
            QWidget *cellWidget = new QWidget();
            cellWidget->setLayout(cellLayout);
            cellWidget->setFixedSize(250, 350); // Set fixed size for each descriptor
            cellWidget->setStyleSheet("background-color: #ffffff; border: 1px solid #ddd; border-radius: 10px; padding: 10px;");
            gridLayout->addWidget(cellWidget, 0, 0);

            // Store the connection between the widget and the descriptor
            widgetDescriptorMap[cellWidget] = current;

            // Connect the click event to the slot
            cellWidget->installEventFilter(this);
        }
        current = current->getNextDescriptor();
    }

    if (!imageFound)
    {
        QMessageBox::warning(this, "Error", "No image found with this ID.");
    }
}
void MainWindow::on_returnButton_clicked()
{
    ShowTheLibrary(mainlibrary);
    // Show the return button
    ui->returnButton->setVisible(false);
}



void MainWindow::on_DescendingButton_clicked()
{
    // order the descriptors by cost in descending order
    ManageLibrary orderedLibrary = mainlibrary.orderDescriptorsByCostDescending();
    mainlibrary = orderedLibrary;
    // refresh the library
    ShowTheLibrary(mainlibrary);
}

void MainWindow::on_AscendingButton_clicked()
{
    ManageLibrary orderedLibrary = mainlibrary.orderDescriptorsByCostAscending();
    mainlibrary = orderedLibrary;
    // refresh the library
    ShowTheLibrary(mainlibrary);
}

void MainWindow::on_saveSubListButton_clicked()
{
    // show a box to enter the name of the library
    bool ok;
    QString libraryName = QInputDialog::getText(this, tr("Save Sublibrary"),
                                                tr("Sublibrary Name:"), QLineEdit::Normal, "", &ok);

    // Save the sublibrary to a JSON file

    sublibrary.saveLibraryToJson(libraryName);
    // save the library name and path in libraries.json file
    QString appPath = QCoreApplication::applicationDirPath();
    QString librariesFilePath = appPath + "/libraries.json";

    QJsonObject library;
    library["name"] = libraryName;
    library["path"] = "/Libraries/" + libraryName + ".json";
    // add to the libraries.json file
    QFile file(librariesFilePath);

    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this,"Error:"," Could not open file");
    }

    QByteArray data = file.readAll();
    // add library to data
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj = doc.object();
    QJsonArray array = obj["libraries"].toArray();
    array.append(library);
    obj["libraries"] = array;
    file.close();
    // write the updated data to the file
    if (!file.open(QIODevice::WriteOnly))
    {
        // qDebug() << "Error: Could not open file";
        exit(1);
    }
    file.write(QJsonDocument(obj).toJson());
    file.close();

    QMessageBox::information(this, "Success", "Sublibrary saved successfully.");
    loadLibrariesButtons();
}
void MainWindow::on_ClearFilterButton_clicked()
{
    // Réinitialiser les champs de saisie
    ui->MaxInput->clear();
    ui->MinInput->clear();
    ui->MaxInput_Only->clear();
    ui->MinInput_Only->clear();
    ui->Gratuit_checkBox->setChecked(false); // Décocher la case "Gratuit"

    // Réafficher la liste complète
    sublibrary.setHead(mainlibrary.getHead());
    ShowTheLibrary(sublibrary);

    // Cacher le bouton "Clear Filter" après réinitialisation
    ui->ClearFilterButton->setVisible(false);
}

void MainWindow::on_SubListButton_MaxMin_clicked()
{
    if (ui->MaxInput->text().isEmpty() || ui->MinInput->text().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please enter valid values for the cost.");
        return;
    }

    double maxCost = ui->MaxInput->text().toDouble();
    double minCost = ui->MinInput->text().toDouble();

    if (maxCost < minCost)
    {
        QMessageBox::warning(this, "Error", "The maximum cost must be greater than or equal to the minimum cost.");
        return;
    }

    Descriptor *newHead = mainlibrary.getDescriptorsBetweenMaxMinCost(maxCost, minCost);
    sublibrary.setHead(newHead);
    ShowTheLibrary(sublibrary);
    ui->ClearFilterButton->setVisible(true);

   
}

void MainWindow::on_SubListButton_Max_clicked()
{
    if (ui->MaxInput_Only->text().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please enter a value for the maximum cost.");
        return;
    }

    double maxCost = ui->MaxInput_Only->text().toDouble();

    if (maxCost < 0)
    {
        QMessageBox::warning(this, "Error", "The maximum cost must be positive.");
        return;
    }

    Descriptor *newHead = mainlibrary.getDescriptorsBetweenMaxMinCost(maxCost, 0);
    sublibrary.setHead(newHead);
    ShowTheLibrary(sublibrary);
    ui->ClearFilterButton->setVisible(true);

    
}

void MainWindow::on_SubListButton_Min_clicked()
{
    if (ui->MinInput_Only->text().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please enter a value for the minimum cost.");
        return;
    }

    double minCost = ui->MinInput_Only->text().toDouble();

    if (minCost < 0)
    {
        QMessageBox::warning(this, "Error", "The minimum cost must be positive.");
        return;
    }

    Descriptor *newHead = mainlibrary.getDescriptorsBetweenMaxMinCost(INFINITY, minCost);
    sublibrary.setHead(newHead);
    ShowTheLibrary(sublibrary);
    ui->ClearFilterButton->setVisible(true);

    
}

void MainWindow::on_SubListButton_Gratuit_clicked()
{
    bool gratuit = ui->Gratuit_checkBox->isChecked();

    Descriptor *newHead = nullptr;

    if (gratuit)
    {
        // Si la case est cochée, on filtre uniquement les éléments gratuits (cost = 0)
        newHead = mainlibrary.getDescriptorsBetweenMaxMinCost(0, 0);
    }
    else
    {
        // Si la case est décochée, on filtre pour NE PAS afficher les gratuits (cost > 0)
        newHead = mainlibrary.getDescriptorsBetweenMaxMinCost(INFINITY, 0.01);
    }

    sublibrary.setHead(newHead);
    ShowTheLibrary(sublibrary);
    ui->ClearFilterButton->setVisible(true);
}

void MainWindow::on_LogoutButton_clicked()
{
    emit logoutRequested();

}

void MainWindow::SaveChanges_clicked(Descriptor *currentDescriptor, unsigned int originalId)
{
    QString libraryPath = this->currentLibraryPath;

    // Récupérer les nouvelles informations
    QJsonObject curObj = currentDescriptor->toJson();

    // Charger la bibliothèque
    QFile file(libraryPath);
    if (!file.open(QIODevice::ReadOnly)) {
        // qDebug() << "Error: Could not open file";
        return;
    }

    // Lire le fichier JSON existant
    QByteArray data = file.readAll();
    file.close();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj = doc.object();
    QJsonArray array = obj["library"].toArray();
    QJsonArray newArray;

    for (int i = 0; i < array.size(); i++) {
        QJsonObject obj = array[i].toObject();

        // Vérifier si l'ID correspond à l'ID d'origine
        if (obj["id"].toInt() == (int)originalId) {
            newArray.append(curObj); // Remplacer l'entrée
        } else {
            newArray.append(obj); // Garder les autres entrées inchangées
        }
    }

    obj["library"] = newArray;

    // Sauvegarder les modifications
    if (!file.open(QIODevice::WriteOnly)) {
        // qDebug() << "Error: Could not open file";
        return;
    }
    file.write(QJsonDocument(obj).toJson());
    file.close();

    // qDebug() << "Changes saved to the library file for ID:" << originalId;
}
