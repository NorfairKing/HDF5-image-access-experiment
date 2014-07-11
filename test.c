/* 
 *  This example illustrates how to write and read data in an existing
 *  dataset.  It is used in the HDF5 Tutorial.
 */

#include "hdf5.h"
#define FILE "dset.h5"

int main() {

    hid_t       file_id;  /* identifiers */
   herr_t      status;
   int         i, j, dset_data[4][6];


   /* Open an existing file. */
   //file_id = H5Fopen(FILE, H5F_ACC_RDWR, H5P_DEFAULT);

   // Create default property list, NOT A FILE.
   hid_t fapl;
   fapl = H5Pcreate(H5P_FILE_ACCESS);
   if (fapl < 0){
       printf("Failed to initialize property list");
       goto error;
   }

   size_t increments;
   increments = 1024; // 1KiB
   hbool_t backing_store;
   backing_store = 1;
       
   herr_t ret;
   // SETTING FAPL TO CORE
   ret = H5Pset_fapl_core(fapl,  increments , backing_store );

   if (ret < 0){
       printf("Failed to set FAPL to core.");
       goto error;
   }

   // FAPL IS SET TO CORE

   
   // CREATING A FILE HERE
   file_id = H5Fcreate(FILE, H5F_ACC_TRUNC,H5P_DEFAULT, fapl);
   if (file_id < 0){
       printf("Failed to create file.\n");
       goto error;
   }
   printf("Created file with id: %d\n.", file_id);

   /* Close the file. */
   status = H5Fclose(file_id);
   if (file_id < 0){
       printf("Failed to close file.\n");
       goto error;
   }
   
   // FILE IS CREATED FROM HERE ON
   

   // OPENING FILE HERE
   printf("Opening file: " FILE "\n");
   file_id = H5Fopen(FILE, H5F_ACC_RDWR, fapl);
   if (file_id < 0){
       printf("Failed to open file.\n");
       goto error;
   }
   printf("Got file id: %d\n",file_id);

   // FILE IS OPEN FROM HERE ON


   // CREATING DATASET HERE
   
   /* Create the data space for the dataset. */
   hid_t       dataset_id, dataspace_id;  /* identifiers */
   hsize_t     dims[2];
   dims[0] = 4; 
   dims[1] = 6; 
   dataspace_id = H5Screate_simple(2, dims, NULL);

   /* Create the dataset. */
   dataset_id = H5Dcreate2(file_id, "/dset", H5T_STD_I32BE, dataspace_id, 
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

   /* End access to the dataset and release resources used by it. */
   status = H5Dclose(dataset_id);

   // DATASET IS CREATED HERE

   
   // MODIFYING DATASET HERE

   /* Initialize the modifieddataset. */
   for (i = 0; i < 4; i++)
      for (j = 0; j < 6; j++)
         dset_data[i][j] = i * 6 + j + 1;

   
   /* Open an existing dataset. */
   dataset_id = H5Dopen2(file_id, "/dset", H5P_DEFAULT);

   /* Write the dataset. */
   status = H5Dwrite(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                     dset_data);

   status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                    dset_data);

   /* Close the dataset. */
   status = H5Dclose(dataset_id);

   // DATASET IS MODIFIED
   
   /* Close the file. */
   status = H5Fclose(file_id);

stop:
   return 0;
   
error:
   return 1;
}
