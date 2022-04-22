QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adminpage.cpp \
    basic.cpp \
    delivery.cpp \
    loginpage.cpp \
    main.cpp \
    registerpage.cpp \
    sendpage.cpp \
    slidepage.cpp \
    user.cpp \
    userpage.cpp

HEADERS += \
    adminpage.h \
    basic.h \
    delivery.h \
    loginpage.h \
    registerpage.h \
    sendpage.h \
    slidepage.h \
    user.h \
    userpage.h

FORMS += \
    adminpage.ui \
    loginpage.ui \
    registerpage.ui \
    sendpage.ui \
    userpage.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc
