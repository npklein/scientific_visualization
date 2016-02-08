// myglwidget.cpp

#include <QtWidgets>
#include <QtOpenGL>
#include <GLUT/glut.h>          //the GLUT graphics library
#include "myglwidget.h"
#include "visualization.cpp"
#include <simulation.cpp>              //the numerical simulation FFTW library



MyGLWidget::MyGLWidget(QWidget *parent)
{
    //--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
    color_dir = 0;            //use direction color-coding or not
    vec_scale = 1000;			//scaling of hedgehogs
    draw_smoke = 0;           //draw the smoke or not
    draw_vecs = 1;            //draw the vector field or not
    scalar_col = 0;           //method for scalar coloring
    DIM = 100;
    simulation.init_simulation(DIM);
    QTimer *timer = new QTimer;
    timer->start(1);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(do_one_simulation_step()));

}

MyGLWidget::~MyGLWidget()
{
}

void MyGLWidget::initializeGL()
{
    qglClearColor(Qt::black);
}

void MyGLWidget::paintGL() //glutDisplayFunc(display);
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_TABLE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    visualize();
    glFlush();
    swapBuffers();
}


void MyGLWidget::resizeGL(int width, int height)
{
    glViewport(0.0f, 0.0f, (GLfloat)width, (GLfloat)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)width, 0.0, (GLdouble)height);
    winWidth = width; winHeight = height;
}


void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int mx = event->x() - lastPos.x();
    int my = event->y() - lastPos.y();
    simulation.drag(mx,my, DIM, winWidth, winHeight);
    lastPos = event->pos();
}

void MyGLWidget::visualize()
{
    int        i, j, idx, idx0, idx1, idx2, idx3; double px0,py0,px1,py1,px2,py2,px3,py3;
    fftw_real  wn = (fftw_real)winWidth / (fftw_real)(DIM + 1);   // Grid cell width
    fftw_real  hn = (fftw_real)winHeight / (fftw_real)(DIM + 1);  // Grid cell heigh

    if (draw_smoke)
    {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);
    for (j = 0; j < DIM - 1; j++)			//draw smoke
    {
        for (i = 0; i < DIM - 1; i++)
        {
            px0  = wn + (fftw_real)i * wn;
            py0  = hn + (fftw_real)j * hn;
            idx0 = (j * DIM) + i;

            px1  = wn + (fftw_real)i * wn;
            py1  = hn + (fftw_real)(j + 1) * hn;
            idx1 = ((j + 1) * DIM) + i;

            px2  = wn + (fftw_real)(i + 1) * wn;
            py2  = hn + (fftw_real)(j + 1) * hn;
            idx2 = ((j + 1) * DIM) + (i + 1);

            px3  = wn + (fftw_real)(i + 1) * wn;
            py3  = hn + (fftw_real)j * hn;
            idx3 = (j * DIM) + (i + 1);

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
      for (i = 0; i < DIM; i++)
        for (j = 0; j < DIM; j++)
        {
          idx = (j * DIM) + i;
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
        simulation.set_forces(DIM);
        simulation.solve(DIM, simulation.get_vx(), simulation.get_vy(), simulation.get_vx0(),
                         simulation.get_vy0(), simulation.get_visc(), simulation.get_dt());
        // Note to self: * in *simulation.get_vx() because simulation.get_vx() returns 'double *' and diffuse_matter needs 'double'
        simulation.diffuse_matter(DIM, simulation.get_vx(), simulation.get_vy(),
                                 simulation.get_rho(), simulation.get_rho0(), simulation.get_dt());
        updateGL();
    }
}
