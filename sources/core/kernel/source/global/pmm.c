#include <lib/log.h>
#include <impl/vmm.h>
#include <lib/lock.h>
#include <global/pmm.h>
#include <impl/panic.h>
#include <lib/bitmap.h>

typedef struct free_list_info_end_s{
    struct free_list_info_end_s* end;
    struct free_list_info_s* start;
}free_list_info_end_t;

typedef struct free_list_info_s{
    /* we put it on a header because if the segment is only one page long the end and start struct is at the same address */
    free_list_info_end_t header;
    uint64_t index_start;
    uint64_t index_end;
    uint64_t page_count;
    struct free_list_info_s* last;
    struct free_list_info_s* next;
}free_list_info_t;

uint64_t available_pages = 0;
uint64_t used_pages = 0;
uint64_t reserved_pages = 0;
uint64_t total_pages = 0;

uint64_t highest_page_index = 0;
uint64_t last_used_index = 0;

static bitmap_t pmm_bitmap;
static spinlock_t pmm_lock;
static uint64_t pmm_first_free_page_index = 0;
static free_list_info_t* pmm_last_free_entry_free_list = NULL;

static inline uint64_t pmm_address_to_index(void* address) {
    return ((uint64_t)address) >> 12;
}

static inline void* pmm_convert_index_to_address(uint64_t index) {
    return (void*)(index << 12);
}

static void pmm_remove_pages_to_free_list(uint64_t index, uint64_t page_count) {
    /* in this function we want to destroy everything but with some pity for the kernel so we will only destroy what we need to destroy */
    free_list_info_t* free_list_info = (free_list_info_t*)vmm_get_virtual_address(pmm_convert_index_to_address(index));
    if(free_list_info->page_count == page_count) {
        /* so here our caller are asking us to destroy the all segments */
        /* so we just need to relink to create a world where this segment have never existed... */
        if(free_list_info->last != NULL) {
            free_list_info->last->next = free_list_info->next;
        }
        if(free_list_info->next != NULL) {
            free_list_info->next->last = free_list_info->last;
        }

        /* check if it's not the entry */
        if(free_list_info == pmm_last_free_entry_free_list) {
            /* so if it's last of the list we just need to update the entry to the last field because it's the only field of the free list info that's abble to be not NULL */
            pmm_last_free_entry_free_list = free_list_info->last;
        }
    }else{
        /* we have to move forward the header */
        free_list_info_t* new_free_list_info = (free_list_info_t*)vmm_get_virtual_address(pmm_convert_index_to_address(index + page_count));
        
        /* steal the link like good stealer */
        new_free_list_info->next = free_list_info->next;
        new_free_list_info->last = free_list_info->last;

        /* check if the segment that we're stealing have still link in his pocket */
        if(free_list_info == pmm_last_free_entry_free_list) {
            pmm_last_free_entry_free_list = new_free_list_info;
        }

        /* update some trivial numbers */
        /* remove some size */
        new_free_list_info->page_count =  free_list_info->page_count - page_count;
        new_free_list_info->index_start = free_list_info->index_start + page_count;

        /* we keep the position of the end segment */
        new_free_list_info->index_end = free_list_info->index_end;

        /* save the end position in the new struct */
        new_free_list_info->header.end = free_list_info->header.end;

        /* update the end */
        new_free_list_info->header.end->start = new_free_list_info;
    }
}

