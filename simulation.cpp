#include "simulation.h"
#include <cmath>
#include "vector.h"
//Default Constructor
Simulation::Simulation() {
    // default values member variables
    dt = 0.4;				//simulation time step
    visc = 0.001;				//fluid viscosity
    frozen = false;               //toggles on/off the animation
}

//Destructor
Simulation::~Simulation(){}

//Accessor Functions
bool Simulation::get_frozen() const{return frozen;}
double Simulation::get_dt() const{return dt;}
float Simulation::get_visc() const{return visc;}
fftw_real* Simulation::get_fx() const{return fx;}
fftw_real* Simulation::get_fy() const{return fy;}
fftw_real* Simulation::get_rho() const{return rho;}
fftw_real* Simulation::get_rho0() const {return rho0;}
fftw_real* Simulation::get_vx() const {return vx;}
fftw_real* Simulation::get_vy() const {return vy;}
fftw_real Simulation::get_rho_min() const{return rho_min;}
fftw_real Simulation::get_rho_max() const{return rho_max;}
float Simulation::get_f_magnitude_min() const{return f_magnitude_min*100;}
float Simulation::get_v_magnitude_min() const {return v_magnitude_min*100;}
float Simulation::get_f_magnitude_max() const{return f_magnitude_max*100;}
float Simulation::get_v_magnitude_max() const {return v_magnitude_max*100;}
fftw_real* Simulation::get_vx0() const {return vx0;}
fftw_real* Simulation::get_vy0() const {return vy0;}

//Mutator functions
void Simulation::set_frozen(bool new_frozen){frozen = new_frozen;}
void Simulation::set_dt(double new_dt){dt = new_dt;}
void Simulation::set_visc(float new_visc){visc = new_visc;}

//init_simulation: Initialize simulation data structures as a function of the grid size 'n'.
//                 Although the simulation takes place on a 2D grid, we allocate all data structures as 1D arrays,
//                 for compatibility with the FFTW numerical library.
void Simulation::init_simulation(int n)
{
    int i; size_t dim;
    dim     = n * 2*(n/2+1)*sizeof(fftw_real);        //Allocate data structures
    vx       = (fftw_real*) malloc(dim);
    vy       = (fftw_real*) malloc(dim);
    vx0      = (fftw_real*) malloc(dim);
    vy0      = (fftw_real*) malloc(dim);
    dim     = n * n * sizeof(fftw_real);
    fx      = (fftw_real*) malloc(dim);
    fy      = (fftw_real*) malloc(dim);
    rho     = (fftw_real*) malloc(dim);
    rho0    = (fftw_real*) malloc(dim);
    plan_rc = rfftw2d_create_plan(n, n, FFTW_REAL_TO_COMPLEX, FFTW_IN_PLACE);
    plan_cr = rfftw2d_create_plan(n, n, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE);

    for (i = 0; i < n * n; i++)                      //Initialize data structures to 0
    { vx[i] = vy[i] = vx0[i] = vy0[i] = fx[i] = fy[i] = rho[i] = rho0[i] = 0.0f; }
}

//FFT: Execute the Fast Fourier Transform on the dataset 'vx'.
//     'dirfection' indicates if we do the direct (1) or inverse (-1) Fourier Transform
void Simulation::FFT(int direction,void* vx)
{
    if(direction==1) rfftwnd_one_real_to_complex(plan_rc,(fftw_real*)vx,(fftw_complex*)vx);
    else             rfftwnd_one_complex_to_real(plan_cr,(fftw_complex*)vx,(fftw_real*)vx);
}

int Simulation::clamp(float x)
{ return ((x)>=0.0?((int)(x)):(-((int)(1-(x))))); }

