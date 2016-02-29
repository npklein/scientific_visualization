// myglwidget.cpp

#include <QtWidgets>
#include <QtOpenGL>
#include <GLUT/glut.h>          //the GLUT graphics library
#include "myglwidget.h"
#include "visualization.cpp"
#include <simulation.cpp>              //the numerical simulation FFTW library
#include <cmath>
#include "vector.cpp"

MyGLWidget::MyGLWidget(QWidget *parent)
{
    //--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
    vec_scale = 2000;			//scaling of hedgehogs
    draw_smoke = 1;           //draw the smoke or not
    draw_vecs = 1;            //draw the vector field or not
    scalar_col = 0;           //method for scalar coloring
    scale_color = false;    // if true, the lowest current value in the screen is the lowest in the color map, same for highest
    DIM = 50;
    color_clamp_min = 0.0;        // The lower bound value to clamp color map at
    color_clamp_max = 1.0;        // The higher bound value to clamp color map at
    velocity_color = 1;
    force_field_color = 1;
    color_bands = 256;
    glyphs = "hedgehogs";
    dataset = "fluid density";
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
    glEnable(GL_COLOR_TABLE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    drawBar();
    fftw_real  wn = (fftw_real)winWidth / (fftw_real)(DIM + 1);   // Grid cell width
    fftw_real  hn = (fftw_real)winHeight / (fftw_real)(DIM + 1);  // Grid cell heigh
    if (draw_vecs)
    {
        drawVelocity(wn, hn);
    }
    if (draw_smoke)
    {
        drawSmoke(wn, hn);
    }
    OGL_Draw_Text();
    glFlush();
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
    int mx = event->x();// - lastposition gets calculated in drag(), could save a step by using lastPos.x/y but leaving it like this is safer
    int my = event->y();
    //simulation.drag(mx,my, DIM, winWidth, winHeight);  // Works for Freerk when using external display
    simulation.drag(mx,my, DIM, winWidth/2, winHeight/2); // Works for Niek
    lastPos = event->pos();
}

void MyGLWidget::drawVelocity(fftw_real wn, fftw_real hn)
{
    int  i, j;

    for (i = 0; i < DIM; i++)
        for (j = 0; j < DIM; j++)
        {
            if (glyphs == "hedgehogs"){
                drawHedgehog(i, j, wn, hn);
            }
            if (glyphs == "arrows"){
                //if (i % 5 == 0 && j % 5 == 0){
                    drawArrow(i, j, wn, hn);
                //}
            }
        }
}

void MyGLWidget::drawArrow(float i, float j, float wn, float hn){
    int idx = (j * DIM) + i;
    direction_to_color(simulation.get_vx()[idx],simulation.get_vy()[idx], velocity_color, color_bands);
    Vector vector = Vector(wn + (fftw_real)i * wn, //x1
                                  hn + (fftw_real)j * hn, //y1
                                  (wn + (fftw_real)i * wn) + vec_scale * simulation.get_vx()[idx], //x2
                                  (hn + (fftw_real)j * hn) + vec_scale * simulation.get_vy()[idx]);//y2
    float angle = vector.direction2angle();
    // have to rotate before begin triangles
    //glRotated(angle,0,0,1);
    //glTranslatef(wn + (fftw_real)i * wn,(hn + (fftw_real)j * hn) + vec_scale * simulation.get_vy()[idx],0);
    glPushMatrix();

    glTranslatef(wn*i,hn*j, 0);
    glRotated(angle,0,0,1);
    //glScaled(vector.length()/20,vector.length()/20,0);
    glScaled(log(vector.length()+1)/15,log(vector.length()+1)/5,0);
    glBegin(GL_TRIANGLES);
    glVertex2f(-100, 50);
    glVertex2f(100, 50);
    glVertex2f(0, 100);

    glVertex2f(-50, 0);
    glVertex2f(-50, 50);
    glVertex2f(50, 50);


    glVertex2f(-50, 0);
    glVertex2f(50, 0);
    glVertex2f(50, 50);
    glEnd();
    glPopMatrix(); // now it's at normal scale again
    glLoadIdentity(); // needed to stop the rotating, otherwise rotates the entire drawing
}

void MyGLWidget::drawHedgehog(float i, float j, float wn, float hn){
    glBegin(GL_LINES);				//draw velocities
    int idx = (j * DIM) + i;
    direction_to_color(simulation.get_vx()[idx],simulation.get_vy()[idx], velocity_color, color_bands);
    glVertex2f(wn + (fftw_real)i * wn, hn + (fftw_real)j * hn);
    glVertex2f((wn + (fftw_real)i * wn) + vec_scale * simulation.get_vx()[idx], (hn + (fftw_real)j * hn) + vec_scale * simulation.get_vy()[idx]);
    glEnd();
}



void MyGLWidget::drawSmoke(fftw_real wn, fftw_real hn){
    int  i, j, idx0, idx1, idx2, idx3; double px0,py0,px1,py1,px2,py2,px3,py3;
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

            set_colormap(simulation.get_rho()[idx0], scalar_col, color_clamp_min, color_clamp_max, color_bands, scale_color);
            glVertex2f(px0, py0);
            set_colormap(simulation.get_rho()[idx1], scalar_col, color_clamp_min, color_clamp_max, color_bands, scale_color);
            glVertex2f(px1, py1);
            set_colormap(simulation.get_rho()[idx2], scalar_col, color_clamp_min, color_clamp_max, color_bands, scale_color);
            glVertex2f(px2, py2);

            set_colormap(simulation.get_rho()[idx0], scalar_col, color_clamp_min, color_clamp_max, color_bands, scale_color);
            glVertex2f(px0, py0);
            set_colormap(simulation.get_rho()[idx2], scalar_col, color_clamp_min, color_clamp_max, color_bands, scale_color);
            glVertex2f(px2, py2);
            set_colormap(simulation.get_rho()[idx3], scalar_col, color_clamp_min, color_clamp_max, color_bands, scale_color);
            glVertex2f(px3, py3);
        }
    }
    glEnd();
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


