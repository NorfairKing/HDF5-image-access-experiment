#include "hdf5.h"

#include <stdlib.h>

#define INCREMENTS 1024 // 1 KiB
#define BACKING_STORE 1 // = Store to file when file is closed.

#define RANK 2

hid_t set_core() {

    // Create default File Access Property List
    hid_t fapl;
    fapl = H5Pcreate(H5P_FILE_ACCESS); // Create a FAPL for file access.

    //Set FAPL to use the core driver.
    herr_t ret;
    H5Pset_fapl_core(fapl
            , INCREMENTS // increments: 1KiB
            , BACKING_STORE // backing store: Yes 
            );

    return fapl;
}

void create_file(hid_t fapl, const char *name) {
    // Create a new file:
    hid_t file_id;
    file_id = H5Fcreate(name // Filename
            , H5F_ACC_TRUNC // Truncate if exists
            , H5P_DEFAULT // File access and creation property list
            , fapl // FAPL ID.
            );

    // Close file
    H5Fclose(file_id);
}

hid_t open_file(hid_t fapl, const char *name) {
    // Open file
    hid_t file_id;
    file_id = H5Fopen(name // Filename
            , H5F_ACC_RDWR // Read/Write access 
            , fapl //FAPL ID
            );

    return file_id;
}

void close_file(hid_t file_id) {
    // Close file
}

void create_dataset(hid_t file_id, const hsize_t dim0, const hsize_t dim1, const char *name) {
    hid_t dataspace_id;

    hsize_t dims[RANK];
    dims[0] = dim0;
    dims[1] = dim1;

    // Create simple data space.
    dataspace_id = H5Screate_simple(RANK // Nr of dimensions
            , dims // array specifying the dimensions
            , dims // array specifying the maximum size of each dimension
            );

    // Create the dataset
    hid_t dataset_id;
    dataset_id = H5Dcreate2(file_id // file ID
            , name // name of data set
            , H5T_NATIVE_DOUBLE // type of data
            , dataspace_id // Data space id
            , H5P_DEFAULT // link creation property list
            , H5P_DEFAULT // dataset creation property list 
            , H5P_DEFAULT // dataset access property list
            );

    // Close the dataset
    H5Dclose(dataset_id);
    // Close the data space
    H5Sclose(dataspace_id);
}

hid_t open_dataset(hid_t file_id, const char *name) {
    // Open the dataset
    hid_t dataset_id;
    dataset_id = H5Dopen2(file_id // File ID
            , name // Dataset name
            , H5P_DEFAULT // Dataset access property list 
            );
    return dataset_id;
}

void close_dataset(hid_t dataset_id) {
    // Close the dataset
    H5Dclose(dataset_id);
}
