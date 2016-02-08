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

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

public slots:

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
    QPoint lastPos;
};

#endif // MYGLWIDGET_H

