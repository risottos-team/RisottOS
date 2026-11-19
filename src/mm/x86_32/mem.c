#include "mem.h"

static BlockHeader *kernel_first_block = 0;
static BlockHeader *user_first_block = 0;
static unsigned char *user_heap_start = 0;
static unsigned char *user_heap_end = 0;

static void heap_init(BlockHeader **first_block, void *heap_start,
                      unsigned int heap_size) {
    *first_block = (BlockHeader *)heap_start;
    (*first_block)->size = heap_size - sizeof(BlockHeader);
    (*first_block)->is_free = 1;
    (*first_block)->next = 0;
}

static void *heap_malloc(BlockHeader *first_block, unsigned int size) {
    BlockHeader *current = first_block;
    while (current != 0) {
        if (current->is_free == 1 && current->size >= size) {
            if (current->size > size + sizeof(BlockHeader)) {
                BlockHeader *new_block = (BlockHeader *)((unsigned char *)current + sizeof(BlockHeader) + size);
                new_block->size = current->size - (size + sizeof(BlockHeader));
                new_block->is_free = 1;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }

            current->is_free = 0;
            return (void *)((unsigned char *)current + sizeof(BlockHeader));
        }

        current = current->next;
    }

    return 0;
}

static void heap_free(BlockHeader *first_block, void *ptr) {
    if (!ptr) return;
    BlockHeader *header = (BlockHeader *)((unsigned char *)ptr - sizeof(BlockHeader));
    header->is_free = 1;

    BlockHeader *current = first_block;

    while (current != 0 && current->next != 0) {
        if (current->is_free == 1 && current->next->is_free == 1) {
            current->size += sizeof(BlockHeader) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

void mem_init(void *heap_start, unsigned int heap_size) {
    heap_init(&kernel_first_block, heap_start, heap_size);
}

void *r_malloc(unsigned int size) {
    return heap_malloc(kernel_first_block, size);
}

void r_free(void *ptr) {
    heap_free(kernel_first_block, ptr);
}

void user_mem_init(void *heap_start, unsigned int heap_size) {
    heap_init(&user_first_block, heap_start, heap_size);
    user_heap_start = (unsigned char *)heap_start;
    user_heap_end = user_heap_start + heap_size;
}

void *user_malloc(unsigned int size) {
    return heap_malloc(user_first_block, size);
}

void user_free(void *ptr) {
    if ((unsigned char *)ptr < user_heap_start ||
        (unsigned char *)ptr >= user_heap_end) {
        return;
    }
    heap_free(user_first_block, ptr);
}