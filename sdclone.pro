QT       += core gui charts network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

contains(QMAKESPEC,.*linux-rasp-pi\d*-.*){
    message(rpi detected)
    CONFIG += rpi
    DEFINES += RPI
}

unix:rpi:
{
    LIBS += -L/home/zoli/pi4_bullseye/sysroot/usr/lib/arm-linux-gnueabihf
}

SOURCES += \
    #actions.cpp \
    bi/devicestorage.cpp \
    bi/imagestorage.cpp \
    devicewidget.cpp \
    dowork.cpp \
    helpers/blockdevicehelper.cpp \
    helpers/filehelper.cpp \
    helpers/filenamehelper.cpp \
    helpers/mounthelper.cpp \
    helpers/processhelper.cpp \
    main.cpp \
    mainpresenter.cpp \
    mainwindow.cpp
    #model.cpp \
     #updates.cpp

HEADERS += \
    #actions.h \
    bi/devicestorage.h \
    bi/imagestorage.h \
    devicewidget.h \
    dowork.h \
    global.h \
    helpers/blockdevicehelper.h \
    helpers/filehelper.h \
    helpers/filenamehelper.h \
    helpers/mounthelper.h \
    helpers/processhelper.h \
    imainview.h \
    mainpresenter.h \
    mainviewmodel.h \
    mainwindow.h
    #model.h \
     #updates.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:rpi: target.path = /home/pi/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
