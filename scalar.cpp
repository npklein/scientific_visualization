#include "scalar.h"
#include <fftw.h>
#include <math.h>


Scalar::Scalar(){}
Scalar::~Scalar(){}

fftw_real Scalar::interpolateScalar(float point_x, float point_y, int DIM, fftw_real (*scalar_function)())
{
    int i = floor(point_x);
    int j = floor(point_y);
    int idx_1 = (j * DIM) + i;
    int idx_2 = (j * DIM) + i+1;
    int idx_4 = ((j+1) * DIM) + i;
    int idx_3 = ((j+1) * DIM) + i+1;

    //fftw_real scalar_1 = (*scalar_function)()[idx_1];
    //fftw_real scalar_2 = (*scalar_function)()[idx_2];
    //fftw_real scalar_3 = (*scalar_function)()[idx_3];
    //fftw_real scalar_4 = (*scalar_function)()[idx_4];
    fftw_real scalar_1 = 0;
    fftw_real scalar_2 = 0;
    fftw_real scalar_3 = 0;
    fftw_real scalar_4 = 0;
    float tx = fabs(point_x - i);	//Parametric coordinates of point within cell
    float ty = fabs(point_y - j);

    fftw_real value=0;								//Compute scalar value at given point, using
                                            //bilinear interpolation of the scalar values at
    value += (1-tx)*(1-ty)*scalar_1;				//the four cell vertices
    value += tx*(1-ty)*scalar_2;
    value += tx*ty*scalar_3;
    value += (1-tx)*ty*scalar_4;

    return(value);
}
