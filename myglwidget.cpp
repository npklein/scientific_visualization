// myglwidget.cpp

#include <QtWidgets>
#include <QtOpenGL>
#include <GLUT/glut.h>          //the GLUT graphics library
#include "myglwidget.h"
#include "visualization.cpp"
#include <simulation.cpp>              //the numerical simulation FFTW library
#include <cmath>
#include "vector.cpp"
#include "grid.h"
#include <deque>

MyGLWidget::MyGLWidget(QWidget *parent)
{
    //--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
    hedgehog_scale = 2000;			//scaling of hedgehogs
    arrow_scale = 1000;			//scaling of arrows
    cone_scale = 1000;          //scaling of cones
    draw_smoke = true;           //draw the smoke or not
    draw_vecs = true;            //draw the vector field or not
    draw_default_points = true;
    draw_selected_points = false;
    draw_v = true; // draw velocity
    draw_f = false; // draw forcefield
    scalar_col = 0;           //method for scalar coloring
    scale_color = false;    // if true, the lowest current value in the screen is the lowest in the color map, same for highest
    DIM = 50;
    // should change to have a color map class that has color clamp values
    color_clamp_min_matter = 0.0;        // The lower bound value to clamp color map at
    color_clamp_max_matter = 1.0;        // The higher bound value to clamp color map at
    color_clamp_min_glyph = 0.0;        // The lower bound value to clamp color map at
    color_clamp_max_glyph = 1.0;        // The higher bound value to clamp color map at
    velocity_color = 1;
    number_of_glyphs = DIM*2;
    force_field_color = 1;
    grid_scale = 1;             // when drawing the grid, the size per cell is grid_scale * cell size, so with 50x50 grid with grid_scale = 10, 5 cells will be drawn
    color_bands = 256;
    hue_glyph = 0;
    hue_matter = 0;
    saturation_matter = 1.0;
    saturation_glyph = 1.0;
    draw_grid = false;
    draw_slices = false;
    show_points = false;
    glyphs = "hedgehogs";
    dataset = "fluid velocity magnitude";
    gradient = false;
    draw_streamline = false;
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

void MyGLWidget::defaultPoints(std::vector<int> &points_x, std::vector<int> &points_y){
    for (int i = 0; i < DIM; i++){
        for (int j = 0; j < DIM; j++){
            points_x.insert(points_x.end(), i);
            points_y.insert(points_y.end(), j);
        }
    }
}

void MyGLWidget::selectedPoints(std::vector<int> &points_x, std::vector<int> &points_y){
    for (unsigned i = 0; i < mouse_x.size(); i++){
        points_x.insert(points_x.end(), mouse_x[i]);
    }
    for (unsigned i = 0; i < mouse_y.size(); i++){
        points_y.insert(points_y.end(),mouse_y[i] );
    }
}

void MyGLWidget::paintGL() //glutDisplayFunc(display);
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_TABLE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    drawBar();
    cell_width = ceil((fftw_real)windowWidth / (fftw_real)(DIM));   // Grid cell width
    cell_height = ceil((fftw_real)windowHeight / (fftw_real)(DIM));  // Grid cell heigh

    if (draw_grid){
        drawGridLines(DIM);
    }
    if(draw_slices){
        drawSlices(2);
    }
    if (draw_streamline){
        drawStreamline();
    }
    if (draw_vecs)
    {
        if(draw_v){
            drawVelocity(simulation.get_vx(), simulation.get_vy());
        }
        else if (draw_f){
            drawForcefield(simulation.get_fx(), simulation.get_fy());
        }
    }
    if (gradient){
        drawGradient();
    }
    if (show_points && ( select_points || draw_selected_points || draw_streamline )){
        drawSelectedPoints();
    }
    if (draw_smoke)
    {
        drawSmoke();
    }

    //OGL_Draw_Text();
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

void MyGLWidget::drawSelectedPoints(){
    float x2,y2;
    float radius  = 2;
    float angle   = 1.0;

    for (unsigned i = 0; i < mouse_x.size(); i++){
        glColor3f(255,0,0);
        glBegin(GL_TRIANGLE_FAN);

        int x = mouse_x[i];
        int y = mouse_y[i];


        for (angle=1.0f;angle<361.0f;angle+=0.2)
        {
            x2 = x+sin(angle)*radius;
            y2 = y+cos(angle)*radius;
            glVertex2f(x2,y2);
        }
        glEnd();
    }

}

void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    if(select_points){
        mouse_x.insert(mouse_x.end(), lastPos.x());
        mouse_y.insert(mouse_y.end(), windowHeight - lastPos.y());
    }
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int mx = event->x();// - lastposition gets calculated in drag(), could save a step by using lastPos.x/y but leaving it like this is safer
    int my = event->y();
    simulation.drag(mx,my, DIM, windowWidth, windowHeight);  // Works for Freerk when using external display
    //simulation.drag(mx,my, DIM, windowWidth, windowHeight); // Works for Niek
}

