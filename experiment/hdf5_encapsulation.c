#include "hdf5.h"

#include <stdlib.h>

#define INCREMENTS 1024 // 1 KiB
#define BACKING_STORE 1 // = Store to file when file is closed.

#define RANK 2

hid_t set_core(){

    printf("Creating default File Access Property List...\n");
    hid_t fapl;
    fapl = H5Pcreate(H5P_FILE_ACCESS); // Create a FAPL for file access.
    if (fapl < 0){
        printf("Failed to initialize property list.\n");
        exit(1);
    }
    printf("Got id: \"%d\" for FAPL.\n", fapl);
        
    


    printf("Setting FAPL to use the core driver.\n");
    herr_t ret;
    ret = H5Pset_fapl_core(fapl
                           , INCREMENTS // increments: 1KiB
                           , BACKING_STORE    // backing store: Yes 
        );

    if (ret < 0){
        printf("Failed to set FAPL to core.\n");
        exit(1);
    }
    printf("Set FAPL to use the core driver.\n\n");
    
    return fapl;
}

void create_file(hid_t fapl, const char *name){
    hid_t  file_id;
    printf("Creating a new file: ");
    printf(name);
    printf(".\n");
    file_id = H5Fcreate(name // Filename
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

hid_t open_file(hid_t fapl, const char *name){
    printf("Opening file: ");
    printf(name);
    printf(".\n");
    hid_t file_id;
    file_id = H5Fopen(name // Filename
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

void create_dataset(hid_t file_id, const hsize_t dim0, const hsize_t dim1, const char *name){
    hid_t dataspace_id;

    hsize_t dims[RANK];
    dims[0] = dim0;
    dims[1] = dim1;
    
    printf("Creating simple dataspace.\n");
    dataspace_id = H5Screate_simple( RANK // Nr of dimensions
                                    , dims // array specifying the dimensions
                                    , dims // array specifying the maximum size of each dimension
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
                            , name // name of data set
                            , H5T_NATIVE_DOUBLE // type of data
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
}

hid_t open_dataset(hid_t file_id, const char *name){
    hid_t dataset_id;
    /* Open an existing dataset. */
    printf("Opening dataset.\n");
    dataset_id = H5Dopen2(file_id // File ID
                          , name // Dataset name
                          , H5P_DEFAULT // Dataset access property list 
        );
    if (dataset_id < 0){
        printf("Failed to open dataset.\n");
        exit(1);
    }
    printf("Got id \"%d\" for dataset.\n");
    return dataset_id;
}

void close_dataset(hid_t dataset_id){
    printf("Closing dataset.\n");
    herr_t status;
    status = H5Dclose(dataset_id);
    if (status < 0){
        printf("Failed to close dataset.\n");
        exit(1);
    }
    printf("Closed dataset.\n\n");
}
