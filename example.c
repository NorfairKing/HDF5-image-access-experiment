/*
  To serve as an example for the experiment.
*/


#include "hdf5.h"
#include <stdlib.h>
#define FILE "dset.h5"

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
    hsize_t     dims[2];
    dims[0] = 4; 
    dims[1] = 6;

    printf("Creating simple dataspace.\n");
    dataspace_id = H5Screate_simple(2 // Nr of dimensions
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
                            , "/dset" // name of data set
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
    int i, j, dset_data[4][6];
    /* Initialize the modifieddataset. */
    for (i = 0; i < 4; i++)
        for (j = 0; j < 6; j++)
            dset_data[i][j] = i * 6 + j + 1;


    /* Open an existing dataset. */
    printf("Opening dataset.\n");
    dataset_id = H5Dopen2(file_id // File ID
                          , "/dset" // Dataset name
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

    close_file(file_id);
}
