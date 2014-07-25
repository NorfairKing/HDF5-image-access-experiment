#include <stdio.h>

#include "hdf5.h"
#include "pure_c.h"
#include "with_hdf5.h"

#define LENGTH "100"
#define DIM0 "100"
#define DIM1 "100"

int main() {
    
    double t1, t2;
    t1 = run_pure();
    t2 = run_hdf5();

    printf("Part one:\n");
    printf("Executed an best- and worst-case read and write test:\n");
    printf(DIM0 " x " DIM1 " x " LENGTH " double reads and writes\n");
    printf("Best-case test took %f seconds.\n", t1);
    printf("Worst-case test took %f seconds.\n\n", t2);
            
    printf("Part two:\n");
    run_read_n_write();
    
}

