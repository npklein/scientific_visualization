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
    hedgehog_scale = 2000;			//scaling of hedgehogs
    arrow_scale = 1000;			//scaling of hedgehogs
    draw_smoke = true;           //draw the smoke or not
    draw_vecs = true;            //draw the vector field or not
    scalar_col = 0;           //method for scalar coloring
    scale_color = false;    // if true, the lowest current value in the screen is the lowest in the color map, same for highest
    DIM = 50;
    color_clamp_min = 0.0;        // The lower bound value to clamp color map at
    color_clamp_max = 1.0;        // The higher bound value to clamp color map at
    velocity_color = 1;
    force_field_color = 1;
    grid_scale = 1;             // when drawing the grid, the size per cell is grid_scale * cell size, so with 50x50 grid with grid_scale = 10, 5 cells will be drawn
    color_bands = 256;
    draw_grid = false;
    glyphs = "hedgehogs";
    dataset = "fluid density";
    gradient = false;
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
    //glLoadIdentity();
    drawBar();
    fftw_real  cellWidth = (fftw_real)windowWidth / (fftw_real)(DIM + 1);   // Grid cell width
    fftw_real  cellHeight = (fftw_real)windowHeight / (fftw_real)(DIM + 1);  // Grid cell heigh
    if (draw_grid){
        drawGridLines(DIM, cellWidth, cellHeight);
    }
    if (draw_vecs)
    {
        drawVelocity(cellWidth, cellHeight);
    }
    if (draw_smoke)
    {
        drawSmoke(cellWidth, cellHeight);
    }
    if (gradient){
        drawGradient(cellWidth, cellHeight);
    }
    OGL_Draw_Text();
    glFlush();
}


void MyGLWidget::resizeGL(int width, int height)
{
    // removing below had no effect on what is drawn, so better to not use to lower complexity
    //glViewport(0.0f, 0.0f, (GLfloat)width, (GLfloat)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)width, 0.0, (GLdouble)height);
    windowWidth = width; windowHeight = height;
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
    simulation.drag(mx,my, DIM, windowWidth/2, windowHeight/2); // Works for Niek
    lastPos = event->pos();
}


