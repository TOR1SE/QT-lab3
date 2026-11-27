cmake_minimum_required(VERSION 3.16)
project(HospitalManagement LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 本工程放在中文目录下，某些 AUTOGEN 临时目录含中文时 moc 会失败，
# 此时把 AUTOGEN 目录挪到纯 ASCII 的系统临时目录。
string(REGEX MATCH "[^A-Za-z0-9:/_. +-\\\\]" NON_ASCII_IN_BINARY "${CMAKE_CURRENT_BINARY_DIR}")
if(NON_ASCII_IN_BINARY)
    set(_autogen "$ENV{TEMP}/hospital_autogen")
    if(_autogen MATCHES "[^A-Za-z0-9:/_. +-\\\\]")
        set(_autogen "C:/Windows/Temp/hospital_autogen")
    endif()
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
    src/main.cpp
    src/dbmanager.h
    src/dbmanager.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/loginwidget.h
    src/loginwidget.cpp
    src/welcomewidget.h
    src/welcomewidget.cpp
    src/patientwidget.h
    src/patientwidget.cpp
    src/patientdialog.h
    src/patientdialog.cpp
    src/genderdelegate.h
    src/genderdelegate.cpp
    src/historywidget.h
    src/historywidget.cpp
    src/simpletablewidget.h
    src/simpletablewidget.cpp
    src/demorunner.h
    src/demorunner.cpp
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(HospitalManagement ${PROJECT_SOURCES})
else()
    add_executable(HospitalManagement ${PROJECT_SOURCES})
endif()

target_link_libraries(HospitalManagement PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)

if(NON_ASCII_IN_BINARY)
    set_target_properties(HospitalManagement PROPERTIES AUTOGEN_BUILD_DIR "${_autogen}")
endif()
