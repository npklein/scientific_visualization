// myglwidget.h

#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>
#include <QTimer>
#include <rfftw.h>              //the numerical simulation FFTW library
#include "vector.h"
#include "simulation.h"
#include <vector>

class MyGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = 0);
    ~MyGLWidget();
signals:

public slots:
    void do_one_simulation_step(bool update);
    void do_one_simulation_step();

private slots:
    void showAnimation(bool new_frozen);

    void drawMatter(bool);

    void drawHedgehogs(bool);

    void timestep(int position);

    void hedgehogScaling(int position);

    void fluidViscosity(int position);

    void scalarColoring(QString scalartype);

    void clampColorMin(int min_color);

    void clampColorMax(int max_color);

    //void OGL_Draw_Text(QString text,float x,float y, float z, float red, float green, float blue);
    void OGL_Draw_Text();

    void drawBar();

    void drawVelocity(fftw_real *vx, fftw_real *vy);

    void drawForcefield(fftw_real *fx, fftw_real *fy);

    void drawSmoke();

    void setFluidDensity();

    void setFluidVelocity();

    void setForceField();

    void setColorBands(int colorBands);

    void drawArrow(Vector vector, int i, int j, float vy, int scaling_factor, float vy_min, float vy_max);

    void drawCone(Vector vector, int i, int j, float vy, int scaling_factor, float vy_min, float vy_max);

    void drawStreamline();

    void setDrawStreamline(bool new_streamline);

    void setGlyphType(QString glyps);

    void scaleColors(bool new_scale_color);

    void drawGridLines(int DIM);

    void setDim(int new_DIM);

    void drawGrid(bool new_draw_grid);

    void setGridSize(int position);

    void drawGradient();

    void setDrawGradient(bool new_gradient);

    void drawSlices(int n);

    void setNumberOfGlyphs(int position);

    void setDrawSlices(bool new_slices);

    void setHue(int new_hue);

    void setSaturation(int new_saturation);

    void selectPoints(bool select_points);

    void defaultPoints(std::vector<int> &points_x, std::vector<int> &points_y);
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);


signals:

private:
    QTimer timer;
    int   windowWidth, windowHeight;      //size of the graphics window, in pixels
    int   velocity_color;            //set direction color-coding type
    int force_field_color;          // det force field color-coding type
    float hedgehog_scale;			//scaling of hedgehogs
    bool   draw_smoke;           //draw the smoke or not
    bool   draw_vecs;            //draw the vector field or not
    int   scalar_col;           //method for scalar coloring
    int DIM;
    int number_of_glyphs;
    int grid_scale;
    int color_bands;
    bool scale_color;
    bool draw_grid;
    float color_clamp_min_matter;
    float color_clamp_max_matter;
    float color_clamp_min_glyph;
    float color_clamp_max_glyph;
    float arrow_scale;
    float cone_scale;
    bool gradient;
    bool draw_streamline;
    bool draw_slices;
    int hue_glyph;
    int hue_matter;
    float saturation_matter;
    float saturation_glyph;
    bool draw_v;
    bool draw_f;
    bool select_points;
    Simulation simulation;
    fftw_real  cell_width;
    fftw_real  cell_height;
    QWidget *window;
    std::string dataset;
    QPoint lastPos;
    QString glyphs;
    std::vector<int> mouse_x;
    std::vector<int> mouse_y;
    std::vector<int> points_x;
    std::vector<int> points_y;
};

#endif // MYGLWIDGET_H
