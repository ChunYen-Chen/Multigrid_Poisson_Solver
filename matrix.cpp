#include "classes.h"

matrix::matrix( int n, double hi )
{
    dim = n;
    value = new double*[n];    
    h = hi;
    for ( int i = 0; i < n; i++ )
    {
        value[i] = new double[n];
        for ( int j = 0; j < n; j++ )
        {
            value[i][j] = 0;
        }
    } // for ( int i = 0; i < n; i++ )

} // CONSTRUCTER : matrix::matrix( int n, double hi )



matrix::~matrix()
{
} // DESTRUCTURE : matrix::~matrix()



double absolute( double x )
{
    if ( x >= 0 )   return x;
    return -x;
} // FUNCTION : absolute()



void matrix::display()
{
    for( int i = 0; i < dim; i++ )
    {
        for( int j = 0; j < dim; j++ )
        {
            cout << value[i][j] << " ";
        }
        cout << endl;
    }
} // FUNCTION : matrix::display()



void matrix::Error( const matrix &b )
{
    double sum = 0;
    double ave = 0;
    
    for( int i = 0; i < dim; i++ )
    {
        for( int j = 0; j < dim; j++ )
        {
            sum += absolute(this->value[i][j]-b.value[i][j]);
            ave += absolute(b.value[i][j]);
        }
        
    } // for( int i = 0; i < dim; i++ )

    cout << sum/ave << endl;

} //FUNCTION : matrix::Error



void matrix::SOR_smoothing( const matrix &rho, int steps )
{
     double err1, err2, residual;
     for ( int t = 0; t < steps; t++ )
     {
     err1 = 0.0;
     err2 = 0.0;
#    ifdef OMP_PARALLEL
#    pragma omp parallel num_threads(2) 
     {
         const int tid = omp_get_thread_num();

#        pragma omp for collapse(2)
#    endif // #ifdef OMP_PARALLEL
         for ( int i = 0; i < dim; i++ ) 
         {
             for ( int j = 0; j < dim; j++ )
             {
                 if ( (i + j) % 2 != 0 )    continue;
                 if ( i == 0 || i == dim-1 || j == 0 || j == dim-1 )    continue; // if ( i != 0 && i != dim - 1 && j != 0 && j != dim - 1 )

                 this->value[i][j] += SOR_OMEGA * 0.25 * ( this->value[ i+1 ][ j  ] + this->value[ i-1 ][ j   ] + 
                                                           this->value[ i   ][ j+1] + this->value[ i   ][ j-1 ] - 
                                                           this->value[ i   ][ j  ] * 4 - h * h * rho.value[i][j]);
                 
                 if ( t%1000 == 0 )
                 {
                     residual = this->value[ i+1 ][ j   ] + this->value[ i-1 ][ j ] + this->value[ i ][ j+1 ] + 
                                this->value[ i   ][ j-1 ] - this->value[ i   ][ j ] * 4 - h * h * rho.value[i][j];
                     err1 += fabs( residual / this->value[i][j]);
                 } // if ( t%1000 == 0 )
             } // for ( int j = 0; j < dim; j++ )
         } // for ( int i = 0; i < dim; i++ ) 

#        ifdef OMP_PARALLEL
#        pragma omp barrier
#        pragma omp for collapse(2)
#        endif // #ifdef OMP_PARALLEL
         for ( int i = 0; i < dim; i++ )
         {
             for ( int j = 0; j < dim; j++ )
             {
                 if ( (i + j) % 2 != 1 )     continue;
                 if ( i == 0 || i == dim-1 || j == 0 || j == dim-1 )    continue; // if ( i != 0 && i != dim - 1 && j != 0 && j != dim - 1 )
                
                 this->value[i][j] += SOR_OMEGA * 0.25 * ( this->value[ i+1 ][ j   ] + this->value[ i-1 ][ j   ] + 
                                                           this->value[ i   ][ j+1 ] + this->value[ i   ][ j-1 ] - 
                                                           this->value[ i   ][ j   ] * 4 - h * h * rho.value[i][j]);
                 
                 if ( t%1000 == 0 )
                 {
                     residual = this->value[ i+1 ][ j   ] + this->value[ i-1 ][ j ] + this->value[ i ][ j+1 ] + 
                                this->value[ i   ][ j-1 ] - this->value[ i   ][ j ] * 4 - h * h * rho.value[i][j];
                     err2 += fabs( residual / this->value[i][j]);
                 } // if ( t%1000 == 0 )
             } // for ( int j = 0; j < dim; j++ )
         } // for ( int i = 0; i < dim; i++ ) 

#    ifdef OMP_PARALLEL
     } // # pragma omp parallel
#    endif // #ifdef OMP_PARALLEL

         if ( t%1000 == 0 && (err1+err2) <= SOR_ERROR )    break;
     
     } //for ( int t = 0; t < steps; t++ )

} // FUNCTION : matrix::SOR_smoothing



