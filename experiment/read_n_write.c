#include <stdio.h>
#include "hdf5.h"
#include "hdf5_encapsulation.h"

#include <time.h>
#include <stdlib.h>

#define FILE_NAME_RNW "rnw.h5"

#define DIM0_START 10
#define DIM1_START 10
#define DIM0_LIM 320
#define DIM1_LIM 320
#define LENGTH 10
#define READS_AND_WRITES 1000

#define NANOSECONDS_IN_A_SECOND 1000000

void read_mat(hid_t dataset_id, size_t dim0, size_t dim1, double matrix[dim0][dim1]) {
    // Read the entire matrix
    H5Dread(dataset_id
            , H5T_NATIVE_DOUBLE
            , H5S_ALL
            , H5S_ALL
            , H5P_DEFAULT
            , matrix
            );
}

void write_mat(hid_t dataset_id, size_t dim0, size_t dim1, double matrix[dim0][dim1]) {
    // Write to dataset
    H5Dwrite(dataset_id // Dataset id
            , H5T_NATIVE_DOUBLE // data type
            , H5S_ALL // Memory dataspace and selection therein
            , H5S_ALL // selection within the file dataset's dataspace
            , H5P_DEFAULT // Transfer Property List identifier
            , matrix // pointer to data to be written
            );
}

hid_t set_up_rnw() {
    // Set up driver to use ram instead of on-disk files.
    hid_t fapl;
    fapl = set_core();

    create_file(fapl, FILE_NAME_RNW);
    hid_t file_id;
    file_id = open_file(fapl, FILE_NAME_RNW);

    size_t dim0, dim1;
    for (dim0 = DIM0_START; dim0 <= DIM0_LIM; dim0 *= 2) {
        for (dim1 = DIM1_START; dim1 <= DIM1_LIM; dim1 *= 2) {
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
    size_t dim0, dim1;

    int i, j, k;
    int x, y;
    clock_t b, e;
    double file_open_time, file_close_time;
    double dataset_open_time, dataset_close_time;
    double read_time, write_time;

    file_open_time = 0;
    file_close_time = 0;
    dataset_open_time = 0;
    dataset_close_time = 0;
    read_time = 0;
    write_time = 0;

    for (dim0 = DIM0_START; dim0 <= DIM0_LIM; dim0 *= 2) {
        for (dim1 = DIM1_START; dim1 <= DIM1_LIM; dim1 *= 2) {
            for (i = 0; i < LENGTH; i++) {

                // Open file
                b = clock();
                hid_t file_id;
                file_id = open_file(fapl, FILE_NAME_RNW);
                e = clock();
                file_open_time += (double) (e - b) / CLOCKS_PER_SEC;


                // get name of dataset
                char name[25];
                sprintf(name, "dataset_%dx%d", dim0, dim1); // puts string into buffer


                // Open dataset 
                b = clock();
                hid_t dataset_id;
                dataset_id = open_dataset(file_id, name);
                e = clock();
                dataset_open_time += (double) (e - b) / CLOCKS_PER_SEC;


                // READS AND WRITES
                double matrix[dim0][dim1];
                for (k = 0; k < READS_AND_WRITES; k++) {

                    b = clock();
                    read_mat(dataset_id, dim0, dim1, matrix);
                    e = clock();
                    read_time += (double) (e - b) / CLOCKS_PER_SEC;


                    // Modify the data at least a bit
                    // just to eliminate any shortcuts.
                    /*
                    for (x = 0; x < dim0; x++) {
                        for (y = 0; y < dim1; y++) {
                            matrix[x][y] += 1;
                        }

                    }
                     */
                    // NOTE: there don't seem to be any shortcuts

                    b = clock();
                    write_mat(dataset_id, dim0, dim1, matrix);
                    e = clock();
                    write_time += (double) (e - b) / CLOCKS_PER_SEC;
                }



                // Close dataset
                b = clock();
                close_dataset(dataset_id);
                e = clock();
                dataset_close_time += (double) (e - b) / CLOCKS_PER_SEC;


                // Close file
                b = clock();
                close_file(file_id);
                e = clock();
                file_close_time += (double) (e - b) / CLOCKS_PER_SEC;
            }
        }
    }

    printf("Executed an average-case read and write test with the following properties:\n");
    printf("The experiment uses file: " FILE_NAME_RNW "\n");
    printf("The experiment repeats %d times.\n", LENGTH);
    printf("Dim0 starting from %d and doubling until %d.\n", DIM0_START, DIM0_LIM);
    printf("Dim1 starting from %d and doubling until %d.\n", DIM1_START, DIM1_LIM);
    printf("Reading and writing a matrix %d times.\n", READS_AND_WRITES);
    double total_reads_and_writes;
    total_reads_and_writes = LENGTH * READS_AND_WRITES * (DIM0_LIM - DIM0_START) * (DIM1_LIM - DIM1_START);
    printf("This means a total of %.0lf doubles have been read and written.\n", total_reads_and_writes);
    printf("\n");

    printf("Opening the file took a total of %f seconds.\n", file_open_time);
    printf("Closing the file took a total of %f seconds.\n", file_close_time);
    printf("Opening the dataset took a total of %f seconds.\n", dataset_open_time);
    printf("Closing the dataset took a total of %f seconds.\n", dataset_close_time);
    printf("Reading the data took a total of %f seconds.\n", read_time);
    printf("Writing the data took a total of %f seconds.\n", write_time);

    printf("This means that reading produces an average overhead of %f ns per double.\n", read_time / total_reads_and_writes * NANOSECONDS_IN_A_SECOND);
    printf("This means that writing produces an average overhead of %f ns per double.\n\n", write_time / total_reads_and_writes * NANOSECONDS_IN_A_SECOND);
}

void tear_down_rnw() {

}

void run_read_n_write() {
    hid_t fapl;
    fapl = set_up_rnw();

    test_rnw(fapl);

    tear_down_rnw();
}
