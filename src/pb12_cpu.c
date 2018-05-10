#include <stdio.h>
#include <string.h>
#include "pb12.h"
#include "pb12_cpu.h"
#include "pb12_hw.h"
#include "pb12_mem.h"
#include "pb12_inst.h"
#include "pb12_strings.h"


/**
    Initialize the CPU.

    @param PB12_CPU *cpu - CPU to initialize.
*/
int pb12InitCpu(PB12_CPU *cpu) {
    memset(cpu, 0, sizeof(PB12_CPU));
    cpu->psw[0] = 'T';
    cpu->psw[1] = ' ';
    return PB12_SUCCESS;
}


/**
    Copies CPU registers.

    @param PB12_CPU *source - Source CPU data structure.
    @param PB12_CPU *dest - Destination CPU data structure.
*/
void pb12CopyCPU(PB12_CPU *source, PB12_CPU *dest) {
    /* Not sure if this would be considered cheating: */
    memcpy(dest, source, sizeof(PB12_CPU));

    /*
    dest->psw[0] = source->psw[0];
    dest->psw[1] = source->psw[1];

    dest->ir[0] = source->ir[0];
    dest->ir[1] = source->ir[1];
    dest->ir[2] = source->ir[2];
    dest->ir[3] = source->ir[3];
    dest->ir[4] = source->ir[4];
    dest->ir[5] = source->ir[5];

    dest->acc = source->acc;
    dest->sp = source->sp;
    dest->ic = source->ic;
    dest->bar = source->bar;
    dest->lr = source->lr;
    dest->ear = source->ear;

    dest->r0 = source->r0;
    dest->r1 = source->r1;
    dest->r2 = source->r2;
    dest->r3 = source->r3;

    dest->p0 = source->p0;
    dest->p1 = source->p1;
    dest->p2 = source->p2;
    dest->p3 = source->p3;

    dest->pc = source->pc;
    */
}


/**
    Extract opcode from the instruction register.

    @param PB12_CPU *cpu - CPU to get opcode from.

    @return int  Opcode value.
*/
int pb12GetOpcode(PB12_CPU *cpu) {
    return pb12CharsToInt(cpu->ir, 2);
}


/**
    Get the value of the operand in the instruction register.

    @param PB12_CPU *cpu - CPU.
    @param int opnum  Which operand to retrieve.

    @return int  Value of the operand.
*/
int pb12GetOperand(PB12_CPU *cpu, int opnum) {
    return pb12CharsToInt(&cpu->ir[2 + opnum * 2], 2);
}


/**
    Get value of an instruction from the instruction register.

    @param PB12_CPU *cpu - CPU.

    @return int  Value that was stored in instruction.
*/
int pb12GetInstValue(PB12_CPU *cpu) {
    return pb12CharsToInt(&cpu->ir[2], 4);
}


/**
    Set value of the process status word's comparison flag.

    @param PB12_CPU *cpu - CPU.
    @param int value  Boolean value to set flag to.
*/
void pb12SetPswCmp(PB12_CPU *cpu, int value) {
    if (value) {
        cpu->psw[0] = 'T';
    }
    else {
        cpu->psw[0] = 'F';
    }
}


/**
    Used with operands that get memory.  This ensures that the effective
    address remains within bounds of base address and limit register.

    @param PB12_CPU *cpu - CPU.
    @param PB12_MEM *mem - Memory in hardware.
    @param int addr - Relative address to read memory from.
*/
int pb12GetMemOp(PB12_CPU *cpu, PB12_MEM *mem, int addr) {
    cpu->ear = cpu->bar + addr;
    if (cpu->ear < cpu->bar || cpu->ear > cpu->lr) {
        pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_ADDRESS_RANGE],
                     cpu->ear, cpu->bar, cpu->lr);
    }
    return pb12GetMemValue(mem, cpu->ear);
}


/**
    Used with operands that write to memory.  This ensure that the effective
    address remains within the bounds of base address and limit register.

    @param PB12_CPU *cpu - CPU.
    @param PB12_MEM *mem - Memory in hardware.
    @param int addr - Relative address to write memory to.
    @param int value - Value to write to memory.
*/
void pb12PutMemOp(PB12_CPU *cpu, PB12_MEM *mem, int addr, int value) {
    cpu->ear = cpu->bar + addr;
    if (cpu->ear < cpu->bar || cpu->ear > cpu->lr) {
        pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_ADDRESS_RANGE],
                     cpu->ear, cpu->bar, cpu->lr);
    }
    pb12PutMemValue(mem, cpu->ear, value);
}