void matrix::SOR_Exact( const matrix &rho, int steps )
{
     double err1, err2, residual;
     for ( int t = 0; t < steps; t++ )
     {
         err1 = 0.0;
         err2 = 0.0;
#    ifdef OMP_PARALLEL
#    pragma omp parallel num_threads(2) 
     {
         const int tid = omp_get_thread_num();

#        pragma omp for collapse(2)
#    endif // #ifdef OMP_PARALLEL
         for ( int i = 0; i < dim; i++ ) 
         {
             for ( int j = 0; j < dim; j++ )
             {
                 if ( (i + j) % 2 != 0 )    continue;
                 if ( i == 0 || i == dim-1 || j == 0 || j == dim-1 )    continue; // if ( i != 0 && i != dim - 1 && j != 0 && j != dim - 1 )
                 residual = this->value[ i+1 ][ j   ] + this->value[ i-1 ][ j ] + this->value[ i ][ j+1 ] + 
                            this->value[ i   ][ j-1 ] - this->value[ i   ][ j ] * 4 - h * h * rho.value[i][j];

                 this->value[i][j] += SOR_OMEGA * 0.25 * residual; 
                 
                 if ( t%1000 == 0 )    err1 += fabs( residual / this->value[i][j]);
             } // for ( int j = 0; j < dim; j++ )
         } // for ( int i = 0; i < dim; i++ ) 

#        ifdef OMP_PARALLEL
#        pragma omp barrier
#        pragma omp for collapse(2)
#        endif // #ifdef OMP_PARALLEL
         for ( int i = 0; i < dim; i++ ) 
         {
             for ( int j = 0; j < dim; j++ )
             {
                 if ( (i + j) % 2 != 1 )     continue;
                 if ( i == 0 || i == dim-1 || j == 0 || j == dim-1 )    continue; // if ( i != 0 && i != dim - 1 && j != 0 && j != dim - 1 )
                 residual = this->value[ i+1 ][ j   ] + this->value[ i-1 ][ j ] + this->value[ i ][ j+1 ] + 
                            this->value[ i   ][ j-1 ] - this->value[ i   ][ j ] * 4 - h * h * rho.value[i][j];

                 this->value[i][j] += SOR_OMEGA * 0.25 * residual; 
                 
                 if ( t%1000 == 0 )    err2 += fabs( residual / this->value[i][j]);
             } // for ( int j = 0; j < dim; j++ )
         } // for ( int i = 0; i < dim; i++ ) 

#    ifdef OMP_PARALLEL
     } // # pragma omp parallel
#    endif // #ifdef OMP_PARALLEL
         
         if ( t%1000 == 0 && (err1+err2) <= SOR_ERROR )    break;
     } //for ( int t = 0; t < steps; t++ )

} // FUNCTION : matrix::SOR_Exact



