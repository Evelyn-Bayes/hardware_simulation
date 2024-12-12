#include <stdio.h>
#include <stdlib.h>

int bit_mask_range(int start, int end) {
    if (start < 0 || end > 32 || start > end) {
        printf("Invalid range\n");
        exit(1);
    }

    int bit_mask = 0;
    for (int i = start; i <= end; i++) {
        bit_mask |= 1 << (i - 1);
    }
    return bit_mask;
}
