#ifndef _HDD_H_
#define _HDD_H_

#include <inttypes.h>

void hdd_write(uint32_t offset, uint32_t bytes, char* data);
void hdd_read(uint32_t offset, uint32_t bytes, char* buffer);
void format_disk(char* hdd_file_name);

#endif