double matrix::averaging( int i, int j )
{
    if ( dim%2 == 1 )
    {
        if      ( i == 0     && j == 0     ) return ( 4*value[i][j] + 2*(value[i+1][j]+value[i][j+1]) + 1*value[i+1][j+1] ) / 9.0;
        else if ( i == 0     && j == dim-1 ) return ( 4*value[i][j] + 2*(value[i+1][j]+value[i][j-1]) + 1*value[i+1][j-1] ) / 9.0;
        else if ( i == dim-1 && j == 0     ) return ( 4*value[i][j] + 2*(value[i-1][j]+value[i][j+1]) + 1*value[i-1][j+1] ) / 9.0;
        else if ( i == dim-1 && j == dim-1 ) return ( 4*value[i][j] + 2*(value[i-1][j]+value[i][j-1]) + 1*value[i-1][j-1] ) / 9.0;

        else if ( i == 0     ) return ( 4*value[i][j] + 2*(value[i+1][j]+value[i][j+1]+value[i][j-1]) + 1*(value[i+1][j+1]+value[i+1][j-1]) ) / 12.0;
        else if ( i == dim-1 ) return ( 4*value[i][j] + 2*(value[i-1][j]+value[i][j+1]+value[i][j-1]) + 1*(value[i-1][j+1]+value[i-1][j-1]) ) / 12.0;
        else if ( j == 0     ) return ( 4*value[i][j] + 2*(value[i+1][j]+value[i][j+1]+value[i-1][j]) + 1*(value[i+1][j+1]+value[i-1][j+1]) ) / 12.0;
        else if ( j == dim-1 ) return ( 4*value[i][j] + 2*(value[i+1][j]+value[i][j-1]+value[i-1][j]) + 1*(value[i+1][j-1]+value[i-1][j-1]) ) / 12.0;

        else                   return ( 4*value[i][j] + 
                                        2*(value[i+1][j]+value[i-1][j]+value[i][j+1]+value[i][j-1]) + 
                                        1*(value[i+1][j+1]+value[i-1][j+1]+value[i+1][j-1]+value[i-1][j-1]) ) / 16.0;
    }
    else // if ( dim%2 == 1 )
    {
        if      ( i == 0     && j == 0     ) return (4*value[i][j] + 2*(value[i+1][j]+value[i][j+1]) + 1*value[i+1][j+1] ) / 9.0;
        else if ( i == 0     && j == dim-2 ) return (4*value[i][j] + 2*(value[i+1][j]+value[i][j-1]) + 1*value[i+1][j-1] ) / 9.0;
        else if ( i == dim-2 && j == 0     ) return (4*value[i][j] + 2*(value[i-1][j]+value[i][j+1]) + 1*value[i-1][j+1] ) / 9.0;
        else if ( i == dim-2 && j == dim-2 ) return (4*value[i][j] + 2*(value[i-1][j]+value[i][j-1]) + 1*value[i-1][j-1] ) / 9.0;

        else if ( i == 0     ) return ( 4*value[i][j] + 2*(value[i+1][j]+value[i][j+1]+value[i][j-1]) + 1*(value[i+1][j+1]+value[i+1][j-1]) ) / 12.0;
        else if ( i == dim-2 ) return ( 4*value[i][j] + 2*(value[i-1][j]+value[i][j+1]+value[i][j-1]) + 1*(value[i-1][j+1]+value[i-1][j-1]) ) / 12.0;
        else if ( j == 0     ) return ( 4*value[i][j] + 2*(value[i+1][j]+value[i][j+1]+value[i-1][j]) + 1*(value[i+1][j+1]+value[i-1][j+1]) ) / 12.0;
        else if ( j == dim-2 ) return ( 4*value[i][j] + 2*(value[i+1][j]+value[i][j-1]+value[i-1][j]) + 1*(value[i+1][j-1]+value[i-1][j-1]) ) / 12.0;

        else                   return ( 4*value[i][j] + 
                                        2*(value[i+1][j]+value[i-1][j]+value[i][j+1]+value[i][j-1]) + 
                                        1*(value[i+1][j+1]+value[i-1][j+1]+value[i+1][j-1]+value[i-1][j-1]) ) / 16.0;
    } // if ( dim%2 == 1 ) ... else ...

} // FUNCTION : matrix::averaging