void MyGLWidget::drawGradient()
{
    for (int i = 0; i < DIM; i++)
        for (int j = 0; j < DIM; j++)
        {
            float left_rho =  simulation.get_rho()[((j-1) * DIM) + i];
            float up_rho =  simulation.get_rho()[(j * DIM) + (i-1)];
            float below_rho =  simulation.get_rho()[(j * DIM) + (i+1)];
            float right_rho =  simulation.get_rho()[((j+1) * DIM) + (i)];
            float x = left_rho - right_rho;
            float y = up_rho - below_rho;
            Vector v = Vector(y, x);
            if (v.length() > 0){
                drawArrow(v, i*cell_width, j*cell_height, v.length(), 2, 0, 1);
            }
        }
}

void MyGLWidget::drawVelocity(fftw_real *vx, fftw_real *vy)
{
    std::vector<int> points_x;
    std::vector<int> points_y;
    if(draw_selected_points){
        selectedPoints(points_x, points_y);
    }
    else if(draw_default_points){
        defaultPoints(points_x, points_y);
    }
    //  for (int i = 0; i < DIM; i++)
    //      for (int j = 0; j < DIM; j++)
    for (unsigned y = 0; y < points_x.size(); y++)
    {
        float vx_draw = 0;
        float vy_draw = 0;
        fftw_real x_coord;
        fftw_real y_coord;

        if(draw_selected_points){
            float point_x = (float)points_x[y]/cell_width;
            float point_y = (float)points_y[y]/cell_height;

            Vector interpolated_vector = interpolateVector(point_x, point_y, cell_width, cell_height, DIM, simulation);
            vx_draw = interpolated_vector.X;
            vy_draw = interpolated_vector.Y;
            x_coord = (fftw_real)points_x[y];
            y_coord = (fftw_real)points_y[y];
        }

        if (draw_default_points){
            int i = points_x[y];
            int j = points_y[y];
            int idx = (j * DIM) + i;
            vx_draw = vx[idx];
            vy_draw = vy[idx];
            x_coord = (fftw_real)i * cell_width;
            y_coord = (fftw_real)j * cell_height;
        }
        if (glyphs == "hedgehogs"){
            direction_to_color(vx_draw, vy_draw, velocity_color, color_bands, color_clamp_min_glyph, color_clamp_max_glyph, hue_glyph, saturation_glyph);
            if (y % (100/number_of_glyphs)  == 0|| y ==0){
                glBegin(GL_LINES);				//draw velocities
                glVertex2f(x_coord, y_coord);
                glVertex2f(x_coord + hedgehog_scale * vx_draw, y_coord + hedgehog_scale * vy_draw);
                glEnd();
            }
        }
        else if (glyphs == "arrows"){
            if (y % (100/number_of_glyphs)  == 0|| y ==0){
                Vector vector = Vector(x_coord, //x1
                                       y_coord, //y1
                                       (x_coord) + arrow_scale * vx_draw, //x2
                                       (y_coord) + arrow_scale * vy_draw);//y2
                Vector unscaled_vector = Vector(x_coord, //x1
                                                y_coord, //y1
                                                (x_coord) + 70 * vx_draw, //x2
                                                (y_coord) + 70 * vy_draw);//y2
                drawArrow(vector, x_coord, y_coord, unscaled_vector.length(), 10, simulation.get_vy_min(), simulation.get_vy_max());
            }
        }
        else if (glyphs == "cones"){
            //
            if (y % (100/number_of_glyphs)  == 0|| y ==0){
                Vector vector = Vector(x_coord, //x1
                                       y_coord, //y1
                                       (x_coord) + cone_scale * vx_draw, //x2
                                       (y_coord) + cone_scale * vy_draw);//y2
                Vector unscaled_vector = Vector(x_coord, //x1
                                                y_coord, //y1
                                                (x_coord) + 70 * vx_draw, //x2
                                                (y_coord) + 70 * vy_draw);//y2
                drawCone(vector, x_coord, y_coord, unscaled_vector.length(), 10, simulation.get_vy_min(), simulation.get_vy_max());
            }
        }
    }
}

