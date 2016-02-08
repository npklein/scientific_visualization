TEMPLATE = app
TARGET = smoke

QT = core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    fluids.cpp \
    simulation.cpp \
    visualization.cpp

HEADERS += \

INCLUDEPATH += fftw-2.1.5/include
LIBS += -L"$$_PRO_FILE_PWD_/fftw-2.1.5/lib" -lrfftw -lfftw -framework OpenGL -framework GLUT