/**
    Gets the register number from the instruction.

    @param PB12_CPU *cpu - CPU.
    @param int select - The operand to get the register number from.

    @return int - The number of the register in this operand.
*/
int pb12GetRegisterNumber(PB12_CPU *cpu, int select) {
    return (int)(cpu->ir[3 + select * 2] - '0');
}


/**
    Get pointer to pointer register from instruction register.  This probably
    would have been simpler just to have used an array for registers.

    @param PB12_CPU *cpu - CPU.
    @param int select - The operand of the pointer register we want.

    @return short int * - The pointer to the pointer register.
*/
short int *pb12GetPointerRegister(PB12_CPU *cpu, int select) {
    int num;
    int offset;
    offset = select*2;
    if (cpu->ir[2 + offset] != 'P') return NULL;
    num = (int)(cpu->ir[3 + offset] - '0');
    return pb12GetPtrReg(cpu, num);
}


/**
    Gets a pointer to a pointer register.

    @param PB12_CPU *cpu - CPU.
    @param int number - Pointer number.

    @return short int * - Pointer to pointer register.
*/
short int *pb12GetPtrReg(PB12_CPU *cpu, int number) {
    switch (number) {
    case 0:
        return &cpu->p0;
        break;
    case 1:
        return &cpu->p1;
        break;
    case 2:
        return &cpu->p2;
        break;
    case 3:
        return &cpu->p3;
        break;
    default:
        return NULL;
    }
}


/**
    Get pointer to general register from instruction register.  Likewise, if
    an array of registers was used this would have been simpler.

    @param PB12_CPU *cpu - CPU.
    @param int select - The operand of the general register we want.

    @return int * - The pointer to the general register.
*/
int *pb12GetGeneralRegister(PB12_CPU *cpu, int select) {
    int num;
    int offset;
    offset = select*2;
    if (cpu->ir[2 + offset] != 'R') return NULL;
    num = (int)(cpu->ir[3 + offset] - '0');
    return pb12GetGenReg(cpu, num);
}


/**
    Gets a pointer to a general register.

    @param PB12_CPU *cpu - CPU.
    @param int number - Number of general register.

    @return int* - Pointer to general register.
*/
int *pb12GetGenReg(PB12_CPU *cpu, int number) {
    switch (number) {
    case 0:
        return &cpu->r0;
        break;
    case 1:
        return &cpu->r1;
        break;
    case 2:
        return &cpu->r2;
        break;
    case 3:
        return &cpu->r3;
        break;
    default:
        return NULL;
    }
}


/**
    Dump CPU state to the terminal.

    @param PB12_CPU *cpu - CPU.
*/
void pb12DumpCPU(PB12_CPU *cpu) {
    printf("CPU:\n");

    printf("  PC=%02u  SP=%02u  IC=%02u  ACC=%04u  BAR=%03u  LR=%03u  EAR=%03u  PSW=%c%c  IR=%.6s\n",
           cpu->pc, cpu->sp, cpu->ic, cpu->acc, cpu->bar, cpu->lr, cpu->ear,
           cpu->psw[0], cpu->psw[1], cpu->ir);

    printf("\nREGISTERS:\n");
    printf("  P0=%02u    P1=%02u    P2=%02u    P3=%02u\n",
           cpu->p0, cpu->p1, cpu->p2, cpu->p3);

    printf("  R0=%04u  R1=%04u  R2=%04u  R3=%04u\n\n",
           cpu->r0, cpu->r1, cpu->r2, cpu->r3);
}


/**
    Fetch next instruction.

    PB12_CPU *cpu - CPU.
*/
void pb12Fetch(PB12_CPU *cpu, PB12_MEM *mem) {
    cpu->ear = cpu->bar + cpu->pc;
    memcpy(cpu->ir, mem->mem[cpu->ear], 6);
    --cpu->ic;
}


