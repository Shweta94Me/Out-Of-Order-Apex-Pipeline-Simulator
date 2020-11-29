/*
 * apex_cpu.c
 * Contains APEX cpu pipeline implementation
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apex_cpu.h"
#include "apex_macros.h"
#include "issue_q.h"
#include "utilities.h"

/* Converts the PC(4000 series) into array index for code memory
 *
 * Note: You are not supposed to edit this function
 */
static int
get_code_memory_index_from_pc(const int pc)
{
    return (pc - 4000) / 4;
}

static void
print_instruction(const CPU_Stage *stage)
{
    switch (stage->opcode)
    {
        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_MUL:
        case OPCODE_DIV:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_XOR:
        {
            printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rd, stage->rs1,
                   stage->rs2);
            break;
        }

        case OPCODE_MOVC:
        {
            printf("%s,R%d,#%d ", stage->opcode_str, stage->rd, stage->imm);
            break;
        }

        case OPCODE_LOAD:
        {
            printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rd, stage->rs1,
                   stage->imm);
            break;
        }

        case OPCODE_STORE:
        {
            printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rs1, stage->rs2,
                   stage->imm);
            break;
        }

        case OPCODE_BZ:
        case OPCODE_BNZ:
        {
            printf("%s,#%d ", stage->opcode_str, stage->imm);
            break;
        }
        
        /*Shweta  ::: Start : Added new instructions*/
        case OPCODE_LDR:
        {
            printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rd, stage->rs1,
                   stage->rs2);
            break;
        }

        case OPCODE_STR:
        {
            printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rs3, stage->rs1,
                   stage->rs2);
            break;
        }

        case OPCODE_ADDL:
        case OPCODE_SUBL:
        {
            printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rd, stage->rs1,
                   stage->imm);
            break;
        }

        case OPCODE_CMP:
        {
            printf("%s,R%d,R%d ", stage->opcode_str, stage->rs1, stage->rs2);
            break;
        }

        case OPCODE_JUMP:
        {
            printf("%s,R%d,#%d ", stage->opcode_str, stage->rs1,
                   stage->imm);
            break;
        }

        case OPCODE_JAL:
        {
            printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rd, stage->rs1,
                   stage->imm);
            break;
        }

        /*Shweta  ::: End :: Added new instructions*/
        case OPCODE_HALT:
        {
            printf("%s", stage->opcode_str);
            break;
        }
    }
}

/* Debug function which prints the CPU stage content
 *
 * Note: You can edit this function to print in more detail
 */
static void
print_stage_content(const char *name, const CPU_Stage *stage)
{
    printf("%-15s: pc(%d) ", name, stage->pc);
    print_instruction(stage);
    printf("\n");
}

/* Debug function which prints the register file
 *
 * Note: You are not supposed to edit this function
 */
static void
print_reg_file(const APEX_CPU *cpu)
{
    int i;

    printf("----------\n%s\n----------\n", "Registers:");

    for (int i = 0; i < REG_FILE_SIZE / 2; ++i)
    {
        printf("R%-3d[%-3d] ", i, cpu->regs[i]);
    }

    printf("\n");

    for (i = (REG_FILE_SIZE / 2); i < REG_FILE_SIZE; ++i)
    {
        printf("R%-3d[%-3d] ", i, cpu->regs[i]);
    }

    printf("\n");
}

/*
 * Fetch Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_fetch(APEX_CPU *cpu)
{
    APEX_Instruction *current_ins;

    if (cpu->fetch.has_insn && !cpu->fetch.stalled)
    {
        /* This fetches new branch target instruction from next cycle */
        if (cpu->fetch_from_next_cycle == TRUE)
        {
            cpu->fetch_from_next_cycle = FALSE;

            /* Skip this cycle*/
            return;
        }

        /* Store current PC in fetch latch */
        cpu->fetch.pc = cpu->pc;

        /* Index into code memory using this pc and copy all instruction fields
         * into fetch latch  */
        current_ins = &cpu->code_memory[get_code_memory_index_from_pc(cpu->pc)];
        strcpy(cpu->fetch.opcode_str, current_ins->opcode_str);
        cpu->fetch.opcode = current_ins->opcode;
        cpu->fetch.rd = current_ins->rd;
        cpu->fetch.rs1 = current_ins->rs1;
        cpu->fetch.rs2 = current_ins->rs2;
        cpu->fetch.rs3 = current_ins->rs3;
        cpu->fetch.imm = current_ins->imm;


        /* Update PC for next instruction */
        cpu->pc += 4;

        if(!cpu->decode.stalled){
            /* Copy data from fetch latch to decode latch*/
            cpu->decode = cpu->fetch;
        }else{
            cpu->fetch.stalled = 1;
        }
        

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Fetch", &cpu->fetch);
        }

        /* Stop fetching new instructions if HALT is fetched */
        if (cpu->fetch.opcode == OPCODE_HALT)
        {
            cpu->fetch.has_insn = FALSE;
        }
    }else{
        if(!cpu->decode.stalled){
            cpu->fetch.stalled = 0;

            /* Copy data from fetch latch to decode latch*/
            cpu->decode = cpu->fetch;
        }
        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Fetch", &cpu->fetch);
        }

        /* Stop fetching new instructions if HALT is fetched */
        if (cpu->fetch.opcode == OPCODE_HALT)
        {
            cpu->fetch.has_insn = FALSE;
        }
    }
}