void MyGLWidget::showAnimation()
{
    simulation.set_frozen(1-simulation.get_frozen());
}

void MyGLWidget::drawMatter()
{
    draw_smoke = 1 - draw_smoke;
    if (draw_smoke==0) draw_vecs = 1;
}

void MyGLWidget::drawHedgehogs()
{
    draw_vecs = 1 - draw_vecs;
    if (draw_vecs==0) draw_smoke = 1;
}

void MyGLWidget::scaleColor(bool new_scale_color)
{
    scale_color = new_scale_color;
}

void MyGLWidget::timestep(int position)
{
    // dt start = 0.4
    //      case 't': simulation.set_dt(simulation.get_dt() - 0.001); break;
    //      case 'T': simulation.set_dt(simulation.get_dt() + 0.001); break;
    static int last_pos_timestep = 500;				//remembers last slider location, statics only get initialized once, after that they keep the new value
    double new_pos = position - last_pos_timestep;
    double old_dt = simulation.get_dt();
    double new_dt = old_dt + new_pos * 0.001; //easier to debug on separate line
    if (new_dt < 0){
        new_dt = 0;
    }
    simulation.set_dt(new_dt);
    last_pos_timestep = position;
}

void MyGLWidget::hedgehogScaling(int position)
{
    // vec_scale = 1000;
    //  	  case 'S': vec_scale *= 1.2; break;
    //        case 's': vec_scale *= 0.8; break;
    // The scaling goes exponential with keyboard, but with slide can just do linear
    static int last_pos_hedgehog = 500;				//remembers last slider location
    int new_pos = position - last_pos_hedgehog;
    vec_scale = vec_scale + new_pos * 200; //easier to debug on separate line
    if (vec_scale < 0){
        vec_scale = 0;
    }
    last_pos_hedgehog = position;
}

void MyGLWidget::fluidViscosity(int position)
{
    // visc = 0.001
    //      case 'V': simulation.set_visc(simulation.get_visc()*5); break;
    //      case 'v': simulation.set_visc(simulation.get_visc()*0.2); break;
    // The scaling goes exponential with keyboard, but with slide can just do linear
    static int last_pos_visc = 500;
    int new_pos = position - last_pos_visc;
    double old_visc = simulation.get_visc();
    double new_visc = old_visc + new_pos * 0.005; //easier to debug on separate line
    if (new_visc < 0){
        new_visc = 0;
    }
    simulation.set_visc(new_visc);
    last_pos_visc = position;
}

