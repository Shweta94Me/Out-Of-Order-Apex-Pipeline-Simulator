/*
 * apex_cpu.h
 * Contains APEX cpu pipeline declarations
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#ifndef _APEX_CPU_H_
#define _APEX_CPU_H_

#include "apex_macros.h"
#include "issue_q.h"
#include "utilities.h"


/* Format of an APEX instruction  */
typedef struct APEX_Instruction
{
    char opcode_str[128];
    int opcode;

    int rd; //Architectural Register
    int rd_phy_res; //Physical Register

    int rs1; //Architectural Register
    int rs1_phy_res; //Physical Register
    

    int rs2; //Architectural Register
    int rs2_phy_res; //Physical Register
    

    int rs3; //Shweta ::: Added entry for third register e.g. STR //Architectural Register
    int rs3_phy_res; //Physical Register
    

    int imm;
} APEX_Instruction;

/* Model of CPU stage latch */
typedef struct CPU_Stage
{
    int pc;
    char opcode_str[128];
    int opcode;

    //Reason for adding phy register in cpu stage is to keep track of physical register across all stage so you dont have to check in each stage
    int rs1; //Architectural Register
    int rs1_phy_res; //Physical Register
    int rs1_ready; // Ready bit for src1

    int rs2; //Architectural Register
    int rs2_phy_res; //Physical Register
    int rs2_ready; // Ready bit for src1

    int rs3; //Shweta ::: Added entry for third register e.g. STR  //Architectural Register
    int rs3_phy_res; //Physical Register
    int rs3_ready; // Ready bit for src1
    
    int rd; //Architectural Register
    int rd_phy_res; //Physical Register

    enum FU fu_type;

    int imm;

    int rs1_value;
    int rs2_value;
    int rs3_value; //Shweta ::: Added entry for third register e.g. STR  //Architectural Register

    int result_buffer;
    int memory_address;
    int has_insn;
    int stalled;
} CPU_Stage;

/* Model of APEX CPU */
typedef struct APEX_CPU
{
    int pc;                        /* Current program counter */
    int clock;                     /* Clock cycles elapsed */
    int insn_completed;            /* Instructions retired */
    int regs[REG_FILE_SIZE];       /* Integer register file */
    int code_memory_size;          /* Number of instruction in the input file */
    APEX_Instruction *code_memory; /* Code Memory */
    int data_memory[DATA_MEMORY_SIZE]; /* Data Memory */
    int single_step;               /* Wait for user input after every cycle */
    int zero_flag;                 /* {TRUE, FALSE} Used by BZ and BNZ to branch */
    int fetch_from_next_cycle;

    int int_fu_free;               //flag for int fu availability 0 is free 1 is occupied
    int mul_fu_free;               //flag for mul fu availability 0 is free 1 is occupied

    int mul_cycles;  //Mul unit takes 3 cycles to complete instruction

    URF urf[URFMaxSize];
    RAT rat[RATMaxSize];
    RRAT rrat[RRATMaxSize];
    
    Queue *iq;
    

    /* Pipeline stages */
    CPU_Stage fetch;
    CPU_Stage decode;
    CPU_Stage ex_int_fu;
    CPU_Stage ex_mul_fu;
    //Shweta ::: Why two mem stages added ::: memory stage is pipelined into two
    CPU_Stage mem1; 
    CPU_Stage mem2;

    CPU_Stage writeback;
} APEX_CPU;

APEX_Instruction *create_code_memory(const char *filename, int *size);
APEX_CPU *APEX_cpu_init(const char *filename);
void APEX_cpu_run(APEX_CPU *cpu);
void APEX_cpu_stop(APEX_CPU *cpu);
#endif