/*Make instruction entry to Issue Queue*/
void dispatch_instr_to_IQ(APEX_CPU *cpu, enum FU fu_type)
{
    cpu->decode.stalled = 0; //Set decode to unstalled
    if(fu_type == Int_FU){

        int phy_reg_dest = 0;

        //CMP instruction has no destination register
        if(strcmp(cpu->decode.opcode_str, "CMP") != 0 && strcmp(cpu->decode.opcode_str, "HALT") != 0){
            phy_reg_dest = allocate_phy_dest_RAT(cpu->URF, cpu->RAT, cpu->decode.rd);
        }
        
        if (phy_reg_dest != -1){

            //CMP instruction has no destination register
            if(strcmp(cpu->decode.opcode_str, "CMP") != 0 && strcmp(cpu->decode.opcode_str, "HALT") != 0)
                cpu->decode.rd_phy_res = phy_reg_dest;
            
            //MOVC instruction has no source registers
            if(strcmp(cpu->decode.opcode_str, "MOVC") != 0 && strcmp(cpu->decode.opcode_str, "HALT") != 0){
                cpu->decode.rs1_ready = renameSrc1_readSrc1(cpu->URF, cpu->RAT, cpu);
                cpu->decode.rs2_ready = renameSrc2_readSrc2(cpu->URF, cpu->RAT, cpu);
                cpu->decode.rs3_ready = 1;
            }
            else{
                cpu->decode.rs1_ready = 1;
                cpu->decode.rs2_ready = 1;
                cpu->decode.rs3_ready = 1;
            }
            //Pass all instructions to Issue Queue
            enQueue(cpu->iq, cpu);
        }
        else{
            //No physical register available in URF
            cpu->decode.stalled = 1; 
        }
    }
    else if(fu_type == Mul_FU){
        int phy_reg_dest = 0;
        phy_reg_dest = allocate_phy_dest_RAT(cpu->URF, cpu->RAT, cpu->decode.rd);
        if (phy_reg_dest != -1){

            cpu->decode.rd_phy_res = phy_reg_dest;

            cpu->decode.rs1_ready = renameSrc1_readSrc1(cpu->URF, cpu->RAT, cpu);
            cpu->decode.rs2_ready = renameSrc2_readSrc2(cpu->URF, cpu->RAT, cpu);

            enQueue(cpu->iq, cpu);
        }
        else{
            //No physical register available in URF
            cpu->decode.stalled = 1; 
        }
    }
    else if (fu_type == Mem_FU)
    {
        int phy_reg_dest = 0;

        //CMP instruction has no destination register
        if(strcmp(cpu->decode.opcode_str, "STORE") != 0 && strcmp(cpu->decode.opcode_str, "STR") != 0){
            phy_reg_dest = allocate_phy_dest_RAT(cpu->URF, cpu->RAT, cpu->decode.rd);
        }

        if (phy_reg_dest != -1){

            //CMP instruction has no destination register
            if(strcmp(cpu->decode.opcode_str, "STORE") != 0 && strcmp(cpu->decode.opcode_str, "STR") != 0)
                cpu->decode.rd_phy_res = phy_reg_dest;
            
            if(strcmp(cpu->decode.opcode_str, "LOAD") == 0 ||
            strcmp(cpu->decode.opcode_str, "LDR") == 0 ||
            strcmp(cpu->decode.opcode_str, "STORE") == 0 ||
            strcmp(cpu->decode.opcode_str, "STR") == 0)
            {
                cpu->decode.rs1_ready = renameSrc1_readSrc1(cpu->URF, cpu->RAT, cpu);
            }

            if(strcmp(cpu->decode.opcode_str, "LDR") == 0 ||
            strcmp(cpu->decode.opcode_str, "STORE") == 0 ||
            strcmp(cpu->decode.opcode_str, "STR") == 0)
            {
                cpu->decode.rs2_ready = renameSrc2_readSrc2(cpu->URF, cpu->RAT, cpu);
            }

            if(strcmp(cpu->decode.opcode_str, "STR") == 0){
                cpu->decode.rs2_ready = renameSrc3_readSrc3(cpu->URF, cpu->RAT, cpu);
            }

            //Pass all instructions to Issue Queue
            enQueue(cpu->iq, cpu);

        }
        else{
            //No physical register available in URF
            cpu->decode.stalled = 1; 
        }
    }
    
}
/*
 * Decode Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_decode(APEX_CPU *cpu)
{
    if (cpu->decode.has_insn && !isQueueFull)
    {
        /* Read operands from register file based on the instruction type */
        switch (cpu->decode.opcode)
        {
            case OPCODE_ADD:
            case OPCODE_SUB:
            case OPCODE_AND:
            case OPCODE_OR:
            case OPCODE_XOR:
            case OPCODE_MOVC:
            case OPCODE_ADDL:
            case OPCODE_SUBL:
            case OPCODE_CMP:
            {
                dispatch_instr_to_IQ(cpu, Int_FU);

                if(!cpu->decode.stalled){
                    /* Copy data from decode latch to execute latch*/
                    // cpu->ex_int_fu = cpu->decode;
                    cpu->decode.has_insn = FALSE;
                }
                break;
            }

            case OPCODE_LOAD:
            case OPCODE_LDR:
            case OPCODE_STORE:
            case OPCODE_STR:
            {
                dispatch_instr_to_IQ(cpu, Mem_FU);

                if(!cpu->decode.stalled){
                    /* Copy data from decode latch to execute latch*/
                    // cpu->mem1 = cpu->decode;
                    cpu->decode.has_insn = FALSE;
                }
                break;
            }

            case OPCODE_MUL:
            {
                dispatch_instr_to_IQ(cpu, Mul_FU);

                if(!cpu->decode.stalled){
                    /* Copy data from decode latch to execute latch*/
                    // cpu->ex_mul_fu = cpu->decode;
                    cpu->decode.has_insn = FALSE;
                }
                break;
            }

            case OPCODE_HALT:
            {
                dispatch_instr_to_IQ(cpu, Int_FU);
                /* Copy data from decode latch to execute latch*/
                // cpu->ex_int_fu = cpu->decode;
                cpu->decode.has_insn = FALSE;
                break;
            }
        }

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Decode/RF", &cpu->decode);
        }

    }
    else{
        cpu->decode.stalled = 1;
        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Decode/RF", &cpu->decode);
        }
    }
}

