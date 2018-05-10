#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <malloc.h>
#include "pb12.h"
#include "pb12_alloc.h"

/**
    Initializes a memory list.

    @param PB12_MemBlock *mem_block - Memory block to initialize.
    @param int length - Size of the block of memory.
*/
void pb12AllocInit(PB12_MemList *mem_list, int length) {
    PB12_MemBlock* mem_block;

    mem_block = (PB12_MemBlock*) malloc(sizeof(PB12_MemBlock));
    mem_block->next = NULL;
    mem_block->address = 0;
    mem_block->length = length;
    mem_list->head = mem_block;
}


/**
    Prints a memory list.

    @param PB12_MemList *mem_list.
*/
void pb12AllocPrint(PB12_MemList *mem_list) {
    PB12_MemBlock *current;

    current = mem_list->head;
    while (current) {
        printf("  [%d-%d:%d]\n",
               current->address, current->address + current->length - 1,
               current->length);
        current = current->next;
    }
}


/**
    Pushes a memory block onto the beginning of the list.

    @param PB12_MemList *mem_list - List of memory blocks
    @param PB12_MemBlock *mem_block - Memory block
*/
void pb12AllocPush(PB12_MemList *mem_list, PB12_MemBlock *mem_block) {
    mem_block->next = mem_list->head;
    mem_list->head = mem_block;
}


/**
    Removes a memory block from a memory list and updates pointers accordingly.

    @param PB12_MemList *mem_list - List of memory blocks
    @param PB12_MemBlock *prev - Previous memory block
    @param PB12_MemBlock *current - Memory block to remove, gets updated to next

    @param PB12_MemBlock* Memory block that has been removed
*/
PB12_MemBlock* pb12AllocRemove(PB12_MemList *mem_list, PB12_MemBlock *prev, PB12_MemBlock *current) {
    PB12_MemBlock* tmp;

    tmp = current;
    if (prev == NULL) {
        mem_list->head = current->next;
    }
    else {
        prev->next = current->next;
    }

    return tmp;
}


/**
    Splits a memory block and puts the remaining amount into a memory list.

    If the memory block will be completely filled up there will not be a
    remainder block added to the list.

    @param PB12_MemList *mem_list - List of memory blocks
    @param int length - Size of the block of memory.
*/
void pb12AllocSplit(PB12_MemList *mem_list, PB12_MemBlock* mem_block, int length) {
    int difference;
    PB12_MemBlock* new_block;

    difference = mem_block->length - length;

    if (difference == 0)
        return;

    if (pb12Options & PB12_OPT_VERBOSE) {
        printf("Splitting memory [%d-%d:%d] into ",
               mem_block->address, mem_block->address + mem_block->length - 1, mem_block->length);
    }

    mem_block->length = length;

    new_block = (PB12_MemBlock*) malloc(sizeof(PB12_MemBlock));
    new_block->address = mem_block->address + length;
    new_block->length = difference;
    pb12AllocPush(mem_list, new_block);

    if (pb12Options & PB12_OPT_VERBOSE) {
        printf("[%d-%d:%d] and [%d-%d:%d].\n",
               mem_block->address, mem_block->address + mem_block->length - 1, mem_block->length,
               new_block->address, new_block->address + new_block->length - 1, new_block->length);
    }

    if (pb12Options & PB12_OPT_VERBOSE) {
        printf("Memory allocated.  Memory list now:\n");
        pb12AllocPrint(mem_list);
    }
}


