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
};


#endif // VECTOR_H
