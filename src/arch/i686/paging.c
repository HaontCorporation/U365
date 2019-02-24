#include <arch/i686/paging.h>

#include <stdint.h>
#include <stddef.h>

#include <stdio.h>
#include <sys.h>
page_directory_t pgdir[1024] __attribute__((aligned(4096)));
page_table_t     pgtbl[1024][1024] __attribute__((aligned(4096)));

void setup_pgdir(page_directory_t pgdir[1024])
{
    for(int i=0; i<1024; i++)
    {
        //Those page directory entries AREN'T USABLE. It just fills all page directory by non-present page tables.
        pgdir[i].present    = 0;
        pgdir[i].rw_flag    = 1;
        pgdir[i].access_lvl = 0;
    }
}

void paging_ident(page_table_t tbl[1024])
{
    //Identity map first 4 megabytes of memory.
    size_t size=1024;
    for(size_t i=0; i<size; i++)
    {
        tbl[i].rw_flag   =1;
        tbl[i].access_lvl=0;
        tbl[i].phys_addr =i;
        tbl[i].present =1;

    }
}
void map_page(page_table_t tbl[1024], void *addr)
{
    for(size_t i=0; i<4096; i++)
    {
        tbl[i].rw_flag   =1;
        tbl[i].access_lvl=0;
        tbl[i].phys_addr =((int)addr)/4096 + i;
        tbl[i].present =1;

    }
}
void setup_paging()
{
    void load_pg_dir(page_directory_t *);
    void paging_enable(void);
    setup_pgdir(pgdir);
    for(int i=0; i<512; i++)
        paging_ident(pgtbl[i]); //Identity map first 32MB of memory.
    for(int i=0; i<512; i++)
        map_page(pgtbl[512+i], vesa_fb_addr); //Identity map first 32MB of memory.
    /*map_page(
    pgtbl[0],
    srt_info.ScreenFBInfo.framebuffer,
    srt_info.ScreenFBInfo.framebuffer,
    srt_info.ScreenFBInfo.height*srt_info.ScreenFBInfo.width*(srt_info.ScreenFBInfo.fbbpp/8)/PAGE_SIZE
    ); //Identity map our VESA framebuffer*/
    for(unsigned int i=0; i<vesa_fb_height*vesa_fb_width*(vesa_fb_bpp/8)/PAGE_SIZE; i++)
    {
        pgtbl[0][(int) vesa_fb_addr/4096-i].rw_flag   =1;
        pgtbl[0][(int) vesa_fb_addr/4096-i].access_lvl=0;
        pgtbl[0][(int) vesa_fb_addr/4096-i].phys_addr =((int)vesa_fb_addr/4096)+i;
        pgtbl[0][(int) vesa_fb_addr/4096-i].present =1;
    }
    for(int i=0; i<1024; i++)
    {
        pgdir[i].present    = 1;
        pgdir[i].rw_flag    = 1;
        pgdir[i].access_lvl = 0;
        pgdir[i].tbl_addr   = (uint32_t)(pgtbl[i]) >> 12;
    }
    putchar('\n');
    load_pg_dir(pgdir);
    paging_enable();
}