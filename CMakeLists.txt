cmake_minimum_required(VERSION 3.16)

project(Library VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

find_package(CURL REQUIRED)
find_package(GDAL REQUIRED)

# Find OpenCV
find_package(OpenCV REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/Library)

include_directories(${CURL_INCLUDE_DIRS} ${GDAL_INCLUDE_DIR})




set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    image.hpp
    image.cpp
    loginwindow.hpp
    loginwindow.cpp
    loginwindow.ui
    user.hpp
    user.cpp
    librarymanagement.hpp
    librarymanagement.cpp
    descriptor.hpp
    descriptor.cpp
    descriptordetails.hpp
    descriptordetails.cpp
    descriptordetails.ui
    add_new_descriptor.hpp
    add_new_descriptor.cpp
    add_new_descriptor.ui
    imageproccessing.hpp
    imageproccessing.cpp
    resources.qrc
    ClickableLabel.hpp
    kernels.hpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Library
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(Library SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Library
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Library 
    PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets 
    ${OpenCV_LIBS} 
    ${CURL_LIBRARIES} 
    ${GDAL_LIBRARY} 
    -lopenjp2 
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Library)
endif()
set_target_properties(Library PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Library
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Library)
endif()
