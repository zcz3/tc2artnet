
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

# Compile libltc on Windows using WSL:
#   $ export CC=x86_64-w64-mingw32-gcc
#   $ ./configure --prefix=/opt/libltc-win32/ --host=x86_64-w64-mingw32
#   $ make
#   $ sudo make install

LIBS += -L$$PWD/../libltc-win32/lib -lltc
INCLUDEPATH += $$PWD/../libltc-win32/include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
