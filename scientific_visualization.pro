TEMPLATE = app
TARGET = smoke

QT = core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    fluids.cpp \
    simulation.cpp \
    visualization.cpp \
    window.cpp \
    myglwidget.cpp \
    vector.cpp \
    grid.cpp

HEADERS += \
    window.h \
    myglwidget.h \
    simulation.h \
    vector.h \
    grid.h

INCLUDEPATH += fftw-2.1.5/include /usr/local/include
LIBS += -L"$$_PRO_FILE_PWD_/fftw-2.1.5/lib" -lrfftw -lfftw -framework OpenGL -framework GLUT -stdlib=libc++

QMAKE_CXXFLAGS += -stdlib=libc++
QMAKE_CXXFLAGS += -std=c++11


FORMS += \
    window.ui
