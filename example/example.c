/*
  To serve as an example for the experiment.
*/


#include "hdf5.h"
#include <stdlib.h>
#define FILE "dset.h5"

#define DATASETNAME "IntArray" 
#define RANK  2

#define DIM0     4                          /* size of dataset */       
#define DIM1     6 
#define DIM0_SUB  3                         /* subset dimensions */ 
#define DIM1_SUB  4 

hid_t set_core(){

    printf("Creating default File Access Property List...\n");
    hid_t fapl;
    fapl = H5Pcreate(H5P_FILE_ACCESS); // Create a FAPL for file access.
    if (fapl < 0){
        printf("Failed to initialize property list.\n");
        exit(1);
    }
    printf("Got id: \"%d\" for FAPL.\n", fapl);
        
    size_t increments;
    increments = 1024; // 1KiB
    hbool_t backing_store;
    backing_store = 1;

    
    herr_t ret;

    printf("Setting FAPL to use the core driver.\n");
    ret = H5Pset_fapl_core(fapl
                           , 1024 // increments: 1KiB
                           , 1    // backing store: Yes 
        );

    if (ret < 0){
        printf("Failed to set FAPL to core.\n");
        exit(1);
    }
    printf("Set FAPL to use the core driver.\n\n");
    
    return fapl;
}

void create_file(hid_t fapl){
    hid_t  file_id;
    printf("Creating a new file: " FILE ".\n");
    file_id = H5Fcreate(FILE // Filename
                        , H5F_ACC_TRUNC // Truncate if exists
                        , H5P_DEFAULT // File access and creation property list
                        , fapl // FAPL ID.
        );
    
    if (file_id < 0){
        printf("Failed to create file.\n");
        exit(1);
    }
    printf("Got id: \"%d\" for file.\n", file_id);

    /* Close the file. */
    herr_t status;
    printf("Closing file: %d.\n",file_id);
    status = H5Fclose(file_id);
    if (file_id < 0){
        printf("Failed to close file.\n");
        exit(1);
    }
    printf("File closed.\n\n");
}

hid_t open_file(hid_t fapl){
    printf("Opening file: " FILE ".\n");
    hid_t file_id;
    file_id = H5Fopen(FILE // Filename
                      , H5F_ACC_RDWR // Read/Write access 
                      , fapl //FAPL ID
        );
    if (file_id < 0){
        printf("Failed to open file.\n");
        exit(1);
    }
    printf("Got id: \"%d\" for file.\n\n",file_id);

    return file_id;
}

hid_t create_dataset(hid_t file_id){
    /* Create the data space for the dataset. */
    hid_t dataspace_id;  /* identifiers */
    hsize_t     dims[RANK];
    dims[0] = DIM0; 
    dims[1] = DIM1;

    printf("Creating simple dataspace.\n");
    dataspace_id = H5Screate_simple( RANK // Nr of dimensions
                                    , dims // array specifying the dimensions
                                    , NULL // array specifying the maximum size of each dimension
        );

    if (dataspace_id < 0){
        printf("Failed to create dataspace.\n");
        exit(1);
    }
    printf("Got id \"%d\" for dataspace.\n",dataspace_id);
    
    /* Create the dataset. */
    hid_t       dataset_id;

    printf("Creating dataset.\n");
    dataset_id = H5Dcreate2(file_id // file ID
                            , DATASETNAME // name of data set
                            , H5T_STD_I32BE // type of data
                            , dataspace_id // Dataspace id
                            , H5P_DEFAULT // link creation property list
                            , H5P_DEFAULT // dataset creation property list 
                            , H5P_DEFAULT // dataset access property list
        );
    if (dataset_id < 0){
        printf("Failed to create dataset.\n");
        exit(1);
    }
    printf("Got id \"%d\" for dataset.\n",dataset_id);

    /* End access to the dataset and release resources used by it. */
    hid_t status;
    printf("Closing dataset.\n");
    status = H5Dclose(dataset_id);
    if (status < 0){
        printf("Failed to close dataset.\n");
        exit(1);
    }
    printf("Closed dataset.\n");

    printf("Closing dataspace.\n");
    status = H5Sclose(dataspace_id);
    if (status < 0){
        printf("Failed to close dataspace.\n");
        exit(1);
    }
    printf("Closed dataspace.\n\n");
    
    return dataset_id;
}

