#include <arch/i686/mboot.h>
#include <tty_old.h>
#include <stdint.h>
#include <debug.h>
#include <stdio.h>

void*    memlist;
uint64_t memlist_size;

const void* minaddr = (void*)(0x500);
extern int kernel_phys_addr;
extern int kend;

typedef struct allar allar;
struct allar
{
	allar* prev;
	allar* next;
	void* area;
	void* end;
};

typedef struct memlist_node memlist_node;
struct memlist_node
{
	uint32_t next;
	uint64_t addr;
	uint64_t size;
	uint32_t type;
};

allar* allar_start = (allar*)(0x500);
allar* allar_curr  = (allar*)(0x500);
void* allar_all_pos = (void*)(0x500 + sizeof(allar));

void* allar_alloc(size_t size)
{
	while((uint8_t*)(allar_all_pos) + size > (uint8_t*)(allar_curr->end))
	{
		allar_curr = allar_curr->next;
		allar_all_pos = allar_curr->area;
		if(!allar_curr)
			return 0;
	}
	void* result = allar_all_pos;
	allar_all_pos = (uint8_t*)(allar_all_pos) + size;
	return result;
}

void allar_free(void* UNUSED(ptr)){}

void add_allar_region(allar* st, void* b, void* e)
{
	if(b > e)
		return;
	// If the area is after the given
	if(st->end < b)
	{
		if(st->next)
		{
			add_allar_region(st->next, b, e);
		}
		else
		{
			allar* na = allar_alloc(sizeof(allar));
			na->area = b;
			na->end = e;
			na->prev = st;
			na->next = st->next;
			if(st->next)
				st->next->prev = na;
			st->next = na;
		}
	}
	else
	{
		if(b < st->area && e >= st->area)
			st->area = b;
		if(e > st->end && b <= st->end)
			st->end = e;
	}
}

void add_nosave_area(allar* st, void* b, void* e)
{
	if(b > e)
		return;
	uint8_t fr = 0;
	if(b <= st->end)
	{
		// Remove the whole region
		if(b <= st->area && e >= st->end)
		{
			// DO IT
			if(st->prev)
				st->prev->next = st->next;
			if(st->next)
				st->next->prev = st->prev;
			fr = 1;
		}
		// Shrink to right
		if(b <= st->area && e > st->area && e < st->end)
		{
			st->area = e;
		}
		// Shrink to left
		if(b > st->area && b < st->end && e >= st->end)
		{
			st->end = b;
		}
		if(b > st->area && e < st->end)
		{
			allar* an = allar_alloc(sizeof(allar));
			an->area = b;
			an->end = st->end;
			st->end = b;

			an->prev = st;
			an->next = st->next;
			if(st->next)
				st->next->prev = an;
			st->next = an;
		}
	}
	if(e > st->end && st->next)
	{
		add_nosave_area(st->next, b, e);
	}
	// freeing does not currently work, but we'll keep this thing until it will become functional
	if(fr)
		allar_free(st);
}

void parse_memlist(mboot_info* mboot)
{
	memlist_node* curr = (memlist_node*)(size_t)(mboot->mmap_addr);
	memlist_node* end = (memlist_node*)((uint8_t*)(curr) + mboot->mmap_length);

	memset(allar_start, 0, sizeof(allar));
	allar_start->end = (void*)(0x600);

	for(uint32_t i = 0; curr < end; ++i)
	{
		if(curr->type == 1)
		{
			// Casting to size_t will shrink it to the exact pointer size
			void* b = (void*)(size_t)(curr->addr);
			uint64_t k = curr->addr + curr->size;
			void* e = (void*)(size_t)(k);
			if((size_t)(b) == curr->addr)
			{
				if((size_t)(e) == k)
				{
					add_allar_region(allar_start, b, e);
				}
				else
				{
					add_allar_region(allar_start, b, (void*)(0xFFFFFFFFFFFFFFFFLL));
					printk("Shrinking region to the biggest representable size");
				}
			}
			else
			{
				printk("Memory region out of representable range");
			}
		}
		curr = (memlist_node*)((uint8_t*)(curr) + curr->next + 4);
	}

	add_nosave_area(allar_start, 0, (void*)(0x500));
	add_nosave_area(allar_start, &kernel_phys_addr, (uint8_t*)(&kend) + 1);
	add_nosave_area(allar_start, (void*)(0x00007C00), (void*)(0x00007DFF)); // Boot sector
	add_nosave_area(allar_start, (void*)(0x0000FDD4), (void*)(0x00010000)); // Just some kind of bad area
	add_nosave_area(allar_start, (void*)(0x00F00000), (void*)(0x01000000)); // ISA Hole
	add_nosave_area(allar_start, mboot, (uint8_t*)(mboot) + sizeof(struct mboot_info)); // Important data
	add_nosave_area(allar_start, (void*)(size_t)(mboot->framebuffer_addr), (void*)(size_t)(mboot->addr + mboot->framebuffer_height * mboot->framebuffer_pitch * mboot->framebuffer_bpp));
	printk_on = 1;
	printk("Final memory areas:");
	allar* prcu = allar_start;
	while(prcu)
	{
		printk("[0x%016llx - 0x%016llx]", (uint64_t)(size_t)(prcu->area), (uint64_t)(size_t)(prcu->end));
		prcu = prcu->next;
	}
	printk_on = 0;
}

void* malloc(int s)
{
	void* result = allar_alloc(s);
	return result;
}
