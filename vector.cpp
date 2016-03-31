#include "vector.h"
#include <math.h>


// Vector.cpp


Vector::Vector(void)
{
}

Vector::Vector(float X, float Y){
    this->X = X;
    this->Y = Y;
}

Vector::Vector(float x1, float y1, float x2, float y2){
    this->X = x1-x2;
    this->Y = y1-y2;

}

// Returns the length of the vector
float Vector::length(){
    return sqrt(X * X + Y * Y);
}

// Normalizes the vector
Vector Vector::normalize(){
    Vector vector;
    float length = this->length();

    if(length != 0){
        vector.X = X/length;
        vector.Y = Y/length;
    }

    return vector;
}

float Vector::direction2angle()			//Converts a 2D vector into an orientation (angle).
{														//The angle is in the [0,360] degrees interval

    double result;
    result = atan2 (X,-Y) * (180 / M_PI);
    return result;

}

void Vector::interpolate(Vector v1, Vector v2, Vector v3, Vector v4, float start_x, float start_y, float vertex_x, float vertex_y, float cell_size){
    float tx = fabs(start_x - vertex_x)/cell_size;	//Parametric coordinates of point within cell
    float ty = fabs(start_y - vertex_y)/cell_size;

            //Compute vector value at given point, using
                                            //bilinear interpolation of the vector values at
    this->X += (1-tx)*(1-ty)*v1.X;		//the four cell vertices
    this->Y += (1-tx)*(1-ty)*v1.Y;

    this->X += tx*(1-ty)*v2.X;
    this->Y += tx*(1-ty)*v2.Y;

    this->X += tx*ty*v3.X;
    this->Y += tx*ty*v3.Y;

    this->X += (1-tx)*ty*v4.X;
    this->Y += (1-tx)*ty*v4.Y;
}

Vector::~Vector(void)
{
}
