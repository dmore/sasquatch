include(common.pri)

DESTDIR = ../hub/lib
TEMPLATE = lib
TARGET = qmhcore
DEPENDPATH += .
INCLUDEPATH += . ./plugins/qmhplugin

QMAKE_CFLAGS_RELEASE += -fvisibility=hidden -fvisibility-inlines-hidden
QMAKE_CFLAGS_DEBUG += -fvisibility=hidden -fvisibility-inlines-hidden
QMAKE_CXXFLAGS += -fvisibility=hidden  -fvisibility-inlines-hidden

#global options
CONFIG += glviewport
#end global options

scenegraph {
    DEFINES += SCENEGRAPH
}

mac {
    QT += webkit\
          multimedia
          
    CONFIG += mobility

    MOBILITY += multimedia\
                systeminfo
}

# This is needed for Maemo5 to recognize minimization of the application window
# Thanks quit coding!
maemo5 {
    QT += dbus
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target
}
#end platform options

#painting options
glgs {
    CONFIG += gl
    DEFINES += GLGS
}

glviewport {
    CONFIG += gl
    DEFINES += GLVIEWPORT
}

gl {
    DEFINES += GL
    QT += opengl
}
#end painting options

# Input
SOURCES += qmh-config.cpp \
    frontend.cpp \
    backend.cpp \
    actionmapper.cpp \
    trackpad.cpp \
    scopedtransaction.cpp \
    skin.cpp \
    device.cpp \
    dbreader.cpp \
    devicemanager.cpp \
    powermanager.cpp

QT += declarative script network sql

HEADERS += qmh-config.h \
    global.h \
    frontend.h \
    backend.h \
    qmhplugin.h \
    dirmodel.h \
    file.h \
    actionmapper.h \
    trackpad.h \
    scopedtransaction.h \
    skin.h \
    device.h \
    dbreader.h \
    devicemanager.h \
    powermanager.h

include(rpc/rpc.pri)
include(media/media.pri)

#for() structure does not work with lupdate
TRANSLATIONS = $$system(cat $$DESTDIR/supported_languages | while read i; do echo translations/"$i".ts; done)

avahi {
    DEFINES += QMH_AVAHI
    # avahi support
    include(3rdparty/libqavahi/libqavahi.pri)
    INCLUDEPATH += 3rdparty/libqavahi/
} else {
    SOURCES += staticservicebrowsermodel.cpp
    HEADERS += staticservicebrowsermodel.h
}

contains(QT_CONFIG, dbus) {
    QT += dbus
    SOURCES += devicemanagerdbus.cpp
    HEADERS += devicemanagerdbus.h
}
