#ifndef VECTOR_H
#define VECTOR_H


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
    void interpolate(Vector v1, Vector v2, Vector v3, Vector v4, float start_x, float start_y, float vertex_x, float vertex_y, float cell_size);
};


#endif // VECTOR_H