/**
    Execute current instruction.

    PB12_CPU *cpu - CPU.

    @return int  PB12_SUCCESS, PB12_FAILURE, or PB12_TERMINATE
*/
int pb12Execute(PB12_CPU *cpu, PB12_HW *hw) {
    /* All of these are static for assumed slight performance reasons. */
    static int opcode;
    static short int *ptrReg1;
    static short int *ptrReg2;
    static int *genReg1;
    static int *genReg2;
    static int addr;
    PB12_MEM *mem;

    mem = &hw->mem;

    opcode = pb12GetOpcode(cpu);

    /* TODO: Figure out what I want to do about error messages */
    if (pb12Options & PB12_OPT_VERBOSE) {
        printf("PC=%02d IC=%02d IR=%.6s: ", cpu->pc, cpu->ic, cpu->ir);
        if (opcode < PB12_MAX_OPCODE)
            printf("%s.\n", pb12InstDesc[opcode]);
        else if (opcode == PB12_HLT)
            printf("%s.\n", pb12InstDesc[PB12_MAX_OPCODE]);
        else
            printf("INVALID OPCODE\n");
    }

    if (opcode != PB12_HLT)
        ++cpu->pc;

    switch (opcode) {
    case PB12_LOD_PTR_I:    /* 0 - Load Pointer Immediate */
        ptrReg1 = pb12GetPointerRegister(cpu, 0);
        if (!ptrReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_PTR_1]);
            return PB12_FAILURE;
        }
        *ptrReg1 = pb12GetOperand(cpu, 1);
        break;

    case PB12_ADD_PTR_I:    /* 1 - Add to Pointer Immediate */
        ptrReg1 = pb12GetPointerRegister(cpu, 0);
        if (!ptrReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_PTR_1]);
            return PB12_FAILURE;
        }
        *ptrReg1 += pb12GetOperand(cpu, 1);
        if (*ptrReg1 > 99)
            *ptrReg1 -= 100;
        break;

    case PB12_SUB_PTR_I:    /* 2 - Subtract from Pointer Immediate */
        ptrReg1 = pb12GetPointerRegister(cpu, 0);
        if (!ptrReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_PTR_1]);
            return PB12_FAILURE;
        }
        *ptrReg1 -= pb12GetOperand(cpu, 1);
        if (*ptrReg1 < 0)
            *ptrReg1 += 100;
        break;

    case PB12_LOD_ACC_I:    /* 3 - Load Accumulator Immediate */
        cpu->acc = pb12GetInstValue(cpu);
        break;

    case PB12_LOD_ACC_R:    /* 4 - Load Accumulator Register Addressing */
        ptrReg1 = pb12GetPointerRegister(cpu, 0);
        if (!ptrReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_PTR_1]);
            return PB12_FAILURE;
        }
        cpu->acc = pb12GetMemOp(cpu, mem, *ptrReg1);
        break;

    case PB12_LOD_ACC_D:    /* 5 - Load Accumulator Direct Addressing */
        addr = pb12GetOperand(cpu, 0);
        if (addr >= mem->mem_size) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_ADDR]);
            return PB12_FAILURE;
        }
        cpu->acc = pb12GetMemOp(cpu, mem, addr);
        break;

    case PB12_STO_ACC_R:    /* 6 - Store Accumulator Register Addressing */
        ptrReg1 = pb12GetPointerRegister(cpu, 0);
        if (!ptrReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_PTR_1]);
            return PB12_FAILURE;
        }
        pb12PutMemOp(cpu, mem, *ptrReg1, cpu->acc);
        break;

    case PB12_STO_ACC_D:    /* 7 - Store Accumulator Direct Addressing */
        addr = pb12GetOperand(cpu, 0);
        if (addr >= mem->mem_size) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_ADDR]);
            return PB12_FAILURE;
        }
        pb12PutMemOp(cpu, mem, addr, cpu->acc);
        break;

    case PB12_STO_REG_R:    /* 8 - Store Register to Memory: Register Addressing */
        genReg1 = pb12GetGeneralRegister(cpu, 0);
        if (!genReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_1]);
            return PB12_FAILURE;
        }
        ptrReg2 = pb12GetPointerRegister(cpu, 1);
        if (!ptrReg2) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_PTR_2]);
            return PB12_FAILURE;
        }
        pb12PutMemOp(cpu, mem, *ptrReg2, *genReg1);
        break;

    case PB12_STO_REG_D:    /* 9 - Store Register to Memory: Direct Addressing */
        genReg1 = pb12GetGeneralRegister(cpu, 0);
        if (!genReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_1]);
            return PB12_FAILURE;
        }
        addr = pb12GetOperand(cpu, 1);
        if (addr >= mem->mem_size) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_ADDR]);
            return PB12_FAILURE;
        }
        pb12PutMemOp(cpu, mem, addr, *genReg1);
        break;

    case PB12_LOD_REG_R:    /* 10 - Load Register from Memory: Register Addressing */
        genReg1 = pb12GetGeneralRegister(cpu, 0);
        if (!genReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_1]);
            return PB12_FAILURE;
        }
        ptrReg2 = pb12GetPointerRegister(cpu, 1);
        if (!ptrReg2) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_PTR_2]);
            return PB12_FAILURE;
        }
        *genReg1 = pb12GetMemOp(cpu, mem, *ptrReg2);
        break;

    case PB12_LOD_REG_D:    /* 11 - Load Register from Memory: Direct Addressing */
        genReg1 = pb12GetGeneralRegister(cpu, 0);
        if (!genReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_1]);
            return PB12_FAILURE;
        }
        addr = pb12GetOperand(cpu, 1);
        if (addr >= mem->mem_size) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_ADDR]);
            return PB12_FAILURE;
        }
        *genReg1 = pb12GetMemOp(cpu, mem, addr);
        break;

    case PB12_LOD_REG_R0_I: /* 12 - Load Register R0 Immediate */
        cpu->r0 = pb12GetInstValue(cpu);
        break;

    case PB12_TRA_REG_REG:  /* 13 - Register to Register Transfer */
        genReg1 = pb12GetGeneralRegister(cpu, 0);
        if (!genReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_1]);
            return PB12_FAILURE;
        }
        genReg2 = pb12GetGeneralRegister(cpu, 1);
        if (!genReg2) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_2]);
            return PB12_FAILURE;
        }
        *genReg1 = *genReg2;
        break;

    case PB12_LOD_ACC_REG:  /* 14 - Load Accumulator from Register */
        genReg1 = pb12GetGeneralRegister(cpu, 0);
        if (!genReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_1]);
            return PB12_FAILURE;
        }
        cpu->acc = *genReg1;
        break;

    case PB12_LOD_REG_ACC:  /* 15 - Load Register from Accumulator */
        genReg1 = pb12GetGeneralRegister(cpu, 0);
        if (!genReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_1]);
            return PB12_FAILURE;
        }
        *genReg1 = cpu->acc;
        break;

    case PB12_ADD_ACC_I:    /* 16 - Add Accumulator Immediate */
        cpu->acc += pb12GetInstValue(cpu);
        if (cpu->acc > 9999)
            cpu->acc -= 10000;
        break;

    case PB12_SUB_ACC_I:    /* 17 - Subtract Accumulator Immediate */
        cpu->acc -= pb12GetInstValue(cpu);
        if (cpu->acc < 0000)
            cpu->acc += 10000;
        break;

    case PB12_ADD_ACC_REG:  /* 18 - Add contents of Register from Accumulator */
        genReg1 = pb12GetGeneralRegister(cpu, 0);
        if (!genReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_1]);
            return PB12_FAILURE;
        }
        cpu->acc += *genReg1;
        if (cpu->acc > 9999)
            cpu->acc -= 10000;
        break;

    case PB12_SUB_ACC_REG:  /* 19 - Subtract contents of Register from Accumulator */
        genReg1 = pb12GetGeneralRegister(cpu, 0);
        if (!genReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_1]);
            return PB12_FAILURE;
        }
        cpu->acc -= *genReg1;
        if (cpu->acc < 0)
            cpu->acc += 10000;
        break;

    case PB12_ADD_ACC_R:    /* 20 - Add Accumulator Register Addressing */
        ptrReg1 = pb12GetPointerRegister(cpu, 0);
        if (!ptrReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_PTR_1]);
            return PB12_FAILURE;
        }
        cpu->acc += pb12GetMemOp(cpu, mem, *ptrReg1);
        if (cpu->acc > 9999)
            cpu->acc -= 9999;
        break;

    case PB12_ADD_ACC_D:    /* 21 - Add Accumulator Direct Addressing */
        addr = pb12GetOperand(cpu, 0);
        if (addr >= mem->mem_size) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_ADDR]);
            return PB12_FAILURE;
        }
        cpu->acc += pb12GetMemOp(cpu, mem, addr);
        if (cpu->acc > 9999)
            cpu->acc -= 10000;
        break;

    case PB12_SUB_ACC_R:    /* 22 - Subtract from Accumulator Register Addressing */
        ptrReg1 = pb12GetPointerRegister(cpu, 0);
        if (!ptrReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_PTR_1]);
            return PB12_FAILURE;
        }
        cpu->acc -= pb12GetMemOp(cpu, mem, *ptrReg1);
        if (cpu->acc < 0)
            cpu->acc += 10000;
        break;

    case PB12_SUB_ACC_D:    /* 23 - Subtract from Accumulator Direct Addressing */
        addr = pb12GetOperand(cpu, 0);
        if (addr >= mem->mem_size) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_ADDR]);
            return PB12_FAILURE;
        }
        cpu->acc -= pb12GetMemOp(cpu, mem, addr);
        if (cpu->acc < 0)
            cpu->acc += 10000;
        break;

    case PB12_EQU_R:        /* 24 - Compare Equal Register Addressing */
        ptrReg1 = pb12GetPointerRegister(cpu, 0);
        if (!ptrReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_PTR_1]);
            return PB12_FAILURE;
        }
        pb12SetPswCmp(cpu, cpu->acc == pb12GetMemOp(cpu, mem, *ptrReg1));
        break;

    case PB12_LES_R:        /* 25 - Compare Less Register Addressing */
        ptrReg1 = pb12GetPointerRegister(cpu, 0);
        if (!ptrReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_PTR_1]);
            return PB12_FAILURE;
        }
        pb12SetPswCmp(cpu, cpu->acc < pb12GetMemOp(cpu, mem, *ptrReg1));
        break;

    case PB12_GRE_R:        /* 26 - Compare Greater Register Addressing */
        ptrReg1 = pb12GetPointerRegister(cpu, 0);
        if (!ptrReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_PTR_1]);
            return PB12_FAILURE;
        }
        pb12SetPswCmp(cpu, cpu->acc > pb12GetMemOp(cpu, mem, *ptrReg1));
        break;

    case PB12_GRE_I:        /* 27 - Compare Greater Immediate */
        pb12SetPswCmp(cpu, cpu->acc > pb12GetInstValue(cpu));
        break;

    case PB12_EQU_I:        /* 28 - Compare Equal Immediate */
        pb12SetPswCmp(cpu, cpu->acc == pb12GetInstValue(cpu));
        break;

    case PB12_LES_I:        /* 29 - Compare Less Immediate */
        pb12SetPswCmp(cpu, cpu->acc < pb12GetInstValue(cpu));
        break;

    case PB12_EQU_REG:      /* 30 - Compare Register Equal */
        genReg1 = pb12GetGeneralRegister(cpu, 0);
        if (!genReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_1]);
            return PB12_FAILURE;
        }
        pb12SetPswCmp(cpu, cpu->acc == *genReg1);
        break;

    case PB12_LES_REG:      /* 31 - Compare Register Less */
        genReg1 = pb12GetGeneralRegister(cpu, 0);
        if (!genReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_1]);
            return PB12_FAILURE;
        }
        pb12SetPswCmp(cpu, cpu->acc < *genReg1);
        break;

    case PB12_GRE_REG:      /* 32 - Compare Register Greater */
        genReg1 = pb12GetGeneralRegister(cpu, 0);
        if (!genReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_1]);
            return PB12_FAILURE;
        }
        pb12SetPswCmp(cpu, cpu->acc > *genReg1);
        break;

    case PB12_BRT:          /* 33 - Branch Condition True */
        if (cpu->psw[0] == 'T') {
            cpu->pc = pb12GetOperand(cpu, 0);
        }
        break;

    case PB12_BRF:          /* 34 - Branch Condition False */
        if (cpu->psw[0] == 'F') {
            cpu->pc = pb12GetOperand(cpu, 0);
        }
        break;

    case PB12_BRU:          /* 35 - Branch Unconditional */
        cpu->pc = pb12GetOperand(cpu, 0);
        break;

    case PB12_HLT:          /* 99 - Halt */
        return PB12_TERMINATE;
        break;

    case PB12_TRAP:         /* 36 - OS Trap Instruction */
        genReg1 = pb12GetGeneralRegister(cpu, 0);
        if (!genReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_1]);
            return PB12_FAILURE;
        }

        hw->trap_num = *genReg1;
        hw->trap_op = pb12GetRegisterNumber(cpu, 1);

        break;

    case PB12_MOD:          /* 37 - Modulo Operator */
        genReg1 = pb12GetGeneralRegister(cpu, 0);
        if (!genReg1) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_1]);
            return PB12_FAILURE;
        }

        genReg2 = pb12GetGeneralRegister(cpu, 1);
        if (!genReg2) {
            pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_REG_2]);
            return PB12_FAILURE;
        }

        cpu->acc = *genReg1 % *genReg2;

        /* printf("%d MOD %d = %d\n", *genReg1, *genReg2, cpu->acc); */

        break;

    default:
        pb12ErrorMsg(pb12ErrorStr[PB12_ERROR_INVALID_OPCODE]);
        return PB12_FAILURE;
        /*return PB12_TERMINATE;*/
    }

    return PB12_SUCCESS;
}