static void pmm_add_page_to_free_list(uint64_t index, uint64_t page_count) {
    /* merge with free list */
    bool is_next_free = !bitmap_get_bit(pmm_bitmap, index + page_count);
    bool is_last_free = !bitmap_get_bit(pmm_bitmap, index - 1);
    if(is_next_free) {
        if(is_last_free) {
            /* so we have next and last, and this we need to merge them */
            /* get next structure start which we can locate at the end of this (index + page_count)*/
            free_list_info_t* free_list_info_next = (free_list_info_t*)vmm_get_virtual_address(pmm_convert_index_to_address(index + page_count)); /* we can get start because we are at the start of the free_list segment */
            
            /* locate end of the last to find the start header to have necessary informations */
            free_list_info_end_t* free_list_info_end = (free_list_info_end_t*)vmm_get_virtual_address(pmm_convert_index_to_address(index - 1));
            
            /* as i said before we want to locate the last headers which will be the main header of the new segment*/
            free_list_info_t* free_list_info_last = free_list_info_end->start; 

            /* here we will destory last end and next start structure because we don't need them anymore */

            /* update the main structures */
            /* as i said before we only keep the main structure and the end structure so link them together */
            free_list_info_last->header.end = free_list_info_next->header.end;
            free_list_info_last->header.end->start = free_list_info_last;
            free_list_info_last->index_end = free_list_info_next->index_end;

            /* update size */
            free_list_info_last->page_count += page_count + free_list_info_next->page_count;

            /* relink, i mean here we destroy segment so we have to be careful to not destroy the main list */
            /* so as we destroy the next start structure let's remove it from the list */
            if(free_list_info_next->last) {
                free_list_info_next->last->next = free_list_info_next->next;
            }
            if(free_list_info_next->next) {
                free_list_info_next->next->last = free_list_info_next->last;
            }

            /* don't be happy now we have still work to do, if the entry pmm_last_free_entry_free_list is the segment to destroy */
            if(free_list_info_next == pmm_last_free_entry_free_list) {
                /* if it's the case the next field will be NULL so we can only use the last field of the struct */
                pmm_last_free_entry_free_list = free_list_info_next->last;
            }
        }else{
            /* so here only next is free let's destroy the next start structure */
            /* locate useful structures */
            free_list_info_t* free_list_info_next = (free_list_info_t*)vmm_get_virtual_address(pmm_convert_index_to_address(index + page_count));
            free_list_info_t* new_free_list_info = (free_list_info_t*)vmm_get_virtual_address(pmm_convert_index_to_address(index));

            /* and link the start struct with the end*/
            new_free_list_info->header.end = free_list_info_next->header.end;
            
            /* update end structures and link it to the new structure */
            new_free_list_info->header.end->start = new_free_list_info;

            /* the new structures is the start */
            new_free_list_info->index_start = index;

            /* set size */
            new_free_list_info->page_count = free_list_info_next->page_count + page_count;

            /* keep index end we don't change anything here */
            new_free_list_info->index_end = free_list_info_next->index_end;

            /* relink, here we will steal the list from the next field because we want to destroy it definitively */
            new_free_list_info->next = free_list_info_next->next;
            new_free_list_info->last = free_list_info_next->last;

            /* check if the segment that we're stealing have still link in his pocket */
            if(free_list_info_next == pmm_last_free_entry_free_list) {
                pmm_last_free_entry_free_list = new_free_list_info;
            }
        }
    }else if(is_last_free) {
        /* get strucutres */
        free_list_info_end_t* free_list_info_end = (free_list_info_end_t*)vmm_get_virtual_address(pmm_convert_index_to_address(index - 1));
        
        /* we get the end segment so with the header locate the start segment */
        free_list_info_t* free_list_info_last = free_list_info_end->start;
        
        /* locate the new end structures */
        free_list_info_end = (free_list_info_end_t*)vmm_get_virtual_address(pmm_convert_index_to_address(index + page_count - 1));
        
        /* add new pages we found */
        free_list_info_last->page_count += page_count;
        free_list_info_last->index_end = index + page_count - 1;

        /* define end and start */
        free_list_info_last->header.end = free_list_info_end;
        free_list_info_end->start = free_list_info_last;

        /* we do not link to the list here because we already done that when we create the segment */
    }else{
        /* get strucutres */
        free_list_info_t* free_list_info = (free_list_info_t*)vmm_get_virtual_address(pmm_convert_index_to_address(index));
        free_list_info_end_t* free_list_info_end = (free_list_info_end_t*)vmm_get_virtual_address(pmm_convert_index_to_address(index + page_count - 1));
        
        /* some numbers to characterize the segment */
        free_list_info->index_start = index;
        free_list_info->index_end = index + page_count - 1;
        free_list_info->page_count = page_count;

        /* define end and start */
        free_list_info->header.end = free_list_info_end;
        free_list_info_end->start = free_list_info;

        /* link to list */
        free_list_info->last = pmm_last_free_entry_free_list;
        free_list_info->next = NULL;
        if(pmm_last_free_entry_free_list != NULL) {
            pmm_last_free_entry_free_list->next = free_list_info;
        }

        /* update last link */
        pmm_last_free_entry_free_list = free_list_info;
    }
}