/**
    Merges this block with neighboring blocks in the list.

    @param PB12_MemList *mem_list - List of memory blocks
    @param PB12_MemBlock *mem_block - Memory block
*/
void pb12AllocMerge(PB12_MemList *mem_list, PB12_MemBlock *mem_block) {
    PB12_MemBlock *prev;
    PB12_MemBlock *current;
    PB12_MemBlock *tmp;
    int end_addr;

    if (mem_list->head == NULL) {
        mem_list->head = mem_block;
        return;
    }

    end_addr = mem_block->address + mem_block->length;

    prev = NULL;
    current = mem_list->head;
    while (current != NULL) {
        /* TODO: Make sure merging first block doesn't cause any problems */

        if (current->address == end_addr) {
            /* Merge mem_block with one after it */
            if (pb12Options & PB12_OPT_VERBOSE) {
                printf("Merging memory [%d-%d:%d] with [%d-%d:%d] making ",
                       mem_block->address, mem_block->address + mem_block->length - 1, mem_block->length,
                       current->address, current->address + current->length - 1, current->length);
            }

            mem_block->length += current->length;
            tmp = pb12AllocRemove(mem_list, prev, current);
            current = current->next;
            free(tmp);

            if (pb12Options & PB12_OPT_VERBOSE) {
                printf("[%d-%d:%d].\n", mem_block->address,
                       mem_block->address + mem_block->length - 1, mem_block->length);
            }
        }
        else if (current->address + current->length == mem_block->address) {
            /* Merge mem_block with one before it */
            if (pb12Options & PB12_OPT_VERBOSE) {
                printf("Merging memory [%d-%d:%d] with [%d-%d:%d] making ",
                       mem_block->address, mem_block->address + mem_block->length - 1, mem_block->length,
                       current->address, current->address + current->length - 1, current->length);
            }

            mem_block->address = current->address;
            mem_block->length += current->length;
            tmp = pb12AllocRemove(mem_list, prev, current);
            current = current->next;
            free(tmp);

            if (pb12Options & PB12_OPT_VERBOSE) {
                printf("[%d-%d:%d].\n", mem_block->address,
                       mem_block->address + mem_block->length - 1, mem_block->length);
            }
        }
        else {
            prev = current;
            current = current->next;
        }
    }

    mem_block->next = mem_list->head;
    mem_list->head = mem_block;

    if (pb12Options & PB12_OPT_VERBOSE) {
        printf("Memory deallocated.  Memory list now:\n");
        pb12AllocPrint(mem_list);
    }
}


/**
    First fit allocation scheme.

    @param PB12_MemList *mem_list - List of memory blocks
    @param int length - Required size of memory

    @return PB12_MemBlock* - Block of memory meeting requirements, else NULL
*/
PB12_MemBlock* pb12AllocFirstFit(PB12_MemList *mem_list, int length) {
    PB12_MemBlock* mem_block;
    PB12_MemBlock* prev;
    PB12_MemBlock* current;

    mem_block = NULL;
    prev = NULL;
    current = mem_list->head;
    while (current != NULL) {
        if (current->length >= length) {
            mem_block = pb12AllocRemove(mem_list, prev, current);
            pb12AllocSplit(mem_list, mem_block, length);
            mem_block->next = NULL;
            break;
        }
        prev = current;
        current = current->next;
    }

    return mem_block;
}


/**
    Best fit allocation scheme.

    @param PB12_MemList *mem_list - List of memory blocks
    @param int length - Required size of memory

    @return PB12_MemBlock* - Block of memory meeting requirements, else NULL
*/
PB12_MemBlock* pb12AllocBestFit(PB12_MemList *mem_list, int length) {
    PB12_MemBlock* mem_prev;
    PB12_MemBlock* mem_block;
    PB12_MemBlock* prev;
    PB12_MemBlock* current;
    int difference;
    int best;

    best = INT_MAX;

    mem_block = NULL;
    prev = NULL;
    current = mem_list->head;
    while (current != NULL) {
        difference = current->length - length;

        if (difference >= 0 && difference < best) {
            best = difference;
            mem_prev = prev;
            mem_block = current;
        }

        prev = current;
        current = current->next;
    }

    if (mem_block != NULL) {
        mem_block = pb12AllocRemove(mem_list, mem_prev, mem_block);
        pb12AllocSplit(mem_list, mem_block, length);
        mem_block->next = NULL;
    }

    return mem_block;
}

/**
    Worst fit allocation scheme.

    @param PB12_MemList *mem_list - List of memory blocks
    @param int length - Required size of memory

    @return PB12_MemBlock* - Block of memory meeting requirements, else NULL
*/
PB12_MemBlock* pb12AllocWorstFit(PB12_MemList *mem_list, int length) {
    PB12_MemBlock* mem_prev;
    PB12_MemBlock* mem_block;
    PB12_MemBlock* prev;
    PB12_MemBlock* current;
    int difference;
    int worst;

    worst = 0;

    mem_block = NULL;
    prev = NULL;
    current = mem_list->head;
    while (current != NULL) {
        difference = current->length - length;

        if (difference >= 0 && difference > worst) {
            worst = difference;
            mem_prev = prev;
            mem_block = current;
        }

        prev = current;
        current = current->next;
    }

    if (mem_block != NULL) {
        mem_block = pb12AllocRemove(mem_list, mem_prev, mem_block);
        pb12AllocSplit(mem_list, mem_block, length);
        mem_block->next = NULL;
    }

    return mem_block;
}
