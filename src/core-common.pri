QMAKE_LIBDIR += $$PROJECTROOT/hub/lib
LIBS += -lqmhcore
QMAKE_LFLAGS += -Wl,-rpath,$${PROJECTROOT}/hub/lib