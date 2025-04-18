QT       += core gui
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    About.cpp \
    Lbutton.cpp \
    LevelEditor.cpp \
    Main.cpp \
    Menu.cpp \
    MessageBox.cpp \
    RandomMap.cpp \
    RandomMapMsgBox.cpp \
    Start.cpp \
    UserManager.cpp \
    Widget.cpp \
    WindowManager.cpp \
    colorpicker.cpp

HEADERS += \
    About.h \
    Lbutton.h \
    LevelEditor.h \
    Menu.h \
    MessageBox.h \
    RandomMap.h \
    RandomMapMsgBox.h \
    Start.h \
    UserManager.h \
    Widget.h \
    WindowManager.h \
    colorpicker.h

FORMS += \
    Widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc

