QT       += core gui network
QT += multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about.cpp \
    custommap.cpp \
    datamanager.cpp \
    game.cpp \
    hexcell.cpp \
    lbutton.cpp \
    leveleditor.cpp \
    levelmode.cpp \
    main.cpp \
    mainwindow.cpp \
    mapdata.cpp \
    mapmsgbox.cpp \
    menu.cpp \
    messagebox.cpp \
    onlinechat.cpp \
    onlinemode.cpp \
    onlinemsgbox.cpp \
    randommap.cpp \
    ranking.cpp \
    rankpage.cpp \
    savemapmsgbox.cpp \
    setting.cpp \
    socketmanager.cpp \
    start.cpp \
    userdata.cpp \
    widget.cpp

HEADERS += \
    about.h \
    custommap.h \
    datamanager.h \
    game.h \
    hexcell.h \
    lbutton.h \
    leveleditor.h \
    levelmode.h \
    mainwindow.h \
    mapdata.h \
    mapmsgbox.h \
    menu.h \
    messagebox.h \
    onlinechat.h \
    onlinemode.h \
    onlinemsgbox.h \
    randommap.h \
    ranking.h \
    rankpage.h \
    savemapmsgbox.h \
    setting.h \
    socketmanager.h \
    start.h \
    userdata.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc

DISTFILES += \
    image/太极.png \
    image/太极.png \
    image/太极.svg

