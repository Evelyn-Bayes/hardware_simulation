#ifndef HDD_H_
#define HDD_H_

void hdd_write(int offset, int bytes, char *data);
char* hdd_read(int offset, int bytes);

#endif
