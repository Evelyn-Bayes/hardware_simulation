#ifndef HDD_H_
#define HDD_H_

void hdd_write(int offset, int bytes, char* data);
void hdd_read(int offset, int bytes, char* buffer);
void hdd_format_disk(char* hdd_file_name);

#endif
