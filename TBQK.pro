QT       += core gui
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about.cpp \
    custommap.cpp \
    game.cpp \
    hexcell.cpp \
    lbutton.cpp \
    leveleditor.cpp \
    levelmode.cpp \
    main.cpp \
    mainwindow.cpp \
    mapdata.cpp \
    mapmanager.cpp \
    mapmsgbox.cpp \
    menu.cpp \
    messagebox.cpp \
    randommap.cpp \
    savemapmsgbox.cpp \
    start.cpp \
    usermanager.cpp \
    widget.cpp

HEADERS += \
    about.h \
    custommap.h \
    game.h \
    hexcell.h \
    lbutton.h \
    leveleditor.h \
    levelmode.h \
    mainwindow.h \
    mapdata.h \
    mapmanager.h \
    mapmsgbox.h \
    menu.h \
    messagebox.h \
    randommap.h \
    savemapmsgbox.h \
    start.h \
    usermanager.h \
    widget.h

FORMS += \
    levelmode.ui \
    mapmsgbox.ui \
    savemapmsgbox.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc

