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

    float cosa = X;
    float sina = Y;

    float a;
    if (sina>=0)
        a = acos(cosa);
    else
        a = 2*M_PI - acos(cosa);

    return 180*a/M_PI;
}

Vector::~Vector(void)
{
}
