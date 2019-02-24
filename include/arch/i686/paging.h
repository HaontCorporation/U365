#ifndef PAGING_H
#define PAGING_H
#define PAGE_SIZE 4096
//4 kb
typedef struct page_directory_t page_directory_t;
typedef struct page_table_t page_table_t;

struct page_directory_t
{
    unsigned int present       : 1;
    unsigned int rw_flag       : 1;
    unsigned int access_lvl    : 1; //0 is for only ring0. 1 is for anybody.
    unsigned int write_through : 1;
    unsigned int cache_off     : 1;
    unsigned int accessed      : 1;
    unsigned int zero          : 1;
    unsigned int page_size     : 1;
    unsigned int unused        : 4;
    unsigned int tbl_addr      : 20;
};

struct page_table_t
{
    unsigned int present       : 1;
    unsigned int rw_flag       : 1;
    unsigned int access_lvl    : 1; //0 is for only ring0. 1 is for anybody.
    unsigned int write_through : 1;
    unsigned int cache_off     : 1;
    unsigned int accessed      : 1;
    unsigned int dirty         : 1;
    unsigned int zero          : 1;
    unsigned int global        : 1;
    unsigned int reserved      : 3;
    unsigned int phys_addr     : 20;
};

void setup_paging();

#endif // PAGING_H
