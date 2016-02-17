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
    void showAnimation();

    void drawMatter();

    void drawHedgehogs();

    void directionColoring();

    void timestep(int position);

    void hedgehogScaling(int position);

    void fluidViscosity(int position);

    void scalarColoring(QString scalartype);

    void clampColorMin(int min_color);

    void clampColorMax(int max_color);

    void drawBar();

    void drawVelocity(fftw_real wn, fftw_real hn);

    void drawSmoke(fftw_real wn, fftw_real hn);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);


signals:

private:
    QTimer timer;
    int   winWidth, winHeight;      //size of the graphics window, in pixels
    int   color_dir;            //use direction color-coding or not
    float vec_scale;			//scaling of hedgehogs
    int   draw_smoke;           //draw the smoke or not
    int   draw_vecs;            //draw the vector field or not
    int   scalar_col;           //method for scalar coloring
    int DIM;
    float color_clamp_min;
    float color_clamp_max;
    QPoint lastPos;
};

#endif // MYGLWIDGET_H