matrix matrix::Restriction(){
    int dim2;
    if  ( dim%2 == 0 )  dim2 = dim / 2;
    else                dim2 = (dim+1) / 2;
    
    matrix r2h( dim2, h/2.0 );

    for( int i = 0; i < dim; i++ )
    {
        for( int j = 0; j < dim; j++ )
        {
            if( i%2 == 0 && j%2 == 0 )   r2h.value[i/2][j/2] = this->averaging( i, j );
        } // for( int j = 0; j < dim; j++ )
    } // for( int i = 0; i < dim; i++ )

    return r2h;

} // FUNCTION : matrix::Restriction



double matrix::insertion( int i, int j, int dim_in )
{
    int dim_new = dim_in;
    
    if ( i == dim_new-1 && j == dim_new-1 )
    {
        if     ( i%2 == 0 && j%2 == 0 ) return   value[i/2][j/2];
        else if( i%2 == 1 && j%2 == 1 ) return ( value[(i-1)/2][(j-1)/2] );
    }
    else if ( i == dim_new-1 )
    {
        if     ( i%2 == 0 && j%2 == 0 ) return   value[i/2][j/2];
        else if( i%2 == 1 && j%2 == 0 ) return ( value[(i-1)/2][j/2] );
        else if( i%2 == 0 && j%2 == 1 ) return ( value[i/2][(j+1)/2] + value[i/2][(j-1)/2] ) / 2.0;
        else if( i%2 == 1 && j%2 == 1 ) return ( value[(i-1)/2][(j+1)/2] + value[(i-1)/2][(j-1)/2] ) / 2.0;
    }
    else if ( j == dim_new-1 )
    {
        if     ( i%2 == 0 && j%2 == 0 ) return   value[i/2][j/2];
        else if( i%2 == 1 && j%2 == 0 ) return ( value[(i+1)/2][j/2] + value[(i-1)/2][j/2] ) / 2.0;
        else if( i%2 == 0 && j%2 == 1 ) return ( value[i/2][(j-1)/2] );
        else if( i%2 == 1 && j%2 == 1 ) return ( value[(i+1)/2][(j-1)/2] + value[(i-1)/2][(j-1)/2] ) / 2.0;
    }

    if      ( i%2 == 0 && j%2 == 0 ) return   value[i/2][j/2];
    else if ( i%2 == 1 && j%2 == 0 ) return ( value[(i+1)/2][j/2] + value[(i-1)/2][j/2] ) / 2.0;
    else if ( i%2 == 0 && j%2 == 1 ) return ( value[i/2][(j+1)/2] + value[i/2][(j-1)/2] ) / 2.0;
    else if ( i%2 == 1 && j%2 == 1 ) return ( value[(i+1)/2][(j+1)/2] + value[(i+1)/2][(j-1)/2] + 
                                              value[(i-1)/2][(j+1)/2] + value[(i-1)/2][(j-1)/2] ) / 4.0;
} // FUNCTION : matrix::insertion



matrix matrix::Interpolation( int dim_in )
{
    int dim_new = dim_in;
      
    matrix r (dim_new, h/2.0 );
  
    for( int i = 0; i < dim_new; i++ )
    {
        for( int j = 0; j < dim_new; j++ )
        {
            r.value[i][j] = this->insertion( i, j, dim_new );
        } // for( int j = 0; j < dim_new; j++ )
    } // for( int i = 0; i < dim_new; i++ )
    
    return r;

} // FUNCTION : matrix::Interpolation



