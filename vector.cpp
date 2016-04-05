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

Vector interpolateVector(float point_x, float point_y, float cell_width, float cell_height, int DIM, Simulation simulation){
    int i = floor(point_x);
    int j = floor(point_y);
    int idx_1 = (j * DIM) + i;
    int idx_2 = (j * DIM) + i+1;
    int idx_4 = ((j+1) * DIM) + i;
    int idx_3 = ((j+1) * DIM) + i+1;

    Vector vector1 = Vector((fftw_real)i * cell_width, //x1
                            (fftw_real)j * cell_height, //y1
                            ((fftw_real)i * cell_width) + simulation.get_vx()[idx_1], //x2
                            ((fftw_real)j * cell_height) + simulation.get_vy()[idx_1]);//y2
    Vector vector2 = Vector((fftw_real)i+1 * cell_width, //x1
                            (fftw_real)j * cell_height, //y1
                            ((fftw_real)i+1 * cell_width) + simulation.get_vx()[idx_2], //x2
                            ((fftw_real)j * cell_height) + simulation.get_vy()[idx_2]);//y2
    Vector vector4 = Vector((fftw_real)i * cell_width, //x1
                            (fftw_real)j+1 * cell_height, //y1
                            ((fftw_real)i * cell_width) + simulation.get_vx()[idx_3], //x2
                            ((fftw_real)j+1 * cell_height) + simulation.get_vy()[idx_3]);//y2
    Vector vector3 = Vector((fftw_real)i+1 * cell_width, //x1
                            (fftw_real)j+1 * cell_height, //y1
                            ((fftw_real)i+1 * cell_width) + simulation.get_vx()[idx_4], //x2
                            ((fftw_real)j+1 * cell_height) + simulation.get_vy()[idx_4]);//y2
    Vector interpolated_vector = Vector(0,0);

    float tx = fabs(point_x - i);	//Parametric coordinates of point within cell
    float ty = fabs(point_y - j);

    //Compute vector value at given point, using
    //bilinear interpolation of the vector values at
    interpolated_vector.X += (1-tx)*(1-ty)*vector1.X;		//the four cell vertices
    interpolated_vector.Y += (1-tx)*(1-ty)*vector1.Y;

    interpolated_vector.X += tx*(1-ty)*vector2.X;
    interpolated_vector.Y += tx*(1-ty)*vector2.Y;

    interpolated_vector.X += tx*ty*vector3.X;
    interpolated_vector.Y += tx*ty*vector3.Y;

    interpolated_vector.X += (1-tx)*ty*vector4.X;
    interpolated_vector.Y += (1-tx)*ty*vector4.Y;
    return(interpolated_vector);
}

Vector::~Vector(void)
{
}
