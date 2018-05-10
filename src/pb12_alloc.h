#ifndef PB12_ALLOC_H
#define PB12_ALLOC_H

typedef struct S_PB12_MemBlock {
    struct S_PB12_MemBlock *next;
    int address;
    int length;
} PB12_MemBlock;


typedef struct S_PB12_MemList {
    PB12_MemBlock *head;
} PB12_MemList;


/**
    Initializes a memory list.

    @param PB12_MemBlock *mem_block - Memory block to initialize.
    @param int length - Size of the block of memory.
*/
void pb12AllocInit(PB12_MemList *mem_list, int length);


/**
    Prints a memory list.

    @param PB12_MemList *mem_list.
*/
void pb12AllocPrint(PB12_MemList *mem_list);


/**
    Pushes a memory block onto the beginning of the list.

    @param PB12_MemList *mem_list - List of memory blocks
    @param PB12_MemBlock *mem_block - Memory block
*/
void pb12AllocPush(PB12_MemList *mem_list, PB12_MemBlock *mem_block);


/**
    Removes a memory block from a memory list and updates pointers accordingly.

    @param PB12_MemList *mem_list - List of memory blocks
    @param PB12_MemBlock *prev - Previous memory block
    @param PB12_MemBlock *current - Memory block to remove, gets updated to next

    @param PB12_MemBlock* Memory block that has been removed
*/
PB12_MemBlock* pb12AllocRemove(PB12_MemList *mem_list, PB12_MemBlock *prev, PB12_MemBlock *current);


/**
    Splits a memory block and puts the remaining amount into a memory list.

    @param PB12_MemList *mem_list - List of memory blocks
    @param int length - Size of the block of memory.
*/
void pb12AllocSplit(PB12_MemList *mem_list, PB12_MemBlock* mem_block, int length);


/**
    Merges this block with neighboring blocks in the list.

    @param PB12_MemList *mem_list - List of memory blocks
    @param PB12_MemBlock *mem_block - Memory block
*/
void pb12AllocMerge(PB12_MemList *mem_list, PB12_MemBlock *mem_block);


/**
    First fit allocation scheme.

    @param PB12_MemList *mem_list - List of memory blocks
    @param int length - Required size of memory

    @return PB12_MemBlock* - Block of memory meeting requirements, else NULL
*/
PB12_MemBlock* pb12AllocFirstFit(PB12_MemList *mem_list, int length);


/**
    Best fit allocation scheme.

    @param PB12_MemList *mem_list - List of memory blocks
    @param int length - Required size of memory

    @return PB12_MemBlock* - Block of memory meeting requirements, else NULL
*/
PB12_MemBlock* pb12AllocBestFit(PB12_MemList *mem_list, int length);


/**
    Worst fit allocation scheme.

    @param PB12_MemList *mem_list - List of memory blocks
    @param int length - Required size of memory

    @return PB12_MemBlock* - Block of memory meeting requirements, else NULL
*/
PB12_MemBlock* pb12AllocWorstFit(PB12_MemList *mem_list, int length);

#endif /* PB12_ALLOC_H */
