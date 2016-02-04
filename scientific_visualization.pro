TEMPLATE = app
TARGET = name_of_the_app

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    main.cpp \
    window.cpp \
    fluids.cpp \
    simulation.cpp \
    visualization.cpp

HEADERS += \
    window.h

INCLUDEPATH += fftw-2.1.5/include

LIBS += -Lfftw-2.1.5/lib -lrfftw -lfftw


