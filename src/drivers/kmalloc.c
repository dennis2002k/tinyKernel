#include <types.h>

static uint32_t heapStart;
static uint32_t heapSize;
static bool kmallocInitialized = false;


void kmallocInit(uint32_t initialHeapSize) {
    heapStart = KERNEL_MALLOC
    heapSize = 0;
    kmallocInitialized = true;

    changeHeapSize(initialHeapSize);
}

void changeHeapSize(int newSize) {
    
}