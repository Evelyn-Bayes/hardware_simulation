#include <stdio.h>
#include <stdlib.h>

struct circular_queue {
    int capacity;
    int size;
    int first_element_offset;
    int last_element_offset;
    int* elements;
};

struct circular_queue new(int capacity) {
    if (capacity < 0) {
        printf("Queue size can't be negative.\n");
        exit(1);
    }

    int* elements = malloc(capacity * sizeof(int))
    struct circular_queue queue = {capacity, 0, 0, 0, elements};

    return queue;
}

int capacity(circular_queue queue) {
    return queue.capacity - queue.size ;
}

void append(circular_queue queue, int element) {
    if (queue.size == queue.capacity) {
        printf("Queue has insufficent capacity.\n");
        exit(1);
    }

    queue->last_element_offset++;
    if (queue.last_element_offset == capacity) {
        queue.last_element_offset = 0;
    }

    queue->size++;
    *(queue.elements + queue.last_element_offset) = element;
}

int pop_first(circular_queue queue) {
    if (queue.size == queue.capacity) {
        printf("Queue is empty.\n");
        exit(1);
    }

    queue.first_element_offset--;
    if (queue.first_element_offset == -1) {
        queue.first_element_offset = capacity - 1;
    }

    queue.size--;
    if (queue.first_element_offset == capacity - 1) {
        return queue.elements*;
    } else {
        return *(queue.elements + queue.first_element_offset - 1);
    }

void free_memory(circular_queue queue) {
    free(queue.elements);
}

int get(circular_queue queue, int offset) {
    if (offset < 0) {
        printf("Negative offset is invalid.\n");
        exit(1);
    } else if (offset >= queue.size) {
        printf("Invalid offset larger then queue size.\n");
        exit(1);
    }

    if (queue.first_element_offset + offset >= queue.capacity) {
        return *(queue.elements + queue.first_element_offset + offset - queue.capacity);
    } else {
        return *(queue.elements + queue.first_element_offset + offset);
    }
}
