# Mac - Linux : to expand with: qmake -spec macx-g++
# Win32 :
#  set QMAKESPEC=win32-msvc2008
#  qmake -tp vc -config debug


TARGET = qshare
DESTDIR= $$(PWD)
CONFIG += x86
TEMPLATE = app
FORMS = capturewindow.ui
SOURCES = main.cpp \
	capturewindow.cpp \
	FastDXT/libdxt.cpp \
        FastDXT/dxt.cpp \
        FastDXT/util.cpp \
        FastDXT/intrinsic.cpp

HEADERS += capturewindow.h

INCLUDEPATH += FastDXT

QMAKE_CXXFLAGS += -DDXT_INTR

RC_FILE = qsharerc.rc

macx { 
    INCLUDEPATH += ../../include
    LIBS += -m32 -L../../lib -lsail -framework OpenGL -framework Cocoa -framework IOKit -lobjc -lm
}
unix:!macx { 
    INCLUDEPATH += ../../include
    LIBS += -L../../lib -lsail -lquanta
}
win32 {
    QMAKE_CXXFLAGS += -D_CRT_SECURE_NO_WARNINGS
    CONFIG += embed_manifest_exe
    INCLUDEPATH += ../../include
    INCLUDEPATH += ../../win32/include
    LIBS += -L../../lib -L../../win32/lib -lsail -lpthread -lWs2_32 -lquanta
}

RESOURCES += 
target.path=../../bin
INSTALLS += target

message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message(Qt resources can be found in the following locations:)
message(Header files: $$[QT_INSTALL_HEADERS])
message(Libraries: $$[QT_INSTALL_LIBS])

OTHER_FILES +=
