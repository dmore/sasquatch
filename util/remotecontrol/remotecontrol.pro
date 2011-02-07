TEMPLATE = app
DESTDIR = ../../hub/
DEPENDPATH += .
INCLUDEPATH += . ../../src/3rdparty/ ../../src ../../src/qml-extensions

QT += network declarative
include(../../src/rpc/rpc.pri)

!symbian {
    # avahi integration
    INCLUDEPATH +=  ../../src/3rdparty/libqavahi/
    include(../../src/3rdparty/libqavahi/libqavahi.pri)
    SOURCES += avahiservicebrowserview.cpp
    HEADERS += avahiservicebrowserview.h
} else {
    LIBS += -llibc
    TARGET.CAPABILITY += NetworkServices
    CONFIG += no-avahi
}

no-avahi {
    DEFINES += QMH_NO_AVAHI
    SOURCES += staticservicebrowserview.cpp
    HEADERS += staticservicebrowserview.h
    RESOURCES += remotecontrol.qrc
    FORMS += addservicedialog.ui
}

TEMP_DIR = $$PWD
OBJECTS_DIR = $$TEMP_DIR/.obj
MOC_DIR = $$TEMP_DIR/.moc
UI_DIR = $$TEMP_DIR/.ui

FORMS += remotecontrol.ui

# Input
SOURCES += main.cpp remotecontrol.cpp \
    mainwindow.cpp
HEADERS += remotecontrol.h \
    mainwindow.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target
}