//solve: Solve (compute) one step of the fluid flow simulation
void Simulation::solve(int n, fftw_real* vx, fftw_real* vy, fftw_real* vx0, fftw_real* vy0, fftw_real visc, fftw_real dt)
{
    fftw_real x, y, x0, y0, f, r, U[2], V[2], s, t;
    int i, j, i0, j0, i1, j1;

    for (i=0;i<n*n;i++)
    { vx[i] += dt*vx0[i]; vx0[i] = vx[i]; vy[i] += dt*vy0[i]; vy0[i] = vy[i]; }

    for ( x=0.5f/n,i=0 ; i<n ; i++,x+=1.0f/n )
       for ( y=0.5f/n,j=0 ; j<n ; j++,y+=1.0f/n )
       {
           x0 = n*(x-dt*vx0[i+n*j])-0.5f;
           y0 = n*(y-dt*vy0[i+n*j])-0.5f;
           i0 = clamp(x0); s = x0-i0;
           i0 = (n+(i0%n))%n;
           i1 = (i0+1)%n;
           j0 = clamp(y0); t = y0-j0;
           j0 = (n+(j0%n))%n;
           j1 = (j0+1)%n;
           vx[i+n*j] = (1-s)*((1-t)*vx0[i0+n*j0]+t*vx0[i0+n*j1])+s*((1-t)*vx0[i1+n*j0]+t*vx0[i1+n*j1]);
           vy[i+n*j] = (1-s)*((1-t)*vy0[i0+n*j0]+t*vy0[i0+n*j1])+s*((1-t)*vy0[i1+n*j0]+t*vy0[i1+n*j1]);
       }

    for(i=0; i<n; i++)
    for(j=0; j<n; j++)
    {  vx0[i+(n+2)*j] = vx[i+n*j];
        vy0[i+(n+2)*j] = vy[i+n*j]; }

    FFT(1,vx0);
    FFT(1,vy0);

    for (i=0;i<=n;i+=2)
    {
        x = 0.5f*i;
        for (j=0;j<n;j++)
        {
            y = j<=n/2 ? (fftw_real)j : (fftw_real)j-n;
            r = x*x+y*y;
            if ( r==0.0f ) continue;
            f = (fftw_real)exp(-r*dt*visc);
            U[0] = vx0[i  +(n+2)*j]; V[0] = vy0[i  +(n+2)*j];
            U[1] = vx0[i+1+(n+2)*j]; V[1] = vy0[i+1+(n+2)*j];

            vx0[i  +(n+2)*j] = f*((1-x*x/r)*U[0]     -x*y/r *V[0]);
            vx0[i+1+(n+2)*j] = f*((1-x*x/r)*U[1]     -x*y/r *V[1]);
            vy0[i+  (n+2)*j] = f*(  -y*x/r *U[0] + (1-y*y/r)*V[0]);
            vy0[i+1+(n+2)*j] = f*(  -y*x/r *U[1] + (1-y*y/r)*V[1]);
        }
    }

    FFT(-1,vx0);
    FFT(-1,vy0);

    f = 1.0/(n*n);
    v_magnitude_max = 0;
    v_magnitude_min = 1;
    for (i=0;i<n;i++)
       for (j=0;j<n;j++)
       {
           vx[i+n*j] = f*vx0[i+(n+2)*j];
           vy[i+n*j] = f*vy0[i+(n+2)*j];
            float v_magnitude = Vector(vx[i+n*j], vy[i+n*j]).length();
            if (v_magnitude > v_magnitude_max){
                v_magnitude_max = v_magnitude;
            }
            if (v_magnitude < v_magnitude_min){
                v_magnitude_min = v_magnitude;
            }
       }
}


// diffuse_matter: This function diffuses matter that has been placed in the velocity field. It's almost identical to the
// velocity diffusion step in the function above. The input matter densities are in rho0 and the result is written into rho.
void Simulation::diffuse_matter(int n, fftw_real *vx, fftw_real *vy, fftw_real *rho, fftw_real *rho0, fftw_real dt)
{
    rho_min = 10;
    rho_max = 0;
    fftw_real x, y, x0, y0, s, t;
    int i, j, i0, j0, i1, j1;

    for ( x=0.5f/n,i=0 ; i<n ; i++,x+=1.0f/n )
    for ( y=0.5f/n,j=0 ; j<n ; j++,y+=1.0f/n )
    {
        x0 = n*(x-dt*vx[i+n*j])-0.5f;
        y0 = n*(y-dt*vy[i+n*j])-0.5f;
        i0 = clamp(x0);
        s = x0-i0;
        i0 = (n+(i0%n))%n;
        i1 = (i0+1)%n;
        j0 = clamp(y0);
        t = y0-j0;
        j0 = (n+(j0%n))%n;
        j1 = (j0+1)%n;
        float rho_value = (1-s)*((1-t)*rho0[i0+n*j0]+t*rho0[i0+n*j1])+s*((1-t)*rho0[i1+n*j0]+t*rho0[i1+n*j1]);
        rho[i+n*j] = rho_value;
        if ( rho_value< rho_min ) rho_min = rho_value;
        else if (rho_value > rho_max) rho_max = rho_value;

    }
}

//set_forces: copy user-controlled forces to the force vectors that are sent to the solver.
//            Also dampen forces and matter density to get a stable simulation.
void Simulation::set_forces(int DIM)
{
    int i;
    f_magnitude_max = 1;
    f_magnitude_min = 0;
    for (i = 0; i < DIM * DIM; i++)
    {
        rho0[i]  = 0.995 * rho[i];
        fx[i] *= 0.85;
        fy[i] *= 0.85;
        vx0[i]    = fx[i];
        vy0[i]    = fy[i];
        float f_magnitude = Vector(fx[i], fy[i]).length();
        if (f_magnitude > f_magnitude_max){
            f_magnitude_max = f_magnitude;
        }
        if (f_magnitude < f_magnitude_min){
            f_magnitude_min = f_magnitude;
        }
    }
}

void Simulation::drag(int mx, int my, int DIM, int winWidth, int winHeight)
{
    int xi,yi,X,Y; double  dx, dy, len;
    static int lmx=0,lmy=0;				//remembers last mouse location

    // Compute the array index that corresponds to the cursor location
    xi = (int)clamp((double)(DIM + 1) * ((double)mx / (double)winWidth));
    yi = (int)clamp((double)(DIM + 1) * ((double)(winHeight - my) / (double)winHeight));

    X = xi; Y = yi;

    if (X > (DIM - 1))  X = DIM - 1; if (Y > (DIM - 1))  Y = DIM - 1;
    if (X < 0) X = 0; if (Y < 0) Y = 0;

    // Add force at the cursor location
    my = winHeight - my;
    dx = mx - lmx;
    dy = my - lmy;
    len = sqrt(dx * dx + dy * dy);
    if (len != 0.0)
    {
        dx *= 0.1 / len;
        dy *= 0.1 / len;
    }
    fx[Y * DIM + X] += dx;
    fy[Y * DIM + X] += dy;
    rho[Y * DIM + X] = 10.0f;
    lmx = mx; lmy = my;
}
