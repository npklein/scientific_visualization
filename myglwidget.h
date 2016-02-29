// myglwidget.h

#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>
#include <QTimer>
#include <rfftw.h>              //the numerical simulation FFTW library
#include "vector.h"
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

    void timestep(int position);

    void hedgehogScaling(int position);

    void fluidViscosity(int position);

    void scalarColoring(QString scalartype);

    void clampColorMin(int min_color);

    void clampColorMax(int max_color);

    //void OGL_Draw_Text(QString text,float x,float y, float z, float red, float green, float blue);
    void OGL_Draw_Text();

    void drawBar();

    void drawVelocity(fftw_real wn, fftw_real hn);

    void drawSmoke(fftw_real wn, fftw_real hn);

    void applyColoringToDataset(QString dataset_to_use);

    void setColorBands(int colorBands);

    void drawHedgehog(float i, float j, float wn, float hn);

    void drawArrow(float j, float i, float wn, float hn);

    void setGlyphType(QString glyps);

    void scaleColor(bool new_scale_color);

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
    int   velocity_color;            //set direction color-coding type
    int force_field_color;          // det force field color-coding type
    float hedgehog_scale;			//scaling of hedgehogs
    int   draw_smoke;           //draw the smoke or not
    int   draw_vecs;            //draw the vector field or not
    int   scalar_col;           //method for scalar coloring
    int DIM;
    int color_bands;
    bool scale_color;
    float color_clamp_min;
    float color_clamp_max;
    float arrow_scale;
    std::string dataset;
    QPoint lastPos;
    QString glyphs;
};

#endif // MYGLWIDGET_H
