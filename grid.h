#ifndef GRID_H
#define GRID_H

#include <rfftw.h>              //the numerical simulation FFTW library
class Grid
{
public:
    Grid();

    Grid(int);
    //Destructor (destroy out of memory after being done)
    ~Grid();

    void addElementToGrid(fftw_real x, fftw_real y, int index);

    size_t dim;
    fftw_real *vx, *vy;

};

#endif // GRID_H
