#ifndef PB12_MEM_H
#define PB12_MEM_H

typedef struct S_PB12_MEM {
    int mem_size;    /* Memory size */
    char (*mem)[6];   /* Memory */
} PB12_MEM;


/**
    Initialize memory.

    @param PB12_MEM *mem - Memory.
    @param int memSize - Size of memory to allocate.
*/
int pb12InitMem(PB12_MEM *mem, int memSize);


/**
    Destroy memory.  This frees memory allocated for PBrain RAM.

    @param PB12_MEM *mem - Memory.
*/
int pb12DestroyMem(PB12_MEM *mem);

/**
    Converts sequence of characters to an integer.

    @param const char *source  String to retrieve integer from.
    @param int length  Number of characters to get integer from.

    @return int  Value of string.
*/
int pb12CharsToInt(const char *source, int length);


/**
    Writes integer out to character string.

    @param char *dest  Destination character array to write to.
    @param int length  Length of character string.
    @param int value  Value to write to character array.
*/
void pb12IntToChars(char *dest, int length, int value);


/**
    Get value at a memory address.

    @param PB12_MEM *mem - Memory.
    @param int address  Memory address.

    @return int  Value from memory address.
*/
int pb12GetMemValue(PB12_MEM *mem, int address);


/**
    Put value at a memory address.

    @param PB12_MEM *mem - Memory.
    @param int address  Memory address.
    @param int value  Value to place into memory.
*/
void pb12PutMemValue(PB12_MEM *mem, int address, int value);


/**
    Prints a range of memory values to the console.

    @param PB12_MEM *mem - Memory.
    @param int start  Starting address
    @param int length  Number of addresses to print
*/
void pb12PrintMem(PB12_MEM *mem, int start, int length);


/**
    Dump Memory to the terminal.

    @param PB12_MEM *mem - Memory.
*/
void pb12DumpMemory(PB12_MEM *mem);


/**
    Load data from file.  This data can be a program, but it can also simply
    be data.

    @param PB12_MEM *mem - Memory.
    @param int addr - Address to begin loading into.
    @param const char *filename  Path to program file to load.

    @return PB12_SUCCESS or PB12_FAILURE.
*/
int pb12Load(PB12_MEM *mem, int addr, const char *filename);

#endif /* PB12_MEM_H */
