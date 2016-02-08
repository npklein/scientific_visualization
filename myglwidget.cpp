// myglwidget.cpp

#include <QtWidgets>
#include <QtOpenGL>
#include <GLUT/glut.h>          //the GLUT graphics library
#include "myglwidget.h"
#include "visualization.cpp"
#include <simulation.cpp>              //the numerical simulation FFTW library



MyGLWidget::MyGLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    //--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
    int   winWidth, winHeight;      //size of the graphics window, in pixels
    int   color_dir = 0;            //use direction color-coding or not
    float vec_scale = 1000;			//scaling of hedgehogs
    int   draw_smoke = 0;           //draw the smoke or not
    int   draw_vecs = 1;            //draw the vector field or not
    int   scalar_col = 0;           //method for scalar coloring
    simulation.init_simulation(simulation.DIM);
    QTimer *timer = new QTimer;
    timer->start(1000);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(do_one_simulation_step()));

}

MyGLWidget::~MyGLWidget()
{
}

QSize MyGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize MyGLWidget::sizeHint() const
{
    return QSize(400, 400);
}

void MyGLWidget::initializeGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

//display: Handle window redrawing events. Simply delegates to draw().
void MyGLWidget::resizeGL(int width, int height)
{
    glViewport(0.0f, 0.0f, (GLfloat)width, (GLfloat)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)width, 0.0, (GLdouble)height);
    winWidth = width; winHeight = height;
}

void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    visualize();
    glFlush();
    swapBuffers();
}


void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int mx = event->x() - lastPos.x();
    int my = event->y() - lastPos.y();

    int xi,yi,X,Y; double  dx, dy, len;
    static int lmx=0,lmy=0;				//remembers last mouse location

    // Compute the array index that corresponds to the cursor location
    xi = (int)simulation.clamp((double)(simulation.DIM + 1) * ((double)mx / (double)winWidth));
    yi = (int)simulation.clamp((double)(simulation.DIM + 1) * ((double)(winHeight - my) / (double)winHeight));

    X = xi; Y = yi;

    if (X > (simulation.DIM - 1))  X = simulation.DIM - 1; if (Y > (simulation.DIM - 1))  Y = simulation.DIM - 1;
    if (X < 0) X = 0; if (Y < 0) Y = 0;

    // Add force at the cursor location
    my = winHeight - my;
    dx = mx - lmx; dy = my - lmy;
    len = sqrt(dx * dx + dy * dy);
    if (len != 0.0) {  dx *= 0.1 / len; dy *= 0.1 / len; }
    simulation.get_fx()[Y * simulation.DIM + X] += dx;
    simulation.get_fy()[Y * simulation.DIM + X] += dy;
    simulation.get_rho()[Y * simulation.DIM + X] = 10.0f;
    lmx = mx; lmy = my;
    lastPos = event->pos();
}

void MyGLWidget::visualize()
{
    int        i, j, idx, idx0, idx1, idx2, idx3; double px0,py0,px1,py1,px2,py2,px3,py3;
    fftw_real  wn = (fftw_real)winWidth / (fftw_real)(simulation.DIM + 1);   // Grid cell width
    fftw_real  hn = (fftw_real)winHeight / (fftw_real)(simulation.DIM + 1);  // Grid cell heigh

    if (draw_smoke)
    {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);
    for (j = 0; j < simulation.DIM - 1; j++)			//draw smoke
    {
        for (i = 0; i < simulation.DIM - 1; i++)
        {
            px0  = wn + (fftw_real)i * wn;
            py0  = hn + (fftw_real)j * hn;
            idx0 = (j * simulation.DIM) + i;

            px1  = wn + (fftw_real)i * wn;
            py1  = hn + (fftw_real)(j + 1) * hn;
            idx1 = ((j + 1) * simulation.DIM) + i;

            px2  = wn + (fftw_real)(i + 1) * wn;
            py2  = hn + (fftw_real)(j + 1) * hn;
            idx2 = ((j + 1) * simulation.DIM) + (i + 1);

            px3  = wn + (fftw_real)(i + 1) * wn;
            py3  = hn + (fftw_real)j * hn;
            idx3 = (j * simulation.DIM) + (i + 1);

            set_colormap(simulation.get_rho()[idx0]);	glVertex2f(px0, py0);
            set_colormap(simulation.get_rho()[idx1]);	glVertex2f(px1, py1);
            set_colormap(simulation.get_rho()[idx2]);	glVertex2f(px2, py2);

            set_colormap(simulation.get_rho()[idx0]);	glVertex2f(px0, py0);
            set_colormap(simulation.get_rho()[idx2]);	glVertex2f(px2, py2);
            set_colormap(simulation.get_rho()[idx3]);	glVertex2f(px3, py3);
        }
    }
    glEnd();
    }

    if (draw_vecs)
    {
      glBegin(GL_LINES);				//draw velocities
      for (i = 0; i < simulation.DIM; i++)
        for (j = 0; j < simulation.DIM; j++)
        {
          idx = (j * simulation.DIM) + i;
          direction_to_color(simulation.get_vx()[idx],simulation.get_vy()[idx],color_dir);
          glVertex2f(wn + (fftw_real)i * wn, hn + (fftw_real)j * hn);
          glVertex2f((wn + (fftw_real)i * wn) + vec_scale * simulation.get_vx()[idx], (hn + (fftw_real)j * hn) + vec_scale * simulation.get_vy()[idx]);
        }
      glEnd();
    }

}

void MyGLWidget::do_one_simulation_step()
{
    if (!simulation.get_frozen())
    {
        simulation.set_forces();
        simulation.solve(simulation.DIM, simulation.get_vx(), simulation.get_vy(), simulation.get_vx0(),
                         simulation.get_vy0(), simulation.get_visc(), simulation.get_dt());
        // Note to self: * in *simulation.get_vx() because simulation.get_vx() returns 'double *' and diffuse_matter needs 'double'
        simulation.diffuse_matter(simulation.DIM, simulation.get_vx(), simulation.get_vy(),
                                 simulation.get_rho(), simulation.get_rho0(), simulation.get_dt());
        updateGL();
    }
}
