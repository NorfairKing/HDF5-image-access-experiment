#include <stdio.h>
#include "hdf5.h"
#include "hdf5_encapsulation.h"

#include <time.h>
#include <stdlib.h>

#define FILE_NAME_RNW "rnw.h5"

#define DIM0_LIM 256
#define DIM1_LIM 256
#define LENGTH 10

hid_t set_up_rnw() {
    // Set up driver to use ram instead of on-disk files.
    hid_t fapl;
    fapl = set_core();

    create_file(fapl, FILE_NAME_RNW);

    hid_t file_id;
    file_id = open_file(fapl, FILE_NAME_RNW);

    int dim0, dim1;
    for (dim0 = 10; dim0 <= DIM0_LIM; dim0 *= 2) {
        for (dim1 = 10; dim1 <= DIM1_LIM; dim1 *= 2) {
            // get name of dataset
            char name[25];
            sprintf(name, "dataset_%dx%d", dim0, dim1); // puts string into buffer

            create_dataset(file_id, dim0, dim1, name);

            /* Initialise with values */
            hid_t dataset_id;
            dataset_id = open_dataset(file_id, name);

            double matrix[dim0][dim1];
            int i, j;

            // fill matrix with some elements
            for (i = 0; i < dim0; i++) {
                for (j = 0; j < dim1; j++) {
                    matrix[i][j] = i * dim1 + j;
                }
            }
            write_matrix(dataset_id, matrix);

            close_dataset(dataset_id);
        }
    }

    /* Done initializing values */
    close_file(file_id);
    return fapl;
}


void test_rnw(hid_t fapl) {
    clock_t begin, end;
    double time;
    
    int dim0, dim1;
    int i;
    clock_t b,e;
    double file_open_time,file_close_time;
    double dataset_open_time,dataset_close_time;
    double read_time,write_time;

    begin = clock();
    
    
    for (dim0 = 10; dim0 <= DIM0_LIM; dim0 *= 2) {
        for (dim1 = 10; dim1 <= DIM1_LIM; dim1 *= 2) {
            for (i = 0; i < LENGTH; i++) {

                double b,e;
                
                b = clock();
                hid_t file_id;
                file_id = open_file(fapl, FILE_NAME_RNW);
                e = clock();
                file_open_time += (double) (e - b) / CLOCKS_PER_SEC;
                
                b = clock();
                close_file(file_id);
                e = clock();
                file_close_time += (double) (e - b) / CLOCKS_PER_SEC;
            }
        }
    }
    end = clock();
    time = (double) (end - begin) / CLOCKS_PER_SEC;

    printf("Read and write test took %f seconds.\n", time);
    printf("Opening the file took a total of %f seconds.\n", file_open_time);
}

void tear_down_rnw() {

}

void run_read_n_write() {
    hid_t fapl;
    fapl = set_up_rnw();
    
    test_rnw(fapl);
    
    tear_down_rnw();
}