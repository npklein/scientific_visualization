// Usage: Drag with the mouse to add smoke to the fluid. This will also move a "rotor" that disturbs
//        the velocity field at the mouse location. Press the indicated keys to change options
//--------------------------------------------------------------------------------------------------


#include <stdio.h>              //for printing the help text
#include <math.h>               //for various math functions
#include <GLUT/glut.h>            //the GLUT graphics library
#include "simulation.c"



//--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
int   winWidth, winHeight;      //size of the graphics window, in pixels
int   color_dir = 0;            //use direction color-coding or not
float vec_scale = 1000;			//scaling of hedgehogs
int   draw_smoke = 0;           //draw the smoke or not
int   draw_vecs = 1;            //draw the vector field or not
const int COLOR_BLACKWHITE=0;   //different types of color mapping: black-and-white, rainbow, banded
const int COLOR_RAINBOW=1;
const int COLOR_BANDS=2;
int   scalar_col = 0;           //method for scalar coloring
//int   frozen = 0;               //toggles on/off the animation






//------ VISUALIZATION CODE STARTS HERE -----------------------------------------------------------------


//rainbow: Implements a color palette, mapping the scalar 'value' to a rainbow color RGB
void rainbow(float value,float* R,float* G,float* B)
{
   const float dx=0.8f;
   if (value<0) value=0; if (value>1) value=1;
   value = (6-2*dx)*value+dx;
      
   *R = max(0.0f,(3-(float)fabs(value-4)-(float)fabs(value-5))/2);
   *G = max(0.0f,(4-(float)fabs(value-2)-(float)fabs(value-4))/2);
   *B = max(0.0f,(3-(float)fabs(value-1)-(float)fabs(value-2))/2);
}

//set_colormap: Sets three different types of colormaps
void set_colormap(float vy)
{
   float R,G,B;

   if (scalar_col==COLOR_BLACKWHITE)
   {
       R = G = B = vy;
   }
   else if (scalar_col==COLOR_RAINBOW)
   {
       rainbow(vy,&R,&G,&B);
   }
   else if (scalar_col==COLOR_BANDS)
       {
          const int NLEVELS = 7;
          vy *= NLEVELS; vy = (int)(vy); vy/= NLEVELS;
	      rainbow(vy,&R,&G,&B);
	   }

   glColor3f(R,G,B);
}


//direction_to_color: Set the current color by mapping a direction vector (x,y), using
//                    the color mapping method 'method'. If method==1, map the vector direction
//                    using a rainbow colormap. If method==0, simply use the white color
void direction_to_color(float x, float y, int method)
{
	float r,g,b,f;
	if (method)
	{
	  f = atan2(y,x) / 3.1415927 + 1;
	  r = f;
	  if(r > 1) r = 2 - r;
	  g = f + .66667;
      if(g > 2) g -= 2;
	  if(g > 1) g = 2 - g;
	  b = f + 2 * .66667;
	  if(b > 2) b -= 2;
	  if(b > 1) b = 2 - b;
	}
	else
	{ r = g = b = 1; }
	glColor3f(r,g,b);
}

//visualize: This is the main visualization function
void visualize(void)
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

			set_colormap(rho[idx0]);	glVertex2f(px0, py0);
			set_colormap(rho[idx1]);	glVertex2f(px1, py1);
			set_colormap(rho[idx2]);	glVertex2f(px2, py2);

			set_colormap(rho[idx0]);	glVertex2f(px0, py0);
			set_colormap(rho[idx2]);	glVertex2f(px2, py2);
			set_colormap(rho[idx3]);	glVertex2f(px3, py3);
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
		  direction_to_color(vx[idx],vy[idx],color_dir);
		  glVertex2f(wn + (fftw_real)i * wn, hn + (fftw_real)j * hn);
		  glVertex2f((wn + (fftw_real)i * wn) + vec_scale * vx[idx], (hn + (fftw_real)j * hn) + vec_scale * vy[idx]);
	    }
	  glEnd();
	}
}


//------ INTERACTION CODE STARTS HERE -----------------------------------------------------------------

//display: Handle window redrawing events. Simply delegates to visualize().
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	visualize();
	glFlush();
	glutSwapBuffers();
}

//reshape: Handle window resizing (reshaping) events
void reshape(int w, int h)
{
 	glViewport(0.0f, 0.0f, (GLfloat)w, (GLfloat)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	winWidth = w; winHeight = h;
}

//keyboard: Handle key presses
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	  case 't': dt -= 0.001; break;
	  case 'T': dt += 0.001; break;
	  case 'c': color_dir = 1 - color_dir; break;
	  case 'S': vec_scale *= 1.2; break;
	  case 's': vec_scale *= 0.8; break;
	  case 'V': visc *= 5; break;
	  case 'v': visc *= 0.2; break;
	  case 'x': draw_smoke = 1 - draw_smoke;
		    if (draw_smoke==0) draw_vecs = 1; break;
	  case 'y': draw_vecs = 1 - draw_vecs;
		    if (draw_vecs==0) draw_smoke = 1; break;
	  case 'm': scalar_col++; if (scalar_col>COLOR_BANDS) scalar_col=COLOR_BLACKWHITE; break;
	  case 'a': frozen = 1-frozen; break;
	  case 'q': exit(0);
	}
}



// drag: When the user drags with the mouse, add a force that corresponds to the direction of the mouse
//       cursor movement. Also inject some new matter into the field at the mouse location.
void drag(int mx, int my)
{
	int xi,yi,X,Y; double  dx, dy, len;
	static int lmx=0,lmy=0;				//remembers last mouse location

	// Compute the array index that corresponds to the cursor location
	xi = (int)clamp((double)(DIM + 1) * ((double)mx / (double)winWidth));
	yi = (int)clamp((double)(DIM + 1) * ((double)(winHeight - my) / (double)winHeight));

	X = xi; Y = yi;

	if (X > (DIM - 1))  X = DIM - 1; if (Y > (DIM - 1))  Y = DIM - 1;
	if (X < 0) X = 0; if (Y < 0) Y = 0;

	// Add force at the cursor location
	my = winHeight - my;
	dx = mx - lmx; dy = my - lmy;
	len = sqrt(dx * dx + dy * dy);
	if (len != 0.0) {  dx *= 0.1 / len; dy *= 0.1 / len; }
	fx[Y * DIM + X] += dx;
	fy[Y * DIM + X] += dy;
	rho[Y * DIM + X] = 10.0f;
	lmx = mx; lmy = my;
}


//main: The main program
int main(int argc, char **argv)
{
	printf("Fluid Flow Simulation and Visualization\n");
	printf("=======================================\n");
	printf("Click and drag the mouse to steer the flow!\n");
	printf("T/t:   increase/decrease simulation timestep\n");
	printf("S/s:   increase/decrease hedgehog scaling\n");
	printf("c:     toggle direction coloring on/off\n");
	printf("V/v:   increase decrease fluid viscosity\n");
	printf("x:     toggle drawing matter on/off\n");
	printf("y:     toggle drawing hedgehogs on/off\n");
	printf("m:     toggle thru scalar coloring\n");
	printf("a:     toggle the animation on/off\n");
	printf("q:     quit\n\n");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("Real-time smoke simulation and visualization");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(do_one_simulation_step);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(drag);

	init_simulation(DIM);	//initialize the simulation data structures
	glutMainLoop();			//calls do_one_simulation_step, keyboard, display, drag, reshape
	return 0;
}
