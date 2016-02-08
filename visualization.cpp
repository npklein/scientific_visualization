#include "simulation.h"

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


Simulation simulation;


//------ VISUALIZATION CODE STARTS HERE -----------------------------------------------------------------
float max(float x, float y)
{ return x > y ? x : y; }

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
   R = G = B = 0;
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
