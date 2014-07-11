#include "hdf5.h"
#include "hdf5_hl.h"
#include <stdlib.h>
#include <stdio.h>
#define FILE "file.h5"

int main() {

    hid_t       file_id;   /* file identifier */
    herr_t      status;
    ssize_t     file_size;
    void        *buffer_ptr;
    unsigned    flags;
    
    /* Create a new file using default properties. */
    file_id = H5Fcreate(FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

        
    hsize_t size;
    H5Fget_filesize(file_id, &size); //size of file
    buffer_ptr = malloc(size); // pointer to free space in memory for file

    flags = H5LT_FILE_IMAGE_OPEN_RW | H5LT_FILE_IMAGE_DONT_COPY | H5LT_FILE_IMAGE_DONT_RELEASE;
    H5LTopen_file_image(buffer_ptr, file_size, flags);

        
    free(buffer_ptr);

        
    /* Terminate access to the file. */
    status = H5Fclose(file_id); 

}
