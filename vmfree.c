#include "vm.h"
#include "vmlib.h"

void vmfree(void *ptr) {
    if (!ptr) {
        return;
    }

    struct block_header *current_header = (struct block_header *)((char *)ptr - sizeof(struct block_header));
    current_header->size_status &= ~VM_BUSY;
    
    struct block_header *start_header = current_header;
    size_t total_size = BLKSZ(current_header);

    struct block_footer *prev_footer = (struct block_footer *)((char *)start_header - sizeof(struct block_footer));
    while ((char *)prev_footer >= (char *)heapstart && !(prev_footer->size & VM_BUSY)) {
        struct block_header *prev_header = (struct block_header *)((char *)prev_footer - prev_footer->size + sizeof(struct block_footer));

        total_size += BLKSZ(prev_header);
        start_header = prev_header;
        prev_footer = (struct block_footer *)((char *)start_header - sizeof(struct block_footer));
    }

    struct block_header *next_header = (struct block_header *)((char *)current_header + BLKSZ(current_header));
    while (next_header->size_status != VM_ENDMARK && !(next_header->size_status & VM_BUSY)) {
        total_size += BLKSZ(next_header);

        next_header = (struct block_header *)((char *)next_header + BLKSZ(next_header));
    }

    start_header->size_status = (start_header->size_status & VM_PREVBUSY) | total_size;

    struct block_footer *end_footer = (struct block_footer *)((char *)start_header + total_size - sizeof(struct block_footer));
    end_footer->size = total_size;

    if (next_header->size_status != VM_ENDMARK) {
        next_header->size_status &= ~VM_PREVBUSY;
    }
}
