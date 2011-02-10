DEFINES += BUILDING_QMH

TEMP_DIR = tmp
OBJECTS_DIR = $$TEMP_DIR/.obj
MOC_DIR = $$TEMP_DIR/.moc

linux* {
    message(Linux specific options: (*default unless adjusted in common.pri))
    message(gstreamer*)
    message(release*)
    CONFIG += gstreamer
}

CONFIG += release
