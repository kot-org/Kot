#include <stdint.h>
#include <stddef.h>
#include <lib/log.h>
#include <impl/vmm.h>
#include <lib/lock.h>
#include <lib/math.h>
#include <impl/panic.h>
#include <global/pmm.h>
#include <global/heap.h>

struct heap_segment_header{
    bool is_free;
    size_t length;
    struct heap_segment_header* next;
    struct heap_segment_header* last;
    uint32_t signature;
}__attribute__((aligned(0x10)));

static void* heap_end = 0;
static void* heap_low = 0;
static struct heap_segment_header* last_segment = NULL;
static struct heap_segment_header* main_segment = NULL;
static size_t total_size;
static size_t free_size;
static size_t used_size;
static spinlock_t lock = {};


static struct heap_segment_header* get_heap_segment_header(void* address) {
    return (struct heap_segment_header*)(void*)((uint64_t)address - sizeof(struct heap_segment_header));
}

static void merge_this_to_next(struct heap_segment_header* header) {
    // merge this segment into the last segment
    struct heap_segment_header* header_next = header->next;
    header_next->length += header->length + sizeof(struct heap_segment_header);
    header_next->last = header->last;
    header->last->next = header_next;

    memset(header, 0, sizeof(struct heap_segment_header));
}

static void merge_last_to_this(struct heap_segment_header* header) {
    // merge this segment into the next segment

    struct heap_segment_header* header_last = header->last;
    header->length += header_last->length + sizeof(struct heap_segment_header);
    header->last = header_last->last;
    header->last->next = header;

    memset(header_last, 0, sizeof(struct heap_segment_header));
}

static void merge_last_and_this_to_next(struct heap_segment_header* header) {
    // merge this segment into the last segment
    merge_last_to_this(header);
    merge_this_to_next(header);
}

static struct heap_segment_header* split_segment(struct heap_segment_header* segment, size_t size) {
    if(segment->length > size + sizeof(struct heap_segment_header)) {
        struct heap_segment_header* new_segment = (struct heap_segment_header*)(void*)((uint64_t)segment + segment->length - size);
        memset(new_segment, 0, sizeof(struct heap_segment_header));
        new_segment->is_free = true;           
        new_segment->signature = 0xff;       
        new_segment->length = size;
        new_segment->next = segment;
        new_segment->last = segment->last;

        if(segment->next == NULL) {
            last_segment = segment;
        }

        if(segment->last != NULL) {
            segment->last->next = new_segment;
        }
        segment->last = new_segment;
        segment->length = segment->length - (size + sizeof(struct heap_segment_header));  
        return new_segment;      
    }
    return NULL;
}

static void expand_heap(size_t length) {
    length += sizeof(struct heap_segment_header);
    if(length % PAGE_SIZE) {
        length -= length % PAGE_SIZE;
        length += PAGE_SIZE;
    }

    size_t page_count = DIV_ROUNDUP(length, PAGE_SIZE);


    for (size_t i = 0; i < page_count; i++) {
        void* new_physical_address = pmm_allocate_page();
        heap_end = (void*)((uint64_t)heap_end - (uint64_t)PAGE_SIZE);
        if((uintptr_t)heap_end < (uintptr_t)heap_low) {
            panic("heap used all allocated virtual size");
        }
        vmm_map(kernel_space, (memory_range_t) {heap_end, PAGE_SIZE}, (memory_range_t) {new_physical_address, PAGE_SIZE}, MEMORY_FLAG_READABLE | MEMORY_FLAG_WRITABLE);
    }

    struct heap_segment_header* new_segment = (struct heap_segment_header*)heap_end;

    if(last_segment != NULL && last_segment->is_free && last_segment->last != NULL) {
        uint64_t size = last_segment->length + length;
        new_segment->signature = 0xff;
        new_segment->length = size - sizeof(struct heap_segment_header);
        new_segment->is_free = true;
        new_segment->last = last_segment->last;
        new_segment->last->next = new_segment;
        new_segment->next = NULL;
        last_segment = new_segment;    
    }else{
        new_segment->signature = 0xff;
        new_segment->length = length - sizeof(struct heap_segment_header);
        new_segment->is_free = true;
        new_segment->last = last_segment;
        new_segment->next = NULL;
        if(last_segment != NULL) {
            last_segment->next = new_segment;
        }
        last_segment = new_segment;        
    }  
    
    total_size += length + sizeof(struct heap_segment_header);     
    free_size += length + sizeof(struct heap_segment_header);     
}

