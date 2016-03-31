#include "grid.h"


Grid::Grid(int n)
{
    dim = n * 2*(n/2+1)*sizeof(fftw_real);        //Allocate data structures
    vx = (fftw_real*) malloc(dim);
    vy = (fftw_real*) malloc(dim);
}

void Grid::addElementToGrid(fftw_real vx_element, fftw_real vy_element, int index){
    vx[index] = vx_element;
    vy[index] = vx_element;
}

//Destructor
Grid::~Grid(){}
