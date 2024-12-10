#include "vm.h"
#include "vmlib.h"

static struct block_header* get_address(size_t size){
    struct block_header* current_block=heapstart;
    struct block_header* smallest_current=current_block;
    while (current_block->size_status!=VM_ENDMARK){
    int lsb = current_block->size_status & 1;
    if (lsb==0){
        if (current_block->size_status==size){
            return current_block;
        }} else if (BLKSZ(current_block) > size && BLKSZ(current_block) < BLKSZ(smallest_current)) {

            smallest_current=current_block;
        
    }
    current_block = (struct block_header *)((char *)current_block + BLKSZ(current_block));


    }
    if ((current_block->size_status & 1)==0){
        return current_block;
    }
    return NULL;
    }
    

void *vmalloc(size_t size) {
    size = ROUND_UP(size + sizeof(struct block_header), BLKALIGN);

    struct block_header *current = get_address(size);
    if (current == NULL) {
        return NULL; 
    }

    size_t current_block_size = BLKSZ(current);

    if (current_block_size >= size) {
        size_t remaining_size = current_block_size - size;

        if (remaining_size >= sizeof(struct block_header) + BLKALIGN) {
            struct block_header *new_block = (struct block_header *)((char *)current + size);
            new_block->size_status = remaining_size | VM_PREVBUSY;

            current->size_status = size | (current->size_status & VM_PREVBUSY);

            struct block_header *next_block = (struct block_header *)((char *)new_block + remaining_size);
            if (next_block->size_status != VM_ENDMARK) {
                next_block->size_status &= ~VM_PREVBUSY;
            }
        } else {
            struct block_header *next_block = (struct block_header *)((char *)current + current_block_size);
            if (next_block->size_status != VM_ENDMARK) {
                next_block->size_status |= VM_PREVBUSY;
            }
        }

        return (void *)((char *)current + sizeof(struct block_header));
    } else {
        return NULL;
    }
}
