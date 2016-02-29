#include "simulation.h"

//--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
const int COLOR_BLACKWHITE=0;   //different types of color mapping: black-and-white, rainbow, banded
const int COLOR_RAINBOW=1;
const int COLOR_HEATMAP=2;

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

   *R = max(0.0f, (3-(float)fabs(value-4)-(float)fabs(value-5))/2);
   *G = max(0.0f, (4-(float)fabs(value-2)-(float)fabs(value-4))/2);
   *B = max(0.0f, (3-(float)fabs(value-1)-(float)fabs(value-2))/2);
}

//heatmap: see 2nd edition page 154-155
void heatmap(float value,float* R,float* G,float* B)
{
    if (value<0) value=0; if (value>1) value=1;
    //all low start, red high midle, green high end
    //https://graphsketch.com/?eqn1_color=1&eqn1_eqn=-%28x-1%29^2%2B1&eqn2_color=2&eqn2_eqn=-%28x-1.5%29^2%2B1&eqn3_color=3&eqn3_eqn=&eqn4_color=4&eqn4_eqn=&eqn5_color=5&eqn5_eqn=&eqn6_color=6&eqn6_eqn=&x_min=-10&x_max=10&y_min=-10&y_max=10&x_tick=1&y_tick=1&x_label_freq=5&y_label_freq=5&do_grid=0&do_grid=1&bold_labeled_lines=0&bold_labeled_lines=1&line_width=4&image_w=850&image_h=525
    *R = max(0.0f, -((value-0.9)*(value-0.9))+1);
    *G = max(0.0f, -((value-1.5)*(value-1.5))+1);// max(0.0f, -(value-1)*-(value-1)+1);
    *B = 0;
}

//set_colormap: Sets three different types of colormaps
void set_colormap(float vy, int scalar_col, float color_clamp_min, float color_clamp_max, int color_bands, bool scale)
{
   float R,G,B;
   R = G = B = 0;
   // clamp vy to the min and max value
   if (vy < color_clamp_min){
       vy = color_clamp_min;
   }
   if (vy > color_clamp_max){
       vy = color_clamp_max;
   }

   vy *= color_bands; vy = (int)(vy); vy/= color_bands;

   if (scalar_col==COLOR_BLACKWHITE)
   {
       R = G = B = vy;
   }
   else if (scalar_col==COLOR_RAINBOW)
   {
       rainbow(vy,&R,&G,&B);
   }
   else if (scalar_col==COLOR_HEATMAP)
   {
       heatmap(vy, &R, &G, &B);
   }
   glColor3f(R,G,B);
}


//direction_to_color: Set the current color by mapping a direction vector (x,y), using
//                    the color mapping method 'method'. If method==1, map the vector direction
//                    using a rainbow colormap. If method==0, simply use the white color
void direction_to_color(float x, float y, int method, int color_bands)
{
	float r,g,b,f;
    if (method)
    {
	  f = atan2(y,x) / 3.1415927 + 1;
      // mehtod acts same way as scalar_col in density
      set_colormap(f, method, 0, 1,color_bands, false);
	}
	else
	{ r = g = b = 1; }
}
