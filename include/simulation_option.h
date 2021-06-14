// Simulation option 
#define N_DIMS                  2                               // Simulation dimensions.
#define BOX_N                   100                              // Simulation resolution.
#define BOX_L                   PI                             // Simulation box size.


// Simulation time
#define END_TIME                3.0                             // Simulation end time.


// Problem number (checkout macro.h)
#define PROB_NUM               PROB_SINWAVE
//#define PROB_NUM                PROB_TWOBODY
//#define PROB_NUM                PROB_NBODY


// Potential solver
#define BG_POTENTIAL            10.0                            // Background potential. (Notice: Don't set it to zero!)
#define POT_SOLVER              SOR                         // SOR / V_CYCLE / W_CYCLE / FAS / FMG
#define SOR_OMEGA               1.9                             // SOR weight
#define SOR_SMOOTH_STEP         1000                           // Smooth step converge lower than 10000 for N=200
#define SOR_EXACT_STEP          100000                          // Exact step will only affect on SOR solver. 
#define SOR_ERROR               1.e-10                          // Solver converge threadshold


// Parallel
#define OMP_PARALLEL                                            // Open the openMP parallel, must turn off GPU
#ifdef OMP_PARALLEL  
#define OMP_THREAD_NUM         8 
#endif
//#define GPU													// Open GPU parallel, must turn off openMP.

// Particle
#define N_PARS                  10000                               // Number of particle
//#define N_PARS                  2                               // Number of particle
#define MASS_TO_CELL            TSC                             // NGP / CIC / TSC


// Time evolve method
#define EVOLVE                  DKD                             // EULER / KDK / DKD / RK4


// Output
#define OUT_DT                  0.1                             // Output data time increment.
