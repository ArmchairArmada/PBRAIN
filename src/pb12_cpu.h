#ifndef PB12_CPU_H
#define PB12_CPU_H

struct S_PB12_HW;
struct S_PB12_MEM;

typedef struct S_PB12_CPU {
    short int p0, p1, p2, p3;       /* Pointer registers */
    short int pc;                   /* Program counter */
    short int sp;                   /* Stack pointer */
    int acc;                        /* Accumulator */
    int r0, r1, r2, r3;             /* General purpose registers */
    char psw[2];                    /* Process status word */
    char ir[6];                     /* Instruction register */
    int ic;                         /* Instruction counter */
    int bar;                        /* Base address register */
    int lr;                         /* Limit register */
    int ear;                        /* Effective address register */
} PB12_CPU;


/**
    Initialize the CPU.

    @param PB12_CPU *cpu - CPU to initialize.
*/
int pb12InitCpu(PB12_CPU *cpu);


/**
    Copies CPU registers.

    @param PB12_CPU *source - Source CPU data structure.
    @param PB12_CPU *dest - Destination CPU data structure.
*/
void pb12CopyCPU(PB12_CPU *source, PB12_CPU *dest);


/**
    Extract opcode from the instruction register.

    @param PB12_CPU *cpu  Virtual Machine state.

    @return int  Opcode value.
*/
int pb12GetOpcode(PB12_CPU *cpu);


/**
    Get the value of the operand in the instruction register.

    @param PB12_CPU *cpu - CPU.
    @param int opnum  Which operand to retrieve.

    @return int  Value of the operand.
*/
int pb12GetOperand(PB12_CPU *cpu, int opnum);


/**
    Get value of an instruction from the instruction register.

    @param PB12_CPU *cpu - CPU.

    @return int  Value that was stored in instruction.
*/
int pb12GetInstValue(PB12_CPU *cpu);


/**
    Set value of the process status word's comparison flag.

    @param PB12_CPU *cpu - CPU.
    @param int value  Boolean value to set flag to.
*/
void pb12SetPswCmp(PB12_CPU *cpu, int value);


/**
    Used with operands that get memory.  This ensures that the effective
    address remains within bounds of base address and limit register.

    @param PB12_CPU *cpu - CPU.
    @param PB12_MEM *mem - Memory in hardware.
    @param int addr - Relative address to read memory from.
*/
int pb12GetMemOp(PB12_CPU *cpu, struct S_PB12_MEM *mem, int addr);


/**
    Used with operands that write to memory.  This ensure that the effective
    address remains within the bounds of base address and limit register.

    @param PB12_CPU *cpu - CPU.
    @param PB12_MEM *mem - Memory in hardware.
    @param int addr - Relative address to write memory to.
    @param int value - Value to write to memory.
*/
void pb12PutMemOp(PB12_CPU *cpu, struct S_PB12_MEM *mem, int addr, int value);


/**
    Gets the register number from the instruction.

    @param PB12_CPU *cpu - CPU.
    @param int select - The operand to get the register number from.

    @return int - The number of the register in this operand.
*/
int pb12GetRegisterNumber(PB12_CPU *cpu, int select);


/**
    Get pointer to pointer register from instruction register.  This probably
    would have been simpler just to have used an array for registers.

    @param PB12_CPU *cpu - CPU.
    @param int select - The operand of the pointer register we want.

    @return short int * - The pointer to the pointer register.
*/
short int *pb12GetPointerRegister(PB12_CPU *cpu, int select);


/**
    Gets a pointer to a pointer register.

    @param PB12_CPU *cpu - CPU.
    @param int number - Pointer number.

    @return short int * - Pointer to pointer register.
*/
short int *pb12GetPtrReg(PB12_CPU *cpu, int number);


/**
    Get pointer to general register from instruction register.  Likewise, if
    an array of registers was used this would have been simpler.

    @param PB12_CPU *cpu - CPU.
    @param int select - The operand of the general register we want.

    @return int * - The pointer to the general register.
*/
int *pb12GetGeneralRegister(PB12_CPU *cpu, int select);


/**
    Gets a pointer to a general register.

    @param PB12_CPU *cpu - CPU.
    @param int number - Number of general register.

    @return int* - Pointer to general register.
*/
int *pb12GetGenReg(PB12_CPU *cpu, int number);


/**
    Dump CPU state to the terminal.

    @param PB12_CPU *cpu - CPU.
*/
void pb12DumpCPU(PB12_CPU *cpu);


/**
    Fetch next instruction.

    PB12_CPU *cpu - CPU.
*/
void pb12Fetch(PB12_CPU *cpu, struct S_PB12_MEM *mem);


/**
    Execute current instruction.

    PB12_CPU *cpu - CPU.

    @return int - PB12_SUCCESS, PB12_FAILURE, or PB12_TERMINATE
*/
int pb12Execute(PB12_CPU *cpu, struct S_PB12_HW *hw);

#endif /* PB12_CPU_H */
