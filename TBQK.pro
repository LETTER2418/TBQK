QT       += core gui
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MessageBox.cpp \
    UserManager.cpp \
    Widget.cpp \
    WindowManager.cpp \
    about.cpp \
    BUTTON.cpp \
    main.cpp \
    menu.cpp \
    start.cpp

HEADERS += \
    MessageBox.h \
    UserManager.h \
    Widget.h \
    WindowManager.h \
    about.h \
    BUTTON.h \
    menu.h \
    start.h

FORMS += \
    Widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc

