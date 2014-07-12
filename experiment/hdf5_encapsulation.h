#ifndef _ENCAPSULATION_H_
#define _ENCAPSULATION_H_

#include "hdf5.h"

hid_t set_core();

void create_file(hid_t fapl, const char *name);
hid_t open_file(hid_t fapl, const char *name);
void close_file(hid_t file_id);

void create_dataset(hid_t file_id, const hsize_t dim0, const hsize_t dim1, const char *name);
hid_t open_dataset(hid_t file_id, const char *name);
void close_dataset(hid_t dataset_id);


#endif