//Broadcasting mechanism also know as data forwarding
/*Here we will broadcast destination register value for those instructions who are waiting in issue queue with same
source regsiter as this destination register*/
void broadcastData(APEX_CPU *cpu, enum FU fu_type)
{
    //1: Make entry in URF 
    updateURF(cpu, fu_type);

    //2: updated IQ entries which has same source regsiters as this destination
    updateIQ(cpu, fu_type);
}
/*
 * Execute Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_int_fu(APEX_CPU *cpu)
{
    if (cpu->ex_int_fu.has_insn)
    {
        /* Execute logic based on instruction type */
        switch (cpu->ex_int_fu.opcode)
        {
            case OPCODE_ADD:
            {
                cpu->int_fu_free = 1;
                cpu->ex_int_fu.result_buffer
                    = cpu->ex_int_fu.rs1_value + cpu->ex_int_fu.rs2_value;

                broadcastData(cpu, Int_FU);

                /* Set the zero flag based on the result buffer */
                if (cpu->ex_int_fu.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_SUB:
            {
                cpu->int_fu_free = 1;
                cpu->ex_int_fu.result_buffer
                    = cpu->ex_int_fu.rs1_value - cpu->ex_int_fu.rs2_value;

                broadcastData(cpu, Int_FU);

                /* Set the zero flag based on the result buffer */
                if (cpu->ex_int_fu.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_AND:
            {
                cpu->int_fu_free = 1;
                cpu->ex_int_fu.result_buffer
                    = cpu->ex_int_fu.rs1_value & cpu->ex_int_fu.rs2_value;
                
                broadcastData(cpu, Int_FU);
                break;
            }
            
            case OPCODE_OR:
            {
                cpu->int_fu_free = 1;
                cpu->ex_int_fu.result_buffer
                    = cpu->ex_int_fu.rs1_value | cpu->ex_int_fu.rs2_value;

                broadcastData(cpu, Int_FU);
                break;
            }

            case OPCODE_XOR:
            {
                cpu->int_fu_free = 1;
                cpu->ex_int_fu.result_buffer
                    = cpu->ex_int_fu.rs1_value ^ cpu->ex_int_fu.rs2_value;

                broadcastData(cpu, Int_FU);
                break;
            }

            case OPCODE_MOVC:
            {
                cpu->int_fu_free = 1;
                cpu->ex_int_fu.result_buffer
                    = cpu->ex_int_fu.imm;

                broadcastData(cpu, Int_FU); 
                break;
            }

            case OPCODE_ADDL:
            {
                cpu->int_fu_free = 1;
                cpu->ex_int_fu.result_buffer
                    = cpu->ex_int_fu.rs1_value + cpu->ex_int_fu.imm;
                
                broadcastData(cpu, Int_FU);
                break;
            }

            case OPCODE_SUBL:
            {
                cpu->int_fu_free = 1;
                cpu->ex_int_fu.result_buffer
                    = cpu->ex_int_fu.rs1_value - cpu->ex_int_fu.imm;
                
                broadcastData(cpu, Int_FU);
                break;
            }

            case OPCODE_CMP:
            {
                cpu->int_fu_free = 1;
                //Shweta ::: As there are no source register for CMP instruction; no need to broadcast
                if (cpu->ex_int_fu.rs1_value == cpu->ex_int_fu.rs2_value)
                {
                    cpu->zero_flag = TRUE;
                }
                else{
                    cpu->zero_flag = FALSE;
                }
                break;
            }

            case OPCODE_JUMP:
            {

                /*Shweta ::: Calculate the new PC and send it to fetch unit*/
                cpu->pc = cpu->ex_int_fu.rs1_value + cpu->ex_int_fu.imm;

                cpu->fetch_from_next_cycle = TRUE;

                /*Flush previous stages*/
                cpu->decode.has_insn = FALSE;

                /*Enable fetch stage to start fetching from new PC*/
                cpu->fetch.has_insn = TRUE;

                break;

            }

            case OPCODE_JAL:
            {
                cpu->ex_int_fu.result_buffer = cpu->pc + 4;
                cpu->ex_int_fu.pc = cpu->ex_int_fu.rs1_value + cpu->ex_int_fu.imm;

                /* Since we are using reverse callbacks for pipeline stages, 
                 * this will prevent the new instruction from being fetched in the current cycle*/
                cpu->fetch_from_next_cycle = TRUE;

                /* Flush previous stages */
                cpu->decode.has_insn = FALSE;

                /* Make sure fetch stage is enabled to start fetching from new PC */
                cpu->fetch.has_insn = TRUE;

            }
            case OPCODE_LOAD:
            {
                cpu->ex_int_fu.memory_address
                    = cpu->ex_int_fu.rs1_value + cpu->ex_int_fu.imm;
                break;
            }

            case OPCODE_BZ:
            {
                if (cpu->zero_flag == TRUE)
                {
                    /* Calculate new PC, and send it to fetch unit */
                    cpu->pc = cpu->ex_int_fu.pc + cpu->ex_int_fu.imm;
                    
                    /* Since we are using reverse callbacks for pipeline stages, 
                     * this will prevent the new instruction from being fetched in the current cycle*/
                    cpu->fetch_from_next_cycle = TRUE;

                    /* Flush previous stages */
                    cpu->decode.has_insn = FALSE;

                    /* Make sure fetch stage is enabled to start fetching from new PC */
                    cpu->fetch.has_insn = TRUE;
                }
                break;
            }

            case OPCODE_BNZ:
            {
                if (cpu->zero_flag == FALSE)
                {
                    /* Calculate new PC, and send it to fetch unit */
                    cpu->pc = cpu->ex_int_fu.pc + cpu->ex_int_fu.imm;
                    
                    /* Since we are using reverse callbacks for pipeline stages, 
                     * this will prevent the new instruction from being fetched in the current cycle*/
                    cpu->fetch_from_next_cycle = TRUE;

                    /* Flush previous stages */
                    cpu->decode.has_insn = FALSE;

                    /* Make sure fetch stage is enabled to start fetching from new PC */
                    cpu->fetch.has_insn = TRUE;
                }
                break;
            }
        }

        /* Copy data from execute latch to memory latch*/
        // cpu->memory = cpu->ex_int_fu;  //Shweta ::: Instead of passing it to memory update ROB entry
        //Clear int unit stage
        cpu->ex_int_fu.has_insn = FALSE;
        cpu->int_fu_free = 0;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Execute", &cpu->ex_int_fu);
        }
    }
}


static void
APEX_mul_fu(APEX_CPU *cpu)
{
    //Entering mul unit for the first time 
    /*Shweta ::: We need to wait for three cycles in MUL and mul_cycles initialized with zero hence checking 
    against 2 when entering in MUL_FU for third cycle*/
    if (cpu->ex_mul_fu.has_insn && cpu->mul_cycles == 2)
    {
        /* Execute logic based on instruction type */
        switch (cpu->ex_mul_fu.opcode)
        {
            case OPCODE_MUL:
            {
                cpu->ex_mul_fu.result_buffer
                    = cpu->ex_mul_fu.rs1_value * cpu->ex_mul_fu.rs2_value;
                
                cpu->mul_cycles = 0; //Reset
                cpu->mul_fu_free = 0; //free mul unit

                broadcastData(cpu, Mul_FU); // only when completed 3 cycles

                /* Copy data from execute latch to memory latch*/
                // cpu->memory = cpu->ex_mul_fu; //Shweta ::: Instead of passing it to memory update ROB entry
                cpu->ex_mul_fu.has_insn = FALSE;
                break;
            }
        }

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Execute", &cpu->ex_mul_fu);
        }
    }
    else{
        cpu->mul_cycles += 1;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Execute", &cpu->ex_mul_fu);
        }        
    }
}

