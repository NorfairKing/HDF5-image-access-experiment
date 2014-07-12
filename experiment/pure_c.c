#include "hdf5.h"
#include "hdf5_encapsulation.h"

#include <time.h>
#include <stdlib.h>

#define FILE_NAME_PURE "pure_c.h5"
#define DATASET_NAME_PURE "/pure"

#define DIM0 10
#define DIM1 15

#define LENGTH 1000



void read_matrix(hid_t dataset_id, double matrix[DIM0][DIM1]){
    
    herr_t status;
    printf("Reading from dataset.\n");
    status = H5Dread(dataset_id
                     , H5T_NATIVE_DOUBLE
                     , H5S_ALL
                     , H5S_ALL
                     , H5P_DEFAULT
                     , matrix 
        );
    if (status < 0){
        printf("Failed to read dataset.\n");
        exit(1);
    }
    printf("Read from dataset.\n");

}

void write_matrix(hid_t dataset_id, double matrix[DIM0][DIM1]){
    herr_t status;
    printf("Writing to dataset.\n");
    status = H5Dwrite(dataset_id // Dataset id
                      , H5T_NATIVE_DOUBLE // data type
                      , H5S_ALL // Memory dataspace and selection therein
                      , H5S_ALL // selection within the file dataset's dataspace
                      , H5P_DEFAULT // Transfer Property List identifier
                      , matrix // pointer to data to be written
        );
    if (status < 0){
        printf("Failed to read from dataset.\n");
        exit(1);
    }
    printf("Wrote to dataset.\n");
}

hid_t set_up_pure(){
    // Set up driver to use ram instead of on-disk files.
    hid_t fapl;
    fapl = set_core();

    //create_file(H5P_DEFAULT, FILE_NAME_PURE);
    create_file(fapl, FILE_NAME_PURE);


    hid_t file_id;
    file_id = open_file(fapl, FILE_NAME_PURE);

    create_dataset(file_id, DIM0, DIM1, DATASET_NAME_PURE);

    /* Initialise with values */
    hid_t dataset_id;
    dataset_id = open_dataset(file_id, DATASET_NAME_PURE);

    double matrix[DIM0][DIM1];
    int i, j;

    for (i = 0; i < DIM0; i++){
        for (j = 0; j < DIM1; j++){
            matrix[i][j] = i*DIM1 + j;
        }
    }
    write_matrix(dataset_id, matrix);

    close_dataset(dataset_id);
    /* Done initializing values */
    
    return file_id;
}

double test_pure(hid_t file_id){
    hid_t dataset_id;
    dataset_id = open_dataset(file_id, DATASET_NAME_PURE);

    double matrix[DIM0][DIM1];
    read_matrix(dataset_id,matrix);

    clock_t begin, end;
    double time;

    begin = clock();

    int i,j,k;
    for (k = 0; k < LENGTH; k++){
        for (i = 0; i < DIM0; i++){
            for (j = 0; j < DIM1; j++){
                if (i < 1 || j < 1){
                    matrix[i][j] = 1;
                }
                else{
                    //Random computation that includes reading and writing
                    matrix[i][j] = (matrix[i-1][j-1] + i*DIM1 + j)+k ; 
                }
            }
        }
    }
    
    end = clock();
    time = (double)(end - begin) / CLOCKS_PER_SEC;
    
    write_matrix(dataset_id, matrix);

    close_dataset(dataset_id);

    return time;
}

void tear_down_pure(hid_t file_id){
    close_file(file_id);
}

double run_pure(){
    hid_t file_id;
    double time;
    file_id = set_up_pure();
    time = test_pure(file_id);
    tear_down_pure(file_id);

    return time;
}
