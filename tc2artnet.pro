
QT += core gui widgets network multimedia

CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

VPATH += $$_PRO_FILE_PWD_/src
DEPENDPATH += $$_PRO_FILE_PWD_/src
INCLUDEPATH += $$_PRO_FILE_PWD_/src

SOURCES += \
  main.cpp \
  tc2artnet.cpp \
  mainwindow.cpp \
  ltcreceiver.cpp \
  ltcdecoder.cpp \
  transmitter.cpp

HEADERS += \
  tc2artnet.h \
  mainwindow.h \
  ltcreceiver.h \
  ltcdecoder.h \
  transmitter.h \
  timecode_frame.h

win32 {
    # Keep windows headers happy
    DEFINES += UNICODE=1 _UNICODE=1
}

win32 {
  DEFINES += ENABLE_MTC=1

  SOURCES += \
    mtcreceiver.cpp \
    mididev_win32.cpp

  HEADERS += \
    mtcreceiver.h \
    mididev.h

  LIBS += -lWinmm
}

# Compile libltc on Windows using WSL:
#   $ export CC=x86_64-w64-mingw32-gcc
#   $ ./configure --prefix=/opt/libltc-win32/ --host=x86_64-w64-mingw32
#   $ make
#   $ sudo make install

LIBS += -L$$PWD/libltc_build/lib -L$$PWD/../libltc_build/lib -lltc
INCLUDEPATH += $$PWD/libltc_build/include $$PWD/../libltc_build/include

win32:release {
  QMAKE_LFLAGS_RELEASE+=-static
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