void modify_dataset(hid_t file_id, hid_t dataset_id){
    int i, j, dset_data[DIM0][DIM1];
    /* Initialize the modifieddataset. */
    for (i = 0; i < DIM0 ; i++)
        for (j = 0; j < DIM1 ; j++)
            dset_data[i][j] = i * DIM1 + j + 1;


    /* Open an existing dataset. */
    printf("Opening dataset.\n");
    dataset_id = H5Dopen2(file_id // File ID
                          , DATASETNAME // Dataset name
                          , H5P_DEFAULT // Dataset access property list 
        );
    if (dataset_id < 0){
        printf("Failed to open dataset.\n");
        exit(1);
    }
    printf("Got id \"%d\" for dataset.\n");

    /* Write the dataset. */
    herr_t status;
    printf("Writing to dataset.\n");
    status = H5Dwrite(dataset_id // Dataset id
                      , H5T_NATIVE_INT // data type
                      , H5S_ALL // Memory dataspace and selection therein
                      , H5S_ALL // selection within the file dataset's dataspace
                      , H5P_DEFAULT // Transfer Property List identifier
                      , dset_data // pointer to data to be written
        );
    if (status < 0){
        printf("Failed to write dataset.\n");
        exit(1);
    }
    printf("Wrote to dataset.\n");

    /* status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, dset_data); */

    /* Close the dataset. */
    printf("Closing dataset.\n");
    status = H5Dclose(dataset_id);
    if (status < 0){
        printf("Failed to close dataset.\n");
        exit(1);
    }
    printf("Closed dataset.\n\n");
}

void modify_partial_dataset(hid_t file_id, hid_t dataset_id){
    hsize_t     dims[RANK], dimsm[RANK];   
    int         data[DIM0][DIM1];           /* data to write */
    int         sdata[DIM0_SUB][DIM1_SUB];  /* subset to write */
    int         rdata[DIM0][DIM1];          /* buffer for read */
 
    hid_t       dataspace_id, memspace_id; 

    herr_t      status;                             
   
    hsize_t     count[RANK];              /* size of subset in the file */
    hsize_t     offset[RANK];             /* subset offset in the file */
    hsize_t     stride[RANK];
    hsize_t     block[RANK];
    int         i, j;

    offset[0] = 1;
    offset[1] = 2;

    count[0]  = DIM0_SUB;  
    count[1]  = DIM1_SUB;

    stride[0] = 1;
    stride[1] = 1;

    block[0] = 1;
    block[1] = 1;

    /* Open an existing dataset. */
    printf("Opening dataset.\n");
    dataset_id = H5Dopen2(file_id // File ID
                          , DATASETNAME // Dataset name
                          , H5P_DEFAULT // Dataset access property list 
        );
    if (dataset_id < 0){
        printf("Failed to open dataset.\n");
        exit(1);
    }
    printf("Got id \"%d\" for dataset.\n");
    
    /* Create memory space with size of subset. Get file dataspace 
       and select subset from file dataspace. */

    dimsm[0] = DIM0_SUB;
    dimsm[1] = DIM1_SUB;

    // Create a dataspace for the subset
    memspace_id = H5Screate_simple (RANK //Rank
                                    , dimsm //current dimensions of dataspace
                                    , NULL); //maximum dimensions of dataspace

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

    for (j = 0; j < DIM0_SUB; j++) {
	for (i = 0; i < DIM1_SUB; i++)
	   sdata[j][i] = 5;
    }

    // write to the hyperslab
    status = H5Dwrite (dataset_id
                       , H5T_NATIVE_INT
                       , memspace_id
                       , dataspace_id
                       , H5P_DEFAULT
                       , sdata
        );
    
     //status = H5Dread (dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);

    status = H5Sclose (memspace_id);
    status = H5Sclose (dataspace_id);
    status = H5Dclose (dataset_id);

}
    
void close_file(hid_t file_id){
    herr_t status;
    printf("Closing file.\n");
    status = H5Fclose(file_id);
    if (status < 0){
        printf("Failed to close file");
        exit(1);
    }
    printf("Closed file.\n\n");
}

int main() {


    hid_t fapl;
    fapl = set_core();

    create_file(fapl);
   
    hid_t file_id;
    file_id = open_file(fapl);

    hid_t dataset_id;
    dataset_id = create_dataset(file_id);

    modify_dataset(file_id,dataset_id);
    modify_partial_dataset(file_id,dataset_id);
    
    close_file(file_id);
}
