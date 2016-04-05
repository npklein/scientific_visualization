#ifndef VECTOR_H
#define VECTOR_H
#include "simulation.h"

class Vector
{ public:
    Vector(void);
    Vector(float X, float Y);
    Vector(float x1, float y1, float x2, float y2);
    ~Vector(void);
    float length();
    Vector normalize();
    float X,Y;
    float direction2angle();
};

Vector interpolateVector(float point_x, float point_y, float cell_width, float cell_height, int DIM, Simulation simulation);
#endif // VECTOR_H