/*Shweta : To handle errors*/
int errorHandler(char *errorType)
{
    if (strcmp(errorType, "MemoryError") == 0)
    {
        printf("Error ::: Requested effective memory address if out of range (0-4096).\n");
        exit(1);
    }
    return 0;
}

/*
 * Shweta ::: Memory1 Stage of APEX Pipeline ::: Memory address compution 
 */

static void
APEX_memory1(APEX_CPU *cpu)
{
    if (cpu->mem1.has_insn)
    {
        switch (cpu->mem1.opcode)
        {
            case OPCODE_LOAD:
            {
                cpu->mem1.memory_address = 
                    cpu->mem1.rs1_value + cpu->mem1.imm;

                if (cpu->mem1.memory_address > 4096 || cpu->mem1.memory_address < 0)
                {
                    errorHandler("MemoryError");
                }
                break;
            }

            case OPCODE_LDR:
            {
                cpu->mem1.memory_address =
                    cpu->mem1.rs1_value + cpu->mem1.rs2_value;
                
                if (cpu->mem1.memory_address > 4096 || cpu->mem1.memory_address < 0)
                {
                    errorHandler("MemoryError");
                }

                break;
            }

            case OPCODE_STORE:
            {
                cpu->mem1.memory_address =
                    cpu->mem1.rs2_value + cpu->mem1.imm;
                
                if (cpu->mem1.memory_address > 4096 || cpu->mem1.memory_address < 0)
                {
                    errorHandler("MemoryError");
                }

                break;
            }

            case OPCODE_STR:
            {
                cpu->mem1.memory_address =
                    cpu->mem1.rs2_value + cpu->mem1.rs3_value;
                
                if (cpu->mem1.memory_address > 4096 || cpu->mem1.memory_address < 0)
                {
                    errorHandler("MemoryError");
                }

                break;
            }
        }

        cpu->mem2 = cpu->mem1;
        cpu->mem1.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Memory1", &cpu->mem1);
        }
    }
}