void MyGLWidget::drawForcefield(fftw_real *fx, fftw_real *fy)
{
    for (int i = 0; i < DIM; i++)
        for (int j = 0; j < DIM; j++)
        {
            int idx = (j * DIM) + i;
            if (glyphs == "hedgehogs"){
                if (i % (100/number_of_glyphs) == 0 && j % (100/number_of_glyphs)  == 0){
                    glBegin(GL_LINES);				//draw velocities
                    direction_to_color(fx[idx], fy[idx], velocity_color, color_bands, color_clamp_min_glyph, color_clamp_max_glyph, hue_glyph, saturation_glyph);
                    glVertex2f((fftw_real)i * cell_width, (fftw_real)j * cell_height);
                    glVertex2f((fftw_real)i * cell_width + hedgehog_scale * fx[idx], (fftw_real)j * cell_height + hedgehog_scale * fy[idx]);
                    glEnd();
                }
            }
            else if (glyphs == "arrows"){
                if (i % (100/number_of_glyphs) == 0 && j % (100/number_of_glyphs)  == 0){
                    int idx = (j * DIM) + i;
                    Vector vector = Vector((fftw_real)i * cell_width, //x1
                                           (fftw_real)j * cell_height, //y1
                                           ((fftw_real)i * cell_width) + arrow_scale * fx[idx], //x2
                                           ((fftw_real)j * cell_height) + arrow_scale * fy[idx]);//y2

                    drawArrow(vector, i, j, vector.length()/15, 10, simulation.get_fx_min(), simulation.get_fx_max());
                }
            }
            else if (glyphs == "cones"){
                //
                if (i % (100/number_of_glyphs) == 0 && j % (100/number_of_glyphs)  == 0){
                    int idx = (j * DIM) + i;
                    Vector vector = Vector((fftw_real)i * cell_width, //x1
                                           (fftw_real)j * cell_height, //y1
                                           ((fftw_real)i * cell_width) + cone_scale * fx[idx], //x2
                                           ((fftw_real)j * cell_height) + cone_scale * fy[idx]);//y2

                    drawCone(vector, i, j, vector.length()/15, 10, simulation.get_fx_min(), simulation.get_fx_max());
                }
            }
        }
}