matrix matrix::Residual( const matrix & rho )
{
    matrix res( this->dim, this->h );
    for( int i = 0; i < dim; i++ )
    {
        for( int j = 0; j < dim; j++ )
        {
            if ( i != 0 && i != dim-1 && j != 0 && j != dim-1 )
            {
                res.value[i][j] = 0.25 * ( this->value[i+1][j  ]   + this->value[i-1][j  ] + 
                                           this->value[i  ][j+1]   + this->value[i  ][j-1] - 
                                           this->value[i  ][j  ]*4 - h*h*rho.value[i][j] ) / h / h;
            }
        } // for( int j = 0; j < dim; j++ )
    } // for( int i = 0; i < dim; i++ )

    return res;

} // FUNCTION : matrix::Residual



matrix matrix::Laplacian()
{
    matrix lap( this->dim, this->h );
    for( int i = 0; i < dim; i++ )
    {
        for( int j = 0; j < dim; j++ )
        {
            if ( i != 0 && i != dim-1 && j != 0 && j != dim-1 )
            {
                lap.value[i][j] = 0.25 * ( this->value[i+1][j  ]   + this->value[i-1][j  ] + 
                                           this->value[i  ][j+1]   + this->value[i  ][j-1] - 
                                           this->value[i  ][j  ]*4  ) / h / h;
            }
        } // for( int j = 0; j < dim; j++ )
    } // for( int i = 0; i < dim; i++ )

    return lap;

} // FUNCTION : matrix::Laplacian



void matrix::init_density()
{
    for( int i = 0; i < dim; i++ )
    {
        for( int j = 0; j < dim; j++ )
        {
            double x = h*i;
            double y = h*j;

            this->value[i][j] = -2.*sin(x)*sin(y);
        } // for( int j = 0; j < dim; j++ )
    } // for( int i = 0; i < dim; i++ )

} //FUNCTION : matrix::init_density



void matrix::init_potential()
{
    for( int i = 0; i < dim; i++ )
    {
        for( int j = 0; j < dim; j++ )
        {
            this->value[i][j] = BG_POTENTIAL;
        } // for( int j = 0; j < dim; j++ )
    } // for( int i = 0; i < dim; i++ )

} //FUNCTION : matrix::init_density



void matrix::reset()
{
    for( int i = 0; i < dim; i++ )    for( int j = 0; j < dim; j++ )    this->value[i][j] = 0.0;
    
} // FUNCTION : matrix::reset_particle_density



double matrix::get_h()
{
    return this->h;
} // FUNCTION : matrix::get_h



double matrix::get_dim()
{
    return this->dim;
} // FUNCTION : matrix::get_dim



double matrix::get_value( int i, int j )
{
    return this->value[i][j];
} // FUNCTION : matrix::get_value



void matrix::set_value( int i, int j, double val )
{
    this->value[i][j] = val;
} // FUNCTION : matrix::set_value



void matrix::add_value( int i, int j, double val )
{
    this->value[i][j] += val;
} // FUNCTION : matrix::add_value



void matrix::input_answer( int i, int j, double ans )
{
    this->value[i][j] = ans;
} // FUNCTION : matrix::input_answer
  


matrix matrix::operator+( const matrix &b )
{
    matrix tmp( dim, h );

    for( int i = 0; i < dim; i++ )
    {
        for( int j = 0; j < dim; j++ )
        {
            tmp.value[i][j] = this->value[i][j] + b.value[i][j];
        }
    }

    return tmp;

} // FUNCTION : matrix::operator+



matrix matrix::operator-( const matrix &b )
{
    matrix tmp( dim, h );

    for( int i = 0; i < dim; i++ )
    {
        for( int j = 0; j < dim; j++ )
        {
            tmp.value[i][j] = this->value[i][j] - b.value[i][j];
        }
    }

    return tmp;

} // FUNCTION : matrix::operator-