/*
 * Shweta ::: Memory2 Stage of APEX Pipeline ::: Accessing D-Cache
 */

static void
APEX_memory2(APEX_CPU *cpu)
{
    if (cpu->mem2.has_insn)
    {
        switch (cpu->mem2.opcode)
        {
            case OPCODE_LOAD:
            case OPCODE_LDR:
            {
                /* Read from data memory */
                cpu->mem2.result_buffer = cpu->data_memory[cpu->mem2.memory_address];

                broadcastData(cpu, Mem_FU);
                //Shweta ::: Update ROB entry for commitment
                break;
            }


            case OPCODE_STORE:
            case OPCODE_STR:
            {
                 /*Store data from destination register to data memory */
                cpu->data_memory[cpu->mem2.memory_address] = cpu->mem2.rs1_value;
                break;
            }
        }

        cpu->mem1.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Memory2", &cpu->mem2);
        }
    }
}

/*
 * This function creates and initializes APEX cpu.
 *
 * Note: You are free to edit this function according to your implementation
 */
APEX_CPU *
APEX_cpu_init(const char *filename)
{
    int i;
    APEX_CPU *cpu;

    if (!filename)
    {
        return NULL;
    }

    cpu = calloc(1, sizeof(APEX_CPU));

    if (!cpu)
    {
        return NULL;
    }

    /* Initialize PC, Registers and all pipeline stages */
    cpu->pc = 4000;
    memset(cpu->regs, 0, sizeof(int) * REG_FILE_SIZE);
    memset(cpu->data_memory, 0, sizeof(int) * DATA_MEMORY_SIZE);
    cpu->single_step = ENABLE_SINGLE_STEP;

    /* Parse input file and create code memory */
    cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);
    if (!cpu->code_memory)
    {
        free(cpu);
        return NULL;
    }

    // Initialize Issue Queue
    cpu->iq = createQueue();

    // Initiliaze URF
    for (int i =0 ; i < URFMaxSize; i++){
        cpu->URF[i].free = 0;
        cpu->URF[i].status = 1;
        cpu->URF[i].value = 0;
    }

    // Initialize RAT
    for (int i = 0; i < RATMaxSize; i++)
    {
        cpu->RAT[i].phy_reg_num = -1;
    }

    // Initialize RRAT
    for (int i = 0; i < RATMaxSize; i++)
    {
        cpu->RRAT[i].phy_reg_after_comit = -1;
    }

    if (ENABLE_DEBUG_MESSAGES)
    {
        fprintf(stderr,
                "APEX_CPU: Initialized APEX CPU, loaded %d instructions\n",
                cpu->code_memory_size);
        fprintf(stderr, "APEX_CPU: PC initialized to %d\n", cpu->pc);
        fprintf(stderr, "APEX_CPU: Printing Code Memory\n");
        printf("%-9s %-9s %-9s %-9s %-9s\n", "opcode_str", "rd", "rs1", "rs2",
               "imm");

        for (i = 0; i < cpu->code_memory_size; ++i)
        {
            printf("%-9s %-9d %-9d %-9d %-9d\n", cpu->code_memory[i].opcode_str,
                   cpu->code_memory[i].rd, cpu->code_memory[i].rs1,
                   cpu->code_memory[i].rs2, cpu->code_memory[i].imm);
        }
    }

    /* To start fetch stage */
    cpu->fetch.has_insn = TRUE;
    return cpu;
}

