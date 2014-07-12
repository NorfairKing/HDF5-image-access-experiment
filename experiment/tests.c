#include <stdio.h>

#include "hdf5.h"
#include "pure_c.h"
#include "with_hdf5.h"

int main(){
    double t1,t2 ;
    t1 = run_pure();
    t2 = run_hdf5();

    printf("Pure test took %f seconds.\n", t1);
    printf("Test with HDF5 took %f seconds\n", t2);
}

