#include <stdio.h>
#include "pb12.h"
#include "pb12_os.h"
#include "pb12_hw.h"
#include "pb12_traps.h"


void (*pb12TrapVectors[PB12_TRAP_MAX_TRAP])(struct S_PB12_OS *os) = {
    pb12TrapWait,
    pb12TrapSignal,
    pb12TrapPID,
    pb12TrapDump
};


/**
    Wait on Semaphore trap.

    @param PB12_OS *os - Operating System
*/
void pb12TrapWait(struct S_PB12_OS *os) {
    int *reg;
    reg = pb12GetGenReg(&os->hw->cpu, os->hw->trap_op);
    if (reg) {
        if (*reg == 0) {
            if (pb12Options & PB12_OPT_VERBOSE) {
                printf("Trap: Wait on fork %d.\n", os->hw->cpu.acc);
            }
            pb12SemWait(&os->forks[os->hw->cpu.acc], os);
        }
        else {
            if (pb12Options & PB12_OPT_VERBOSE) {
                printf("Trap: Wait on doorman.\n");
            }
            pb12SemWait(&os->doorman, os);
        }
    }
}


/**
    Signal Semaphore trap.

    @param PB12_OS *os - Operating System
*/
void pb12TrapSignal(struct S_PB12_OS *os) {
    int *reg;
    reg = pb12GetGenReg(&os->hw->cpu, os->hw->trap_op);
    if (reg) {
        if (*reg == 0) {
            if (pb12Options & PB12_OPT_VERBOSE) {
                printf("Trap: Signal on fork %d.\n", os->hw->cpu.acc);
            }
            pb12SemSignal(&os->forks[os->hw->cpu.acc], os);
        }
        else {
            if (pb12Options & PB12_OPT_VERBOSE) {
                printf("Trap: Signal doorman.\n");
            }
            pb12SemSignal(&os->doorman, os);
        }
    }
}


/**
    Get Process ID trap.

    @param PB12_OS *os - Operating System
*/
void pb12TrapPID(struct S_PB12_OS *os) {
    int *reg;
    reg = pb12GetGenReg(&os->hw->cpu, os->hw->trap_op);
    if (reg) {
        *reg = pb12CurrentPid(os);
        if (pb12Options & PB12_OPT_VERBOSE) {
            printf("Trap: Get PID %d.\n", *reg);
        }
    }
}


/**
    Trap added for debugging purposes.

    @param struct S_PB12_OS *os - Operating System
*/
void pb12TrapDump(struct S_PB12_OS *os) {
    pb12DumpCPU(&os->hw->cpu);
    pb12DumpMemory(&os->hw->mem);
}