/*
 * APEX CPU simulation loop
 *
 * Note: You are free to edit this function according to your implementation
 */
void
APEX_cpu_run(APEX_CPU *cpu)
{
    char user_prompt_val;

    while (TRUE)
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("--------------------------------------------\n");
            printf("Clock Cycle #: %d\n", cpu->clock);
            printf("--------------------------------------------\n");
        }

        if (APEX_writeback(cpu))
        {
            /* Halt in writeback stage */
            printf("APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
            break;
        }

        APEX_memory2(cpu);
        APEX_memory1(cpu);
        if(cpu->ex_int_fu.has_insn){
            APEX_int_fu(cpu);
        }
        if(cpu->ex_mul_fu.has_insn){
            APEX_mul_fu(cpu);
        }
        issueInstruction(cpu);
        APEX_decode(cpu);
        APEX_fetch(cpu);

        print_reg_file(cpu);

        if (cpu->single_step)
        {
            printf("Press any key to advance CPU Clock or <q> to quit:\n");
            scanf("%c", &user_prompt_val);

            if ((user_prompt_val == 'Q') || (user_prompt_val == 'q'))
            {
                printf("APEX_CPU: Simulation Stopped, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
                break;
            }
        }

        cpu->clock++;
    }
}

/*
 * This function deallocates APEX CPU.
 *
 * Note: You are free to edit this function according to your implementation
 */
void
APEX_cpu_stop(APEX_CPU *cpu)
{
    free(cpu->code_memory);
    free(cpu);
}