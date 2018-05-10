#ifndef PB12_INST_H
#define PB12_INST_H

/* Instructions */
#define PB12_LOD_PTR_I       0 /* Load Pointer Immediate */
#define PB12_ADD_PTR_I       1 /* Add to Pointer Immediate */
#define PB12_SUB_PTR_I       2 /* Subtract from Pointer Immediate */
#define PB12_LOD_ACC_I       3 /* Load Accumulator Immediate */
#define PB12_LOD_ACC_R       4 /* Load Accumulator Register Addressing */
#define PB12_LOD_ACC_D       5 /* Load Accumulator Direct Addressing */
#define PB12_STO_ACC_R       6 /* Store Accumulator Register Addressing */
#define PB12_STO_ACC_D       7 /* Store Accumulator Direct Addressing */
#define PB12_STO_REG_R       8 /* Store Register to Memory: Register Addressing */
#define PB12_STO_REG_D       9 /* Store Register to Memory: Direct Addressing */
#define PB12_LOD_REG_R      10 /* Load Register from Memory: Register Addressing */
#define PB12_LOD_REG_D      11 /* Load Register from Memory: Direct Addressing */
#define PB12_LOD_REG_R0_I   12 /* Load Register R0 Immediate */
#define PB12_TRA_REG_REG    13 /* Register to Register Transfer */
#define PB12_LOD_ACC_REG    14 /* Load Accumulator from Register */
#define PB12_LOD_REG_ACC    15 /* Load Register from Accumulator */
#define PB12_ADD_ACC_I      16 /* Add Accumulator Immediate */
#define PB12_SUB_ACC_I      17 /* Subtract Accumulator Immediate */
#define PB12_ADD_ACC_REG    18 /* Add contents of Register from Accumulator */
#define PB12_SUB_ACC_REG    19 /* Subtract contents of Register from Accumulator */
#define PB12_ADD_ACC_R      20 /* Add Accumulator Register Addressing */
#define PB12_ADD_ACC_D      21 /* Add Accumulator Direct Addressing */
#define PB12_SUB_ACC_R      22 /* Subtract from Accumulator Register Addressing */
#define PB12_SUB_ACC_D      23 /* Subtract from Accumulator Direct Addressing */
#define PB12_EQU_R          24 /* Compare Equal Register Addressing */
#define PB12_LES_R          25 /* Compare Less Register Addressing */
#define PB12_GRE_R          26 /* Compare Greater Register Addressing */
#define PB12_GRE_I          27 /* Compare Greater Immediate */
#define PB12_EQU_I          28 /* Compare Equal Immediate */
#define PB12_LES_I          29 /* Compare Less Immediate */
#define PB12_EQU_REG        30 /* Compare Register Equal */
#define PB12_LES_REG        31 /* Compare Register Less */
#define PB12_GRE_REG        32 /* Compare Register Greater */
#define PB12_BRT            33 /* Branch Condition True */
#define PB12_BRF            34 /* Branch Condition False */
#define PB12_BRU            35 /* Branch Unconditional */
#define PB12_TRAP           36 /* OS Trap Instruction */
#define PB12_MOD            37 /* Modulo Operator */
#define PB12_MAX_OPCODE     38 /* ALL OPCODES ARE VALUED LESS THAN THIS */
#define PB12_HLT            99 /* Halt */

#endif /* PB12_INST_H */
