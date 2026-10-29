#ifndef R_PAGING_H
#define R_PAGING_H

void paging_init(void);
int paging_map_mmio(unsigned int pyshical, unsigned int size);
void page_fault_handler(void);
int user_range_valid(const void *address, unsigned int size);
int user_string_valid(const char *string, unsigned int max_length);

#endif
