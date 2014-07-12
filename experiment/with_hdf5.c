#include "hdf5_encapsulation.h"
#include "hdf5.h"

#include <stdlib.h>
#include <time.h>

#define FILE_NAME_WITH_HDF5 "with_hdf5.h5"
#define DATASET_NAME_WITH_HDF5 "/with_hdf5"

#define RANK 2
#define DIM0 10
#define DIM1 15

#define LENGTH 1000

static hsize_t dimsm[RANK] = {1,1};
static hsize_t count[RANK] = {1,1};
static hsize_t stride[RANK] = {1,1};
static hsize_t block[RANK] = {1,1};

double get_elem_with_hdf5(hid_t dataset_id, int i, int j){
    double      rdata[1][1];
    hsize_t     offset[RANK] = {i,j};

    hid_t       dataspace_id, memspace_id;
    herr_t      status;
    
    /* Create memory space with size of subset. Get file dataspace 
       and select subset from file dataspace. */

    // Create a dataspace for the subset
    memspace_id = H5Screate_simple (RANK //Rank
                                    , dimsm //current dimensions of dataspace
                                    , dimsm); //maximum dimensions of dataspace

    // Get the entire dataspace
    dataspace_id = H5Dget_space (dataset_id);

    // Select the hyperslab
    status = H5Sselect_hyperslab (dataspace_id // Dataspace ID
                                  , H5S_SELECT_SET //  	Replaces the existing selection with the parameters from this call
                                  , offset // the offset of the selections
                                  , stride // the amount of values between blocks
                                  , count // The amount of blocks in eacht direction
                                  , block // The size of the blocks in each direction.
        );


    // read from the hyperslab
    status = H5Dread (dataset_id
                     , H5T_NATIVE_DOUBLE
                     , memspace_id
                     , dataspace_id
                     , H5P_DEFAULT
                     , rdata
        );

    if (status < 0){
        printf("Failed to put get element.\n");
        exit(1);
    }
    
    status = H5Sclose (memspace_id);
    status = H5Sclose (dataspace_id);

    return rdata[0][0];
}

double put_elem_with_hdf5(hid_t dataset_id, int i, int j, double value){
    double      sdata[1][1];
    hsize_t     offset[RANK] = {i,j};

    hid_t       dataspace_id, memspace_id;
    herr_t      status;
    
    /* Create memory space with size of subset. Get file dataspace 
       and select subset from file dataspace. */

    // Create a dataspace for the subset
    memspace_id = H5Screate_simple (RANK //Rank
                                    , dimsm //current dimensions of dataspace
                                    , dimsm); //maximum dimensions of dataspace

    // Get the entire dataspace
    dataspace_id = H5Dget_space (dataset_id);

    // Select the hyperslab
    status = H5Sselect_hyperslab (dataspace_id // Dataspace ID
                                  , H5S_SELECT_SET //  	Replaces the existing selection with the parameters from this call
                                  , offset // the offset of the selections
                                  , stride // the amount of values between blocks
                                  , count // The amount of blocks in eacht direction
                                  , block // The size of the blocks in each direction.
        );

    sdata[0][0] = value;
    
    // write to read from the hyperslab
    status = H5Dwrite (dataset_id
                     , H5T_NATIVE_DOUBLE
                     , memspace_id
                     , dataspace_id
                     , H5P_DEFAULT
                     , sdata
        );

    if (status < 0){
        printf("Failed to put element.\n");
        exit(1);
    }
    
    status = H5Sclose (memspace_id);
    status = H5Sclose (dataspace_id);
}

hid_t set_up_with_hdf5(){
    // Set up driver to use ram instead of on-disk files.
    hid_t fapl;
    fapl = set_core();

    create_file(fapl, FILE_NAME_WITH_HDF5);


    hid_t file_id;
    file_id = open_file(fapl, FILE_NAME_WITH_HDF5);

    create_dataset(file_id, DIM0, DIM1, DATASET_NAME_WITH_HDF5);

    /* Initialise with values */
    hid_t dataset_id;
    dataset_id = open_dataset(file_id, DATASET_NAME_WITH_HDF5);

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

double test_with_hdf5(hid_t file_id){
    hid_t dataset_id;
    dataset_id = open_dataset(file_id, DATASET_NAME_WITH_HDF5);

    double matrix[DIM0][DIM1];


    clock_t begin, end;
    double time;

    begin = clock();

    
    int i,j,k;
    for (k = 0; k < LENGTH; k++){
        for (i = 0; i < DIM0; i++){
            for (j = 0; j < DIM1; j++){
                if (i < 1 || j < 1){
                    put_elem_with_hdf5(dataset_id,i,j,1);
                }
                else{
                    //Random computation that includes reading and writing
                    double value;
                    value = get_elem_with_hdf5(dataset_id, i-1,j-1);
                    value = (value + i*DIM1 + j) + k;
                    put_elem_with_hdf5(dataset_id,i,j,value);
                }
            }
        }
    }

    
    end = clock();
    time = (double)(end - begin) / CLOCKS_PER_SEC;

    H5Fflush(file_id, H5F_SCOPE_GLOBAL);

    close_dataset(dataset_id);

    return time;
}

void tear_down_with_hdf5(hid_t file_id){
    close_file(file_id);
}

double run_hdf5(){
    hid_t file_id;
    double time;

    file_id = set_up_with_hdf5();
    time = test_with_hdf5(file_id);
    tear_down_with_hdf5(file_id);
    return time;
}