void MyGLWidget::drawGradient(fftw_real cell_width, fftw_real cell_height)
{
    int  i, j;
/*
    for (i = 0; i < DIM; i++)
        for (j = 0; j < DIM; j++)
        {
            float center_rho =  simulation.get_rho()[(j * DIM) + i];
            float upper_left_rho =  simulation.get_rho()[((j-1) * DIM) + (i-1)];
            if (upper_left_rho > center_rho){
                Vector vector = Vector(cell_width + (fftw_real)i * cell_width,
                                       cell_height + (fftw_real)j * cell_height,
                                       cell_width + (fftw_real)(i-1) * cell_width,
                                       cell_height + (fftw_real)(j-1) * cell_height);
                drawArrow(vector, cell_width, cell_height, i, j);
            }
            float left_rho =  simulation.get_rho()[((j-1) * DIM) + i];
            if (left_rho > center_rho){
                Vector vector = Vector(cell_width + (fftw_real)i * cell_width,
                                       cell_height + (fftw_real)j * cell_height,
                                       cell_width + (fftw_real)i * cell_width,
                                       cell_height + (fftw_real)(j-1) * cell_height);
                drawArrow(vector, cell_width, cell_height, i, j);
            }
            float lower_left_rho =  simulation.get_rho()[((j-1) * DIM) + (i+1)];
            if (lower_left_rho > center_rho){
                Vector vector = Vector(cell_width + (fftw_real)i * cell_width,
                                       cell_height + (fftw_real)j * cell_height,
                                       cell_width + (fftw_real)(i+1) * cell_width,
                                       cell_height + (fftw_real)(j-1) * cell_height);
                drawArrow(vector, cell_width, cell_height, i, j);
            }
            float up_rho =  simulation.get_rho()[(j * DIM) + (i-1)];
            if (up_rho > center_rho){
                Vector vector = Vector(cell_width + (fftw_real)i * cell_width,
                                       cell_height + (fftw_real)j * cell_height,
                                       cell_width + (fftw_real)(i-1) * cell_width,
                                       cell_height + (fftw_real)j * cell_height);
                drawArrow(vector, cell_width, cell_height, i, j);
            }
            float below_rho =  simulation.get_rho()[(j * DIM) + (i+1)];
            if (below_rho > center_rho){
                Vector vector = Vector(cell_width + (fftw_real)i * cell_width,
                                       cell_height + (fftw_real)j * cell_height,
                                       cell_width + (fftw_real)(i+1) * cell_width,
                                       cell_height + (fftw_real)j * cell_height);
                drawArrow(vector, cell_width, cell_height, i, j);
            }
            float upper_right_rho =  simulation.get_rho()[((j+1) * DIM) + (i-1)];
            if (upper_right_rho > center_rho){
                Vector vector = Vector(cell_width + (fftw_real)i * cell_width,
                                       cell_height + (fftw_real)j * cell_height,
                                       cell_width + (fftw_real)(i-1) * cell_width,
                                       cell_height + (fftw_real)(j+1) * cell_height);
                drawArrow(vector, cell_width, cell_height, i, j);
            }
            float right_rho =  simulation.get_rho()[((j+1) * DIM) + (i)];
            if (right_rho > center_rho){
                Vector vector = Vector(cell_width + (fftw_real)i * cell_width,
                                       cell_height + (fftw_real)j * cell_height,
                                       cell_width + (fftw_real)i * cell_width,
                                       cell_height + (fftw_real)(j+1) * cell_height);
                drawArrow(vector, cell_width, cell_height, i, j);
            }
            float lower_right_rho =  simulation.get_rho()[((j+1) * DIM) + (i+1)];
            if (lower_right_rho > center_rho){
                Vector vector = Vector(cell_width + (fftw_real)i * cell_width,
                                       cell_height + (fftw_real)j * cell_height,
                                       cell_width + (fftw_real)(i+1) * cell_width,
                                       cell_height + (fftw_real)(j+1) * cell_height);
                drawArrow(vector, cell_width, cell_height, i, j);
            }
        }
        */
}

void MyGLWidget::drawVelocity(fftw_real cell_width, fftw_real cell_height)
{
    int  i, j, idx;

    for (i = 0; i < DIM; i++)
        for (j = 0; j < DIM; j++)
        {
            if (glyphs == "hedgehogs"){
                drawHedgehog(i, j, cell_width, cell_height);
            }
            if (glyphs == "arrows"){
                // if (i % 5 == 0 && j % 5 == 0){
                idx = (j * DIM) + i;
                Vector vector = Vector(cell_width + (fftw_real)i * cell_width, //x1
                                       cell_height + (fftw_real)j * cell_height, //y1
                                       (cell_width + (fftw_real)i * cell_width) + arrow_scale * simulation.get_vx()[idx], //x2
                                       (cell_height + (fftw_real)j * cell_height) + arrow_scale * simulation.get_vy()[idx]);//y2

                drawArrow(vector, cell_width, cell_height, i, j);
            }
        }
}

void MyGLWidget::drawArrow(Vector vector, fftw_real cell_width, fftw_real cell_height, int i, int j){
    // draw an error the size of a cell, scale according to vector length
    float angle = vector.normalize().direction2angle();

    set_colormap(vector.length()/15, velocity_color, color_clamp_min, color_clamp_max, color_bands);
    glPushMatrix();
    glTranslatef(cell_width*i,cell_height*j, 0);
    glRotated(angle,0,0,1);
    glScaled(log(vector.length()+1)/10,log(vector.length()+1)/5,0);
    glBegin(GL_TRIANGLES);
    float size_right = (cell_width/4)*3.0;
    float size_left = cell_width/4.0;
    float half_cell_height = cell_height/2.0;
    // arrow head, whole cell width, 1/3 of cell heigth
    glVertex2f(0, half_cell_height);            //base1
    glVertex2f(cell_width/2, cell_height);       //tip
    glVertex2f(cell_width, half_cell_height);    //base2
    // arrow tail (made up of 2 triangles)
    glVertex2f(size_right, half_cell_height);
    glVertex2f(size_left, 0);
    glVertex2f(size_left, half_cell_height);
    glVertex2f(size_right, 0);
    glVertex2f(size_right, half_cell_height);
    glVertex2f(size_left, 0);



    glEnd();
    glPopMatrix(); // now it's at normal scale again
    glLoadIdentity(); // needed to stop the rotating, otherwise rotates the entire drawing
}