void heap_init(void* heap_address_high, void* heap_address_low) {
    heap_end = heap_address_high;
    heap_low = heap_address_low;
    void* new_physical_address = pmm_allocate_page();
    heap_end = (void*)((uint64_t)heap_end - PAGE_SIZE);
    vmm_map(kernel_space, (memory_range_t) {heap_end, PAGE_SIZE}, (memory_range_t) {new_physical_address, PAGE_SIZE}, MEMORY_FLAG_READABLE | MEMORY_FLAG_WRITABLE);
    main_segment = (struct heap_segment_header*)((uint64_t)heap_end + ((uint64_t)PAGE_SIZE - sizeof(struct heap_segment_header)));
    main_segment->signature = 0xff;
    main_segment->length = 0;
    main_segment->is_free = false;
    main_segment->last = NULL;
    main_segment->next = (struct heap_segment_header*)((uint64_t)main_segment - (uint64_t)PAGE_SIZE + sizeof(struct heap_segment_header));

    main_segment->next->is_free = true;
    main_segment->next->length = (uint64_t)PAGE_SIZE - sizeof(struct heap_segment_header) - sizeof(struct heap_segment_header); /* remove twice because we have the main and new header in the same page */
    main_segment->next->signature = 0xff;
    main_segment->next->last = main_segment;
    main_segment->next->next = NULL;
    last_segment = main_segment->next;  

    total_size += PAGE_SIZE;     
    free_size += PAGE_SIZE;  
}

void* calloc(size_t number, size_t size) {
    void* address = malloc(number * size);
    memset(address, 0, number * size);
    return address;
}

void* malloc(size_t size) {    
    if(size == 0) return NULL;

    if(size % 0x10 > 0) { // it is not a multiple of 0x10
        size -= (size % 0x10);
        size += 0x10;
    }

    spinlock_acquire(&lock);
    struct heap_segment_header* current_seg = (struct heap_segment_header*)main_segment;
    uint64_t size_with_header = size + sizeof(struct heap_segment_header);
    while(current_seg) {
        if(current_seg->is_free) {
            if(current_seg->length > size_with_header) {
                // split this segment in two 
                current_seg = split_segment(current_seg, size);
                current_seg->is_free = false;
                used_size += current_seg->length + sizeof(struct heap_segment_header);
                free_size -= current_seg->length + sizeof(struct heap_segment_header);
                spinlock_release(&lock);
                return (void*)((uint64_t)current_seg + sizeof(struct heap_segment_header));
            }else if(current_seg->length == size) {
                current_seg->is_free = false;
                used_size += current_seg->length + sizeof(struct heap_segment_header);
                free_size -= current_seg->length + sizeof(struct heap_segment_header);
                spinlock_release(&lock);
                return (void*)((uint64_t)current_seg + sizeof(struct heap_segment_header));
            }
        }
        current_seg = current_seg->next;
    }
    
    expand_heap(size);
    spinlock_release(&lock);
    return malloc(size);
}

void free(void* address) {
    if(address != NULL) {
        spinlock_acquire(&lock);
        struct heap_segment_header* header = (struct heap_segment_header*)(void*)((uint64_t)address - sizeof(struct heap_segment_header));
        header->is_free = true;
        free_size += header->length + sizeof(struct heap_segment_header);
        used_size -= header->length + sizeof(struct heap_segment_header);

        if(header->next != NULL && header->last != NULL) {
            if(header->next->is_free && header->last->is_free) {
                // merge this segment and next segment into the last segment
                merge_last_and_this_to_next(header);
                spinlock_release(&lock);
                return;
            }
        }

        if(header->last != NULL) {
            if(header->last->is_free) {
                // merge this segment into the last segment
                merge_last_to_this(header);
                spinlock_release(&lock);  
                return;  
            }         
        }
        
        if(header->next != NULL) {
            if(header->next->is_free) {
                // merge this segment into the next segment
                merge_this_to_next(header);
                spinlock_release(&lock);
                return; 
            }
        }
        spinlock_release(&lock);
    }
}

void* realloc(void* buffer, size_t size) {
    void* new_buffer = malloc(size);
    if(new_buffer == NULL) {
        return NULL;
    }
    if(buffer != NULL) {
        size_t old_size = get_heap_segment_header(buffer)->length;
        if (size < old_size) {
            old_size = size;
        }
        memcpy(new_buffer, buffer, old_size);
        free(buffer);
    }
    return new_buffer;
}