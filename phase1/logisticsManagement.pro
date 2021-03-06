QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    basic.cpp \
    delivery.cpp \
    loginpage.cpp \
    main.cpp \
    mainwindow.cpp \
    registerpage.cpp \
    sendpage.cpp \
    slidepage.cpp \
    user.cpp

HEADERS += \
    basic.h \
    delivery.h \
    loginpage.h \
    mainwindow.h \
    registerpage.h \
    sendpage.h \
    slidepage.h \
    user.h

FORMS += \
    loginpage.ui \
    mainwindow.ui \
    registerpage.ui \
    sendpage.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc
