TEMPLATE = app
TARGET = name_of_the_app

QT = core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    window.cpp \
    fluids.cpp \
    simulation.cpp \
    visualization.cpp

HEADERS += \
    window.h \
    simulation.h \
    visualization.h

INCLUDEPATH += fftw-2.1.5/include
LIBS += -L"$$_PRO_FILE_PWD_/fftw-2.1.5/lib" -lrfftw -lfftw