void MyGLWidget::drawHedgehog(float i, float j, float cell_width, float cell_height){
    glBegin(GL_LINES);				//draw velocities
    int idx = (j * DIM) + i;
    direction_to_color(simulation.get_vx()[idx],simulation.get_vy()[idx], velocity_color, color_bands);
    glVertex2f(cell_width + (fftw_real)i * cell_width, cell_height + (fftw_real)j * cell_height);
    glVertex2f((cell_width + (fftw_real)i * cell_width) + hedgehog_scale * simulation.get_vx()[idx], (cell_height + (fftw_real)j * cell_height) + hedgehog_scale * simulation.get_vy()[idx]);
    glEnd();
}


void MyGLWidget::drawSmoke(fftw_real cell_width, fftw_real cell_height){
    int  i, j, idx0, idx1, idx2, idx3; double px0,py0,px1,py1,px2,py2,px3,py3;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);
    for (i = 0; i < DIM - 1; i++)			//draw smoke
    {
        for (j = 0; j < DIM - 1; j++)
        {
            px0  = cell_width + (fftw_real)i * cell_width;
            py0  = cell_height + (fftw_real)j * cell_height;
            idx0 = (j * DIM) + i;

            px1  = cell_width + (fftw_real)i * cell_width;
            py1  = cell_height + (fftw_real)(j + 1) * cell_height;
            idx1 = ((j + 1) * DIM) + i;

            px2  = cell_width + (fftw_real)(i + 1) * cell_width;
            py2  = cell_height + (fftw_real)(j + 1) * cell_height;
            idx2 = ((j + 1) * DIM) + (i + 1);

            px3  = cell_width + (fftw_real)(i + 1) * cell_width;
            py3  = cell_height + (fftw_real)j * cell_height;
            idx3 = (j * DIM) + (i + 1);
            set_colormap(simulation.get_rho()[idx0], scalar_col, color_clamp_min, color_clamp_max, color_bands);
            glVertex2f(px0, py0);
            set_colormap(simulation.get_rho()[idx1], scalar_col, color_clamp_min, color_clamp_max, color_bands);
            glVertex2f(px1, py1);
            set_colormap(simulation.get_rho()[idx2], scalar_col, color_clamp_min, color_clamp_max, color_bands);
            glVertex2f(px2, py2);

            set_colormap(simulation.get_rho()[idx0], scalar_col, color_clamp_min, color_clamp_max, color_bands);
            glVertex2f(px0, py0);
            set_colormap(simulation.get_rho()[idx2], scalar_col, color_clamp_min, color_clamp_max, color_bands);
            glVertex2f(px2, py2);
            set_colormap(simulation.get_rho()[idx3], scalar_col, color_clamp_min, color_clamp_max, color_bands);
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


void MyGLWidget::showAnimation(bool new_frozen)
{
    // ! because if the checkbox = true, frozen should be set to false
    simulation.set_frozen(!new_frozen);
}

void MyGLWidget::drawMatter(bool new_draw_smoke)
{
    draw_smoke = new_draw_smoke;
    if (!new_draw_smoke) {
        draw_vecs = true;
    }
}

void MyGLWidget::drawHedgehogs(bool new_draw_vecs)
{
    draw_vecs = new_draw_vecs;
    if (!draw_vecs) draw_smoke = true;
}

void MyGLWidget::drawGrid(bool new_draw_grid)
{
    draw_grid = new_draw_grid;
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

void MyGLWidget::setGridSize(int position)
{
    grid_scale = position;
}


void MyGLWidget::hedgehogScaling(int position)
{
    // vec_scale = 1000;
    //  	  case 'S': vec_scale *= 1.2; break;
    //        case 's': vec_scale *= 0.8; break;
    // The scaling goes exponential with keyboard, but with slide can just do linear
    if (glyphs == "hedgehogs"){
        static int last_pos_hedgehog = 500;				//remembers last slider location
        int new_pos = position - last_pos_hedgehog;
        hedgehog_scale = hedgehog_scale + new_pos * 200; //easier to debug on separate line
        if (hedgehog_scale < 0){
            hedgehog_scale = 0;
        }
        if (hedgehog_scale > 4000){
            hedgehog_scale = 4000;
        }
        last_pos_hedgehog = position;
    }
    if (glyphs == "arrows"){
        static int last_pos_arrow = 500;				//remembers last slider location
        int new_pos = position - last_pos_arrow;
        arrow_scale = arrow_scale + new_pos*2; //easier to debug on separate line
        if (arrow_scale < 0){
            arrow_scale = 0;
        }
        if (arrow_scale > 2000){
            arrow_scale = 2000;
        }
        last_pos_arrow = position;
    }
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
    if (draw_smoke){
        for (int i = 0; i < 1001; i = i + 1){
            set_colormap(0.001*i,scalar_col, color_clamp_min, color_clamp_max, color_bands);
            glVertex3f(15+(0.5*i), 40, 0); //(x,y top left)
            glVertex3f(15+(0.5*i), 10, 0); //(x,y bottom left)
            glVertex3f(15+(0.5*(i+1)),10, 0); //(x,y bottom right)
            glVertex3f(15+(0.5*(i+1)),40, 0); //(x,y top right)
        }
    }
    if (draw_vecs){
        for (int i = 0; i < 1001; i = i + 1){
            set_colormap(0.001*i,velocity_color, color_clamp_min, color_clamp_max, color_bands);
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
    if (draw_smoke){
        //qglColor(Qt::white);
        set_colormap(1-0.001,scalar_col, color_clamp_min, color_clamp_max,color_bands);
        renderText(20, 15, 0, "0.001", QFont("Arial", 12, QFont::Bold, false) ); // render bottom bar left
        //qglColor(Qt::black);
        set_colormap(1-color_clamp_max,scalar_col, color_clamp_min, color_clamp_max,color_bands);
        renderText(490, 15, 0, "1", QFont("Arial", 12, QFont::Bold, false) ); // render bottom bar right
}
    //QString maxCol = QString::number(color_clamp_max);
    if (draw_vecs){
        set_colormap(1-0.001,velocity_color, color_clamp_min, color_clamp_max,color_bands);
        renderText(20, 45, 0, "0.001", QFont("Arial", 12, QFont::Bold, false) ); // render top bar left
        set_colormap(1-color_clamp_max,velocity_color, color_clamp_min, color_clamp_max,color_bands);
        //renderText(490, 45, 0, maxCol, QFont("Arial", 12, QFont::Bold, false) ); // render top bar right
        renderText(490, 45, 0, "1", QFont("Arial", 12, QFont::Bold, false) ); // render top bar right
}
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_LIGHTING);
    //glPopMatrix();

}

void MyGLWidget::setColorBands(int new_color_bands){
    color_bands = new_color_bands;
}

void MyGLWidget::setDim(int new_DIM){
    DIM = new_DIM;
    simulation.init_simulation(DIM);
}


void MyGLWidget::setGlyphType(QString new_glyphs){
    glyphs = new_glyphs;
}

void MyGLWidget::drawGridLines(int DIM, int wn, int hn){
    glBegin(GL_LINES);
    for(int i=0;i<=DIM/grid_scale;i++) {
        glColor3f(1,1,1);
        glVertex2f(i*wn*grid_scale,0);
        glVertex2f(i*wn*grid_scale,DIM*hn*grid_scale);
        glVertex2f(0,i*hn*grid_scale);
        glVertex2f(DIM*wn*grid_scale,i*hn*grid_scale);
    };
    glEnd();
}

void MyGLWidget::setDrawGradient(bool new_gradient){
    gradient = new_gradient;
}
