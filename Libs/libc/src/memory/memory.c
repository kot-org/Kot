#include <kot/memory.h>

void memset(void* start, uint8_t value, uint64_t num){
    for (uint64_t i = 0; i < num; i++){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}

void memcpy(void* destination, void* source, uint64_t num){
    long d0, d1, d2; 
    __asm__ volatile(
            "rep ; movsq\n\t movq %4,%%rcx\n\t""rep ; movsb\n\t": "=&c" (d0),
            "=&D" (d1),
            "=&S" (d2): "0" (num >> 3), 
            "g" (num & 7), 
            "1" (destination),
            "2" (source): "memory"
    );  
}

int memcmp(const void *aptr, const void *bptr, size_t n){
	const unsigned char *a = (const unsigned char*)aptr, *b = (const unsigned char*)bptr;
	for (size_t i = 0; i < n; i++) {
		if (a[i] < b[i])
			return -1;
		else if (a[i] > b[i])
			return 1;
	}
	return 0;
}