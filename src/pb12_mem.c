#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pb12.h"
#include "pb12_mem.h"

/**
    Initialize memory.

    @param PB12_MEM *mem - Memory.
    @param int memSize - Size of memory to allocate.
*/
int pb12InitMem(PB12_MEM *mem, int memSize) {
    mem->mem_size = memSize;
    mem->mem = (char (*)[6]) malloc(memSize * sizeof(char[6]));
    if (mem->mem == NULL)
        return PB12_FAILURE;
    memset(mem->mem, 'Z', memSize * sizeof(char[6]));
    return PB12_SUCCESS;
}

/**
    Destroy memory.  This frees memory allocated for PBrain RAM.

    @param PB12_MEM *mem - Memory.
*/
int pb12DestroyMem(PB12_MEM *mem) {
    free(mem->mem);
    mem->mem = NULL;
    return PB12_SUCCESS;
}


/**
    Converts sequence of characters to an integer.

    @param const char *source  String to retrieve integer from.
    @param int length  Number of characters to get integer from.

    @return int  Value of string.
*/
int pb12CharsToInt(const char *source, int length) {
    int i;
    int exponent = 10;
    int value = (int)(source[length-1] - '0');

    for (i=length-2; i>-1; i--) {
        value += (int)(source[i] - '0') * ((length-i-1) * exponent);
        exponent *= 10;
    }

    return value;
}


/**
    Writes integer out to character string.

    @param char *dest  Destination character array to write to.
    @param int length  Length of character string.
    @param int value  Value to write to character array.
*/
void pb12IntToChars(char *dest, int length, int value) {
    int i;
    for (i=length-1; i>-1; i--) {
        dest[i] = (char)(value % 10) + '0';
        value /= 10;
    }
}


/**
    Get value at a memory address.

    @param PB12_MEM *mem - Memory.
    @param int address  Memory address.

    @return int  Value from memory address.
*/
int pb12GetMemValue(PB12_MEM *mem, int address) {
    /* TODO: Bounds Checking */
    return pb12CharsToInt(&mem->mem[address][2], 4);
}


/**
    Put value at a memory address.

    @param PB12_MEM *mem - Memory.
    @param int address  Memory address.
    @param int value  Value to place into memory.
*/
void pb12PutMemValue(PB12_MEM *mem, int address, int value) {
    /* TODO: Bounds Checking */
    pb12IntToChars(&mem->mem[address][2], 4, value);
}


/**
    Prints a range of memory values to the console.

    @param PB12_MEM *mem - Memory.
    @param int start  Starting address
    @param int length  Number of addresses to print
*/
void pb12PrintMem(PB12_MEM *mem, int start, int length) {
    int i;

    printf("Values for memory addresses %d to %d:\n", start, start+length-1);
    for (i=start; i<start+length; i++) {
        printf("  %02d: %d\n", i, pb12GetMemValue(mem, i));
    }
}


/**
    Dump Memory to the terminal.

    @param PB12_MEM *mem - Memory.
*/
void pb12DumpMemory(PB12_MEM *mem) {
    int i;

    printf("MEMORY:\n");

    for (i=0; i<mem->mem_size; i++) {
        if (i % 10 == 0) {
            printf("  %03u:", i);
        }

        printf(" %.6s", mem->mem[i]);

        if (i % 10 == 9) {
            printf("\n");
        }
    }
}


/**
    Load data from file.  This data can be a program, but it can also simply
    be data.

    @param PB12_MEM *mem - Memory.
    @param int addr - Address to begin loading into.
    @param const char *filename  Path to program file to load.

    @return PB12_SUCCESS or PB12_FAILURE.
*/
int pb12Load(PB12_MEM *mem, int addr, const char *filename) {
    /* change this to use 'open' instead of 'fopen'? */

    char buffer[256];
    FILE *fp;
    int line = addr;

    fp = fopen(filename, "rt");
    if (!fp) {
        return PB12_FAILURE;
    }

    while (fgets(buffer, 256, fp)) {
        /* Valid program has 6 chars per line (more than that is ignored) */
        if (strlen(buffer) >= 6) {
            memcpy(mem->mem[line], buffer, 6);
            ++line;
        }
    }

    fclose(fp);

    return PB12_SUCCESS;
}
