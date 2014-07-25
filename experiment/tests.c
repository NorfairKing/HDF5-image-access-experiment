#include <stdio.h>

#include "hdf5.h"
#include "pure_c.h"
#include "with_hdf5.h"

#define LENGTH "1000"

int main() {
    double t1, t2;
    t1 = run_pure();
    t2 = run_hdf5();

    printf("Tested reading/writing values from matrix\n");
    printf("150 x " LENGTH " reads and writes\n");
    printf("Pure test took %f seconds.\n", t1);
    printf("Test with HDF5 took %f seconds\n\n", t2);
    
    run_read_n_write();
    
}

