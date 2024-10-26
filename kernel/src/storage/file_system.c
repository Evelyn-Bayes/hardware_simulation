struct file_system {
    int volume_first_block_offset_bytes;
    int block_count;
    int next_free_block;
    int[] freed_blocks;
};

struct file_system new()

struct block {
    int file_system_offset_bytes;
    char[4028] data;
}


