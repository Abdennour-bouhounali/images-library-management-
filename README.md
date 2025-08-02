# 📷 Image Library Management System

A C++ and Qt-based desktop application for organizing, filtering, and managing collections of images within customizable libraries.

This project was developed as part of a university team effort, where we earned **second place** for our work. The experience helped us improve our skills in UI design, file handling, image processing, and collaborative development using Qt and modern C++.

---

## 🛠️ Technologies Used

- **C++** – Core programming language
- **Qt (Widgets)** – GUI framework
- **OpenCV** *(optional)* – For advanced image processing (SIFT, filters)
- **Qt Signals/Slots** – Event-driven architecture

---

## 🎯 Features

### 🔐 Authentication
- Users enter the application using a **code-based access** system

### 🗂️ Library Management
- Create new libraries
- Delete existing libraries
- Each library can contain a collection of images with metadata

### 🖼️ Image Management
- View all images of a selected library in the **main window**
- Display image descriptors (e.g., name, ID, price, etc.)
- Add new images with information
- Delete or **edit image metadata**

### 🔍 Search & Sorting
- Search images by **name** or **ID**
- Order images by **price**

### 🧪 Image Processing
- Select an image to open a dedicated **processing window**
- Apply filters and algorithms:
  - Grayscale
  - Gaussian blur
  - SIFT (Scale-Invariant Feature Transform)
  - Other custom filters
- Save the filtered image

---

## 🧭 Project Structure


```
ImageLibraryManager/
├── src/
│   ├── mainwindow.ui
│   ├── mainwindow.cpp
│   ├── imagewindow.cpp
│   ├── librarymanager.cpp
│   ├── imageprocessor.cpp
├── assets/
│   └── sample images
├── include/
│   └── header files
├── CMakeLists.txt or *.pro
└── README.md
```


---

## 📸 Screenshots



---

## 🧠 What We Learned

- Structuring a scalable Qt application with multiple views/windows
- Building real-time UI interactions with Qt’s signal/slot system
- Integrating image processing algorithms
- Working collaboratively using Git and clean code practices
- Managing user interaction, data storage, and persistent state

---

## 🏆 Recognition

> 🎉 **Awarded 2nd place** among all university software projects in our cohort!

---

## 🚀 Getting Started

1. Clone the repo:
   ```bash
   git clone https://github.com/yourusername/image-library-manager.git
Open the project in Qt Creator or build manually:

qmake
make
./ImageLibraryManager
Start managing your image libraries!

🤝 Contributors

Abdennour Bouhounali
📧 abdennour.bouhounali@univ-tlse3.fr

Youcef Brahimi
📧 youcef.brahimi@univ-tlse3.fr

Mohammed Bouziane Belbekri
📧 mohammed-bouziane.belbekri@univ-tlse3.fr

Roua Cherni
📧 roua.cherni@univ-tlse3.fr

Hadj Habib Rouabah
📧 hadj-habib.rouabah@univ-tlse3.fr


📄 License
This project is for educational and personal use. Feel free to modify and extend it.

📬 Contact
For questions or suggestions, feel free to open an issue or contact us via GitHub.