static void pmm_free_pages_index(uint64_t index, uint64_t page_count) {
    spinlock_acquire(&pmm_lock);

    pmm_add_page_to_free_list(index, page_count);
    bitmap_set_bits(pmm_bitmap, index, page_count, false);

    available_pages += page_count;
    used_pages -= page_count;

    if(pmm_first_free_page_index > index) {
        pmm_first_free_page_index = index;
    }

    spinlock_release(&pmm_lock);
}

static void pmm_free_page_index(uint64_t index) {
    pmm_free_pages_index(index, 1);
}

static void pmm_lock_pages_index(uint64_t index, uint64_t page_count) {
    bitmap_set_bits(pmm_bitmap, index, page_count, true);

    available_pages -= page_count;
    used_pages += page_count;
}

static void pmm_lock_page_index(uint64_t index) {
    pmm_lock_pages_index(index, 1);
}

static void pmm_unreserve_page_index(uint64_t index) {
    reserved_pages--;
    pmm_free_page_index(index);
}

static void pmm_unreserve_pages_index(uint64_t index, uint64_t page_count) {
    reserved_pages -= page_count;
    pmm_free_pages_index(index, page_count);
}

static void pmm_reserve_page_index(uint64_t index) {
    reserved_pages++;
    pmm_lock_page_index(index);
}

static void pmm_reserve_pages_index(uint64_t index, uint64_t page_count) {
    reserved_pages += page_count;
    pmm_lock_pages_index(index, page_count);
}

void pmm_init_bitmap(void* address, size_t size, bool lock_all) {
    bitmap_init(address, size, &pmm_bitmap, lock_all);
}

void* pmm_allocate_page(void) {
    spinlock_acquire(&pmm_lock);

    for(uint64_t index = pmm_first_free_page_index; index < highest_page_index; index++) {
        if(!bitmap_get_set_bit(pmm_bitmap, index, true)) {
            pmm_first_free_page_index = index;
            available_pages--;
            used_pages++;
            pmm_remove_pages_to_free_list(index, 1);
            spinlock_release(&pmm_lock);
            return (void*)(index * PAGE_SIZE);
        }
    }
    
    panic("not enough memory available");
    spinlock_release(&pmm_lock);

    return NULL; 
}

void* pmm_allocate_pages(uint64_t page_count) {
    spinlock_acquire(&pmm_lock);

    free_list_info_t* pmm_free_list = pmm_last_free_entry_free_list;
	while(pmm_free_list->page_count < page_count) {
        if(pmm_free_list->last != NULL) {
            pmm_free_list = pmm_free_list->last;
        }else{
            panic("not pmm_lock memory available");
            spinlock_release(&pmm_lock);
	        return NULL;
        }
    }

    uint64_t index = pmm_free_list->index_start;

    pmm_lock_pages_index(index, page_count);

    pmm_remove_pages_to_free_list(index, page_count);

    spinlock_release(&pmm_lock);

	return (void*)(index * PAGE_SIZE);
}

void pmm_free_page(void* address) {
    pmm_free_page_index(pmm_address_to_index(address));
}

void pmm_free_pages(void* address, uint64_t page_count) {
    pmm_free_pages_index(pmm_address_to_index(address), page_count);
}

void pmm_lock_page(void* address) {
    pmm_lock_page_index(pmm_address_to_index(address));
}

void pmm_lock_pages(void* address, uint64_t page_count) {
    pmm_lock_pages_index(pmm_address_to_index(address), page_count);
}

void pmm_unreserve_page(void* address) {
    pmm_unreserve_page_index(pmm_address_to_index(address));
}

void pmm_unreserve_pages(void* address, uint64_t page_count) {
    pmm_unreserve_pages_index(pmm_address_to_index(address), page_count);
}

void pmm_reserve_page(void* address) {
    pmm_reserve_page_index(pmm_address_to_index(address));
}

void pmm_reserve_pages(void* address, uint64_t page_count) {
    pmm_reserve_pages_index(pmm_address_to_index(address), page_count);
}