void MyGLWidget::drawArrow(Vector vector, int x_coord, int y_coord, float vy, int scaling_factor, float vy_min, float vy_max){
    // draw an arrow the size of a cell, scale according to vector length
    float angle = vector.normalize().direction2angle();

    set_colormap(vy, velocity_color, color_clamp_min_glyph, color_clamp_max_glyph, color_bands, hue_glyph, saturation_glyph,scale_color, vy_min, vy_max);
    glPushMatrix();
    glTranslatef(x_coord,y_coord, 0);
    glRotated(angle,0,0,1);
    glScaled(log(vector.length()/scaling_factor+1),log(vector.length()/(scaling_factor/2)+1),0);
    //glScaled(log(vector.length()/2+1),log(vector.length()*5+1),0);
    glBegin(GL_TRIANGLES);
    // arrow base size of 2/20th of cell width
    float size_right = (cell_width/20)*11.0;
    float size_left = (cell_width/20)*9.0;
    float half_cell_height = cell_height/2.0;
    // arrow head, whole cell width, 1/3 of cell heigth
    glVertex2f(0, half_cell_height);            //base1
    glVertex2f(cell_width/2, cell_height);       //tip
    glVertex2f(cell_width, half_cell_height);    //base2
    // arrow tail (made up of 2 triangles)
    glScaled(log(vector.length()+1),log(vector.length()+1),0);
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

void MyGLWidget::drawCone(Vector vector, int i, int j, float vy, int scaling_factor, float vy_min, float vy_max){
    // draw
    float angle = vector.normalize().direction2angle();

    set_colormap(vy, velocity_color, color_clamp_min_glyph, color_clamp_max_glyph, color_bands, hue_glyph, saturation_glyph, scale_color, vy_min, vy_max);
    glPushMatrix();
    glTranslatef(i,j, 0);
    glRotated(angle,0,0,1);
    glScaled(log(vector.length()/scaling_factor+1),log(vector.length()/(scaling_factor/2)+1),0);

    // draw the upper part of the cone
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    // Smooth shading
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glEnable (GL_LIGHTING);
    float radius = cell_height/3; // calculate radius
    // foreach degree angle draw circle + arrow point
    for (int angle = 1; angle <= 360; angle++) {
        glColor4f(0,0,0,0.5-(0.5/angle)); // colors(R, G, B, alpha)
        glVertex2f(cell_width/2, cell_height); // draw cone point/tip
        glVertex2f(sin(angle) * radius, cos(angle) * radius); // draw cone base (circle)
    }

    glEnd();
    glPopMatrix(); // now it's at normal scale again
    glLoadIdentity(); // needed to stop the rotating, otherwise rotates the entire drawing
}

void MyGLWidget::drawSlices(int n){
    // n = number of slices (timepoints) to draw
    // use std::queue instead of std::list because it forces FIFO
        std::deque<Grid> grid_timepoints;
        for(int y = 0; y < n; y++){
            do_one_simulation_step(false);
            Grid grid = Grid(DIM);

            defaultPoints(points_x, points_y);
            //  for (int i = 0; i < DIM; i++)
            //      for (int j = 0; j < DIM; j++)
            for (unsigned y = 0; y < points_x.size(); y++)
            {
                int i = points_x[y];
                int j = points_y[y];
                int idx = (j * DIM) + i;
                grid.addElementToGrid(simulation.get_vx()[idx], simulation.get_vy()[idx], idx);
            }
            grid_timepoints.push_front(grid);
        }
        Grid popped_grid = grid_timepoints.front();
        grid_timepoints.pop_back();
        drawVelocity(popped_grid.vx, popped_grid.vy);
        //updateGL();
    }
    void MyGLWidget::drawStreamline()
    {
        float dt = cell_width/3;
        float max_size = cell_width*100;
        //drawStreamline(25,25);
        selectedPoints(points_x, points_y);

        for (unsigned s = 0; s < points_x.size(); s++)
        {
            float start_x = (float)points_x[s];
            float start_y = (float)points_y[s];
            for (int y = 0; y < max_size; y+=dt){
                Vector interpolated_vector = interpolateVector(start_x/cell_width, start_y/cell_height, cell_width, cell_height, DIM, simulation);
                // if outside the grid, stop the stream line
                //if(interpolated_vector.X > DIM*cell_width || interpolated_vector.Y > DIM*cell_height || interpolated_vector.X <0 || interpolated_vector.Y <0 ){
                //    return;
                //}
                float length  = interpolated_vector.length();

                if(length>0){
                    interpolated_vector.X = interpolated_vector.X / length;
                    interpolated_vector.Y = interpolated_vector.Y / length;
                    interpolated_vector.X += interpolated_vector.X * dt;
                    interpolated_vector.Y += interpolated_vector.Y * dt;

                    //DRAW
                    glBegin(GL_LINES);				//draw
                    qglColor(Qt::white);
                    glVertex2f(start_x, start_y);
                    glVertex2f(interpolated_vector.X*cell_width+points_x[s], interpolated_vector.Y*cell_height+points_y[s]);
                    glEnd();
                    start_x = interpolated_vector.X*cell_width+points_x[s];
                    start_y = interpolated_vector.Y*cell_height+points_y[s];
                }
            }
        }
    }

    void MyGLWidget::drawSmoke(){
        int  i, j, idx0, idx1, idx2, idx3; double px0,py0,px1,py1,px2,py2,px3,py3;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_TRIANGLES);
        for (i = 0; i < DIM; i++)			//draw smoke
        {
            for (j = 0; j < DIM; j++)
            {
                px0  = floor((fftw_real)i * cell_width);
                py0  = floor((fftw_real)j * cell_height);
                idx0 = (j * DIM) + i;

                px1  = floor((fftw_real)i * cell_width);
                py1  = floor((fftw_real)(j + 1) * cell_height);
                idx1 = ((j + 1) * DIM) + i;

                px2  = floor((fftw_real)(i + 1) * cell_width);
                py2  = floor((fftw_real)(j + 1) * cell_height);
                idx2 = ((j + 1) * DIM) + (i + 1);

                px3  = floor((fftw_real)(i + 1) * cell_width);
                py3  = floor((fftw_real)j * cell_height);
                idx3 = (j * DIM) + (i + 1);
                set_colormap(simulation.get_rho()[idx0], scalar_col, color_clamp_min_matter, color_clamp_max_matter, color_bands, hue_matter,
                             saturation_matter, scale_color, simulation.get_rho_min(), simulation.get_rho_max());
                glVertex2f(px0, py0);
                set_colormap(simulation.get_rho()[idx1], scalar_col, color_clamp_min_matter, color_clamp_max_matter, color_bands, hue_matter,
                             saturation_matter, scale_color, simulation.get_rho_min(), simulation.get_rho_max());
                glVertex2f(px1, py1);
                set_colormap(simulation.get_rho()[idx2], scalar_col, color_clamp_min_matter, color_clamp_max_matter, color_bands, hue_matter,
                             saturation_matter, scale_color, simulation.get_rho_min(), simulation.get_rho_max());
                glVertex2f(px2, py2);

                set_colormap(simulation.get_rho()[idx0], scalar_col, color_clamp_min_matter, color_clamp_max_matter, color_bands, hue_matter,
                             saturation_matter, scale_color, simulation.get_rho_min(), simulation.get_rho_max());
                glVertex2f(px0, py0);
                set_colormap(simulation.get_rho()[idx2], scalar_col, color_clamp_min_matter, color_clamp_max_matter, color_bands, hue_matter,
                             saturation_matter, scale_color, simulation.get_rho_min(), simulation.get_rho_max());
                glVertex2f(px2, py2);
                set_colormap(simulation.get_rho()[idx3], scalar_col, color_clamp_min_matter, color_clamp_max_matter, color_bands, hue_matter,
                             saturation_matter, scale_color, simulation.get_rho_min(), simulation.get_rho_max());
                glVertex2f(px3, py3);
            }
        }
        glEnd();
    }

    void MyGLWidget::do_one_simulation_step(bool update)
    {
        if (!simulation.get_frozen())
        {
            simulation.set_forces(DIM);
            simulation.solve(DIM, simulation.get_vx(), simulation.get_vy(), simulation.get_vx0(),
                             simulation.get_vy0(), simulation.get_visc(), simulation.get_dt());
            // Note to self: * in *simulation.get_vx() because simulation.get_vx() returns 'double *' and diffuse_matter needs 'double'
            simulation.diffuse_matter(DIM, simulation.get_vx(), simulation.get_vy(),
                                      simulation.get_rho(), simulation.get_rho0(), simulation.get_dt());
            if(update){
                updateGL();
            }
        }
    }

    void MyGLWidget::do_one_simulation_step()
    {
        do_one_simulation_step(true);
    }

    void MyGLWidget::showAnimation(bool new_frozen)
    {
        // ! because if the checkbox = true, frozen should be set to false
        simulation.set_frozen(!new_frozen);
    }

    void MyGLWidget::drawMatter(bool new_draw_smoke)
    {
        draw_smoke = new_draw_smoke;
        if (!draw_smoke) {
            draw_vecs = true;
        }
        else{draw_slices = false;}

    }

    void MyGLWidget::drawHedgehogs(bool new_draw_vecs)
    {
        draw_vecs = new_draw_vecs;
        if (!draw_vecs) {draw_smoke = true;}
        else{draw_slices = false;}
    }

    void MyGLWidget::drawGrid(bool new_draw_grid)
    {
        draw_grid = new_draw_grid;
    }

    void MyGLWidget::scaleColors(bool new_scale_color)
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
            hedgehog_scale = position*2;
        }
        if (glyphs == "arrows"){
            arrow_scale = position*1.5;
        }
        if (glyphs == "cones"){
            cone_scale = position*3; //easier to debug on separate line
        }
    }

    void MyGLWidget::fluidViscosity(int position)
    {
        // visc = 0.001
        //      case 'V': simulation.set_visc(simulation.get_visc()*5); break;
        //      case 'v': simulation.set_visc(simulation.get_visc()*0.2); break;
        // The scaling goes exponential with keyboard, but with slide can just do linear
        float visc =  position*0.00001;
        simulation.set_visc(visc);
    }

    void MyGLWidget::setNumberOfGlyphs(int position)
    {
        number_of_glyphs = position;
    }

    void MyGLWidget::clampColorMin(int min_color)
    {
        if (min_color > 0){
            if(dataset == "fluid density"){
                color_clamp_min_matter = min_color/100.0;
            }
            if(dataset == "fluid velocity magnitude" || dataset == "force field magnitude"){
                color_clamp_min_glyph = min_color/100.0;
            }
        }
    }

    void MyGLWidget::clampColorMax(int max_color)
    {
        if (max_color > 0){
            if(dataset == "fluid density"){
                color_clamp_max_matter = 1-(max_color/100.0);
            }
            if(dataset == "fluid velocity magnitude" || dataset == "force field magnitude"){
                color_clamp_max_glyph = 1-(max_color/100.0);
            }
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
        else if(scalartype == "zebrafish"){
            if (dataset == "fluid density"){
                scalar_col = 3;
            }
            else if (dataset == "fluid velocity magnitude"){
                velocity_color = 3;
            }
            else if (dataset == "force field magnitude"){
                force_field_color = 3;
            }
        }
    }

    void MyGLWidget::setFluidDensity(){
        dataset = "fluid density";
    }

    void MyGLWidget::setFluidVelocity(){
        dataset = "fluid velocity magnitude";
        draw_v = true;
        draw_f = false;
    }

    void MyGLWidget::setForceField(){
        dataset = "force field magnitude";
        draw_f = true;
        draw_v = false;
    }

    // Color map explained
    // http://www.glprogramming.com/red/chapter04.html just above table 4.2
    // The first float is the offset color to start the map of R,G,B from

    void MyGLWidget::drawBar(){
        glPushMatrix ();
        glBegin (GL_QUADS);
        if (draw_smoke){
            for (int i = 0; i < 1001; i = i + 1){
                float rho_min = 0;
                float rho_max = 0;
                if (scale_color){
                    rho_min = simulation.get_rho_min();
                    rho_max = simulation.get_rho_max();
                }
                set_colormap(0.001*i,scalar_col, color_clamp_min_matter, color_clamp_max_matter, color_bands, hue_matter, saturation_matter, scale_color, rho_min, rho_max);
                glVertex3f(15+(0.5*i), 40, 0); //(x,y top left)
                glVertex3f(15+(0.5*i), 10, 0); //(x,y bottom left)
                glVertex3f(15+(0.5*(i+1)),10, 0); //(x,y bottom right)
                glVertex3f(15+(0.5*(i+1)),40, 0); //(x,y top right)
            }
        }
        if (draw_vecs){
            for (int i = 0; i < 1001; i = i + 1){
                set_colormap(0.001*i,velocity_color, color_clamp_min_glyph, color_clamp_max_glyph, color_bands, hue_glyph, saturation_glyph, scale_color, 0, 1);
                glVertex3f(15+(0.5*i), 70, 0); //(x,y top left)
                glVertex3f(15+(0.5*i), 40, 0); //(x,y bottom left)
                glVertex3f(15+(0.5*(i+1)),40, 0); //(x,y bottom right)
                glVertex3f(15+(0.5*(i+1)),70, 0); //(x,y top right)
            }
        }
        glEnd ();
        glPopMatrix ();
        OGL_Draw_Text();
    }

    void MyGLWidget::OGL_Draw_Text(){
        //glPushMatrix();
        //glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        QString text_min = QString::number(color_clamp_min_matter);
        QString text_max = QString::number(color_clamp_max_matter);
        if (draw_smoke){
            //qglColor(Qt::white);
            if(scale_color){
                // to round off to 1 decimal
                text_min = QString::number(floor(simulation.get_rho_min()*10)/10);
                text_max = QString::number(floor(simulation.get_rho_max()*10)/10);
            }
            set_colormap(1-color_clamp_min_matter,scalar_col, color_clamp_min_matter, color_clamp_max_matter,color_bands, hue_matter, 1, scale_color, 0, 1);
            renderText(20, 15, 0, text_min, QFont("Arial", 12, QFont::Bold, false) ); // render bottom bar left
            //qglColor(Qt::black);
            renderText(240, 15, 0, "matter", QFont("Arial", 8, QFont::Bold, false) );
            set_colormap(1-color_clamp_max_matter, scalar_col, color_clamp_min_matter, color_clamp_max_matter, color_bands, hue_matter, 1, scale_color, 0, 1);
            renderText(470, 15, 0, text_max, QFont("Arial", 12, QFont::Bold, false) ); // render bottom bar right
        }
        //QString maxCol = QString::number(color_clamp_max);
        if (draw_vecs){
            set_colormap(1-color_clamp_min_glyph,velocity_color, color_clamp_min_glyph, color_clamp_max_glyph,color_bands, hue_glyph, 1, scale_color, 0, 1);
            renderText(20, 45, 0, QString::number(color_clamp_min_glyph), QFont("Arial", 12, QFont::Bold, false) ); // render top bar left
            renderText(240, 45, 0, "glyph", QFont("Arial", 8, QFont::Bold, false) );
            set_colormap(1-color_clamp_max_glyph,velocity_color, color_clamp_min_glyph, color_clamp_max_glyph,color_bands, hue_glyph, 1, scale_color, 0, 1);
            renderText(470, 45, 0, QString::number(color_clamp_max_glyph), QFont("Arial", 12, QFont::Bold, false) ); // render top bar right
        }
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_LIGHTING);
        //glPopMatrix();

    }

    void MyGLWidget::setHue(int new_hue){
        if(dataset == "fluid density"){
            hue_matter = new_hue;
        }
        if(dataset == "fluid velocity magnitude" || dataset == "force field magnitude"){
            hue_glyph = new_hue;
        }
    }

    void MyGLWidget::setSaturation(int new_saturation){
        if(dataset == "fluid density"){
            saturation_matter = new_saturation/100.0;
        }
        if(dataset == "fluid velocity magnitude" || dataset == "force field magnitude"){
            saturation_glyph = new_saturation/100.0;
        }
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

    void MyGLWidget::clearSelectedPoints(){
        points_x.clear();
        points_y.clear();
        mouse_x.clear();
        mouse_y.clear();
    }

    void MyGLWidget::drawGridLines(int DIM){
        glBegin(GL_LINES);
        for(int i=0;i <= DIM/grid_scale;i++) {
            glColor3f(1,1,1);
            glVertex2f(i*cell_width*grid_scale,0);
            glVertex2f(i*cell_width*grid_scale,DIM*cell_height*grid_scale);
            glVertex2f(0,i*cell_height*grid_scale);
            glVertex2f(DIM*cell_width*grid_scale,i*cell_height*grid_scale);
        };
        glEnd();
    }

    void MyGLWidget::setDrawGradient(bool new_gradient){
        gradient = new_gradient;
        if (gradient){
            draw_slices = false;
        }
    }

    void MyGLWidget::setDrawStreamline(bool new_streamline){
        draw_streamline = new_streamline;
        if (draw_streamline) {
            draw_vecs = false;
            draw_slices = false;
            draw_smoke = false;
        }
        else{
            draw_slices = true;
        }
    }

    void MyGLWidget::setDrawSlices(bool new_slices){
        draw_slices = new_slices;
        if (draw_slices) {
            draw_vecs = false;
            draw_streamline = false;
            draw_smoke = false;
        }
        if (!draw_slices) {
            draw_streamline = true;
        }
    }

    void MyGLWidget::selectPoints(bool new_select_points){
        select_points = new_select_points;
        draw_vecs = !new_select_points;
        draw_smoke = !new_select_points;
        show_points = true;
        if(draw_streamline){
            draw_smoke = false;
            draw_streamline = false;
        }
    }

    void MyGLWidget::selectPointsStreamline(bool new_select_points){
        select_points = new_select_points;
        draw_streamline = !new_select_points;
        if(new_select_points){
            draw_smoke = false;
            draw_vecs = false;
            show_points = true;
        }
    }

    void MyGLWidget::drawDefaultPoints(){
        draw_default_points = true;
        draw_selected_points = false;
    }

    void MyGLWidget::setDrawSelectedPoints(){
        draw_default_points = false;
        draw_selected_points = true;
    }

    void MyGLWidget::showPoints(bool new_show_points){
        show_points = new_show_points;
    }

