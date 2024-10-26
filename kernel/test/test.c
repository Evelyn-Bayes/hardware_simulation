#include "../simulated_resources/disk.h"

#include <stdio.h>

int main() {
    char data[] = "stupid";
    write(0, sizeof(data), data);
    char* returned = read(0, 6);
    printf("Returned val: %s", returned);
}
