#include "hdf5.h"

#include <stdlib.h>

#define INCREMENTS 1024 // 1 KiB
#define BACKING_STORE 1 // = Store to file when file is closed.

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
