#ifndef SCALAR_H
#define SCALAR_H

#include <fftw.h>
class Scalar
{
public:
    Scalar();
    ~Scalar();
    fftw_real interpolateScalar(float point_x, float point_y, int DIM, fftw_real (*scalar_function)(void));

    fftw_real scalar_1;
    fftw_real scalar_2;
    fftw_real scalar_3;
    fftw_real scalar_4;
};

#endif // SCALAR_H