void MyGLWidget::clampColorMin(int min_color)
{
    if (min_color > 0){
        color_clamp_min = min_color/100.0;
    }
}
void MyGLWidget::clampColorMax(int max_color)
{
    if (max_color > 0){
        color_clamp_max = 1-(max_color/100.0);
    }
}

void MyGLWidget::scalarColoring(QString scalartype){
    if (scalartype == "rainbow") {
        if (dataset == "fluid density"){
            scalar_col = 1;
        }
        else if (dataset == "fluid velocity magnitude"){
            velocity_color = 1;
        }
        else if (dataset == "force field magnitude"){
            force_field_color = 1;
        }
    }
    else if (scalartype == "black&white") {
        if (dataset == "fluid density"){
            scalar_col = 0;
        }
        else if (dataset == "fluid velocity magnitude"){
            velocity_color = 0;
        }
        else if (dataset == "force field magnitude"){
            force_field_color = 0;
        }
    }
    else if (scalartype == "heatmap") {
        if (dataset == "fluid density"){
            scalar_col = 2;
        }
        else if (dataset == "fluid velocity magnitude"){
            velocity_color = 2;
        }
        else if (dataset == "force field magnitude"){
            force_field_color = 2;
        }
    }
}


void MyGLWidget::applyColoringToDataset(QString dataset_to_use){
    dataset = dataset_to_use.toStdString();
}

// Color map explained
// http://www.glprogramming.com/red/chapter04.html just above table 4.2
// The first float is the offset color to start the map of R,G,B from

void MyGLWidget::drawBar(){
    glPushMatrix ();
    glBegin (GL_QUADS);
    if (draw_smoke == 1){
        for (int i = 0; i < 1001; i = i + 1){
            set_colormap(0.001*i,scalar_col, color_clamp_min, color_clamp_max, color_bands, scale_color);
            glVertex3f(15+(0.5*i), 40, 0); //(x,y top left)
            glVertex3f(15+(0.5*i), 10, 0); //(x,y bottom left)
            glVertex3f(15+(0.5*(i+1)),10, 0); //(x,y bottom right)
            glVertex3f(15+(0.5*(i+1)),40, 0); //(x,y top right)
        }
    }
    if (draw_vecs == 1){
        for (int i = 0; i < 1001; i = i + 1){
            set_colormap(0.001*i,velocity_color, color_clamp_min, color_clamp_max, color_bands, scale_color);
            glVertex3f(15+(0.5*i), 70, 0); //(x,y top left)
            glVertex3f(15+(0.5*i), 40, 0); //(x,y bottom left)
            glVertex3f(15+(0.5*(i+1)),40, 0); //(x,y bottom right)
            glVertex3f(15+(0.5*(i+1)),70, 0); //(x,y top right)
        }
    }

    glEnd ();
    glPopMatrix ();

}

void MyGLWidget::OGL_Draw_Text(){
    //glPushMatrix();
    //glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    //qglColor(Qt::white);
    set_colormap(1-0.001,scalar_col, color_clamp_min, color_clamp_max,color_bands, scale_color);
    renderText(20, 15, 0, "0.001", QFont("Arial", 12, QFont::Bold, false) ); // render bottom bar left
    //qglColor(Qt::black);
    set_colormap(1-color_clamp_max,scalar_col, color_clamp_min, color_clamp_max,color_bands, scale_color);
    renderText(490, 15, 0, "1", QFont("Arial", 12, QFont::Bold, false) ); // render bottom bar right

    //QString maxCol = QString::number(color_clamp_max);

    set_colormap(1-0.001,velocity_color, color_clamp_min, color_clamp_max,color_bands, scale_color);
    renderText(20, 45, 0, "0.001", QFont("Arial", 12, QFont::Bold, false) ); // render top bar left
    set_colormap(1-color_clamp_max,velocity_color, color_clamp_min, color_clamp_max,color_bands, scale_color);
    //renderText(490, 45, 0, maxCol, QFont("Arial", 12, QFont::Bold, false) ); // render top bar right
    renderText(490, 45, 0, "1", QFont("Arial", 12, QFont::Bold, false) ); // render top bar right

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_LIGHTING);
    //glPopMatrix();

}

void MyGLWidget::setColorBands(int new_color_bands){
    color_bands = new_color_bands;
}

void MyGLWidget::setGlyphType(QString new_glyphs){
    glyphs = new_glyphs;
}
