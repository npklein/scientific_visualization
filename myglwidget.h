// myglwidget.h

#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>
#include <QTimer>
#include <rfftw.h>              //the numerical simulation FFTW library

class MyGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = 0);
    ~MyGLWidget();
signals:

public slots:
    void do_one_simulation_step(void);

private slots:
    void showAnimation(bool checked);

    void drawMatter(bool checked);

    void drawHedgehogs(bool checked);

    void directionColoring(bool checked);

    void timestep(int position);

    void hedgehogScaling(int position);

    void fluidViscosity(int position);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);


signals:

private:
    void visualize();
    QTimer timer;
    int   winWidth, winHeight;      //size of the graphics window, in pixels
    int   color_dir;            //use direction color-coding or not
    float vec_scale;			//scaling of hedgehogs
    int   draw_smoke;           //draw the smoke or not
    int   draw_vecs;            //draw the vector field or not
    static const int COLOR_BLACKWHITE=0;   //different types of color mapping: black-and-white, rainbow, banded
    static const int COLOR_RAINBOW=1;
    static const int COLOR_BANDS=2;
    int   scalar_col;           //method for scalar coloring
    int DIM;
    QPoint lastPos;
};

#endif // MYGLWIDGET_H
