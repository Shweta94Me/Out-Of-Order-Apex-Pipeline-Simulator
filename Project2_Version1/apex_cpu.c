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
// static void
// print_reg_file(const APEX_CPU *cpu)
// {
//     int i;

//     printf("----------\n%s\n----------\n", "Registers:");

//     for (int i = 0; i < REG_FILE_SIZE / 2; ++i)
//     {
//         printf("R%-3d[%-3d] ", i, cpu->regs[i]);
//     }

//     printf("\n");

//     for (i = (REG_FILE_SIZE / 2); i < REG_FILE_SIZE; ++i)
//     {
//         printf("R%-3d[%-3d] ", i, cpu->regs[i]);
//     }

//     printf("\n");
// }

/*Utility functions*/

node_attr createData(APEX_CPU *cpu)
{
    node_attr data;
    data.pc = cpu->decode.pc;
    strcpy(data.opcode_str, cpu->decode.opcode_str);
    data.opcode = cpu->decode.opcode;
    data.rs1_arch = cpu->decode.rs1;
    data.rs1_tag = cpu->decode.rs1_phy_res;
    data.rs1_ready = cpu->decode.rs1_ready;
    data.rs1_value = cpu->decode.rs1_value;

    data.rs2_arch = cpu->decode.rs2;
    data.rs2_tag = cpu->decode.rs2_phy_res;
    data.rs2_ready = cpu->decode.rs2_ready;
    data.rs2_value = cpu->decode.rs2_value;

    data.rs3_arch = cpu->decode.rs3;
    data.rs3_tag = cpu->decode.rs3_phy_res;
    data.rs3_ready = cpu->decode.rs3_ready;
    data.rs3_value = cpu->decode.rs3_value;

    data.FU_Type = cpu->decode.fu_type;

    data.imm = cpu->decode.imm;
    data.status = 1;

    data.phy_rd = cpu->decode.rd_phy_res;
    data.rd_arch = cpu->decode.rd;

    return data;
}

void updateIQ(APEX_CPU *cpu, enum FU fu_type)
{
    struct node *temp = iq->front;

    while (temp && !isQueueEmpty())
    {
        if (fu_type == Int_FU)
        {
            if (temp->data.rs1_tag == cpu->ex_int_fu.rd_phy_res)
            {
                temp->data.rs1_value = cpu->ex_int_fu.result_buffer;
                temp->data.rs1_ready = 1;
            }
            if (temp->data.rs2_tag == cpu->ex_int_fu.rd_phy_res)
            {
                temp->data.rs2_value = cpu->ex_int_fu.result_buffer;
                temp->data.rs2_ready = 1;
            }
            if (temp->data.rs3_tag == cpu->ex_int_fu.rd_phy_res)
            {
                temp->data.rs3_value = cpu->ex_int_fu.result_buffer;
                temp->data.rs3_ready = 1;
            }
        }
        else if (fu_type == Mul_FU)
        {
            if (temp->data.rs1_tag == cpu->ex_mul_fu.rd_phy_res)
            {
                temp->data.rs1_value = cpu->ex_mul_fu.result_buffer;
                temp->data.rs1_ready = 1;
            }
            if (temp->data.rs2_tag == cpu->ex_mul_fu.rd_phy_res)
            {
                temp->data.rs2_value = cpu->ex_mul_fu.result_buffer;
                temp->data.rs2_ready = 1;
            }
            if (temp->data.rs3_tag == cpu->ex_mul_fu.rd_phy_res)
            {
                temp->data.rs3_value = cpu->ex_mul_fu.result_buffer;
                temp->data.rs3_ready = 1;
            }
        }
        else if (fu_type == Mem_FU)
        {
            if (temp->data.rs1_tag == cpu->mem2.rd_phy_res)
            {
                temp->data.rs1_value = cpu->mem2.result_buffer;
                temp->data.rs1_ready = 1;
            }
            if (temp->data.rs2_tag == cpu->mem2.rd_phy_res)
            {
                temp->data.rs2_value = cpu->mem2.result_buffer;
                temp->data.rs2_ready = 1;
            }
            if (temp->data.rs3_tag == cpu->mem2.rd_phy_res)
            {
                temp->data.rs3_value = cpu->mem2.result_buffer;
                temp->data.rs3_ready = 1;
            }
        }
        temp = temp->next;
    }

    free(temp);
}

/*
Logic for isusing a instruction from the issue queue is based on two conditions if that fu type required by instruction and operands are
available then issue the instruction from the queue
else
iterate in the q and look for the instruction that satisfies this condition
*/

void issueInstruction(APEX_CPU *cpu)
{

    struct node *temp = iq->front;

    // int int_fu_flag = 0; // using this flag to know if we sent any instruction in int fu
    // int mul_fu_flag = 0; // using thus flag to know if we sent any instruction in mul fu

    while (temp && !isQueueEmpty())
    {

        /* suppose we have mul instruction at front of the q and mul fu is free then we issue mul instruction to the mul fu
		but in the same clock cycle we will try to issue instruction that uses int fu if condition to dispatch is satisfied. 
		By this way we can achiveve parallelism in function unit. thats why seperate if instead of if else
		*/

        //0 is free 1 is not free
        if (temp->data.FU_Type == Mul_FU && !cpu->mul_fu_free && temp->data.rs1_ready && temp->data.rs2_ready)
        {
            cpu->mul_fu_free = 1; // making mul fu not free. Also by doing this we ensure only one instruction is sent to int fu , mul fu
            // and ... even if the while loop continues till the end of the q. Early stopping is not possible

            // only copying all the important data from iq to next stage i.e int_fu
            cpu->ex_mul_fu.pc = temp->data.pc;
            cpu->ex_mul_fu.opcode = temp->data.opcode;
            cpu->ex_mul_fu.fu_type = temp->data.FU_Type;
            cpu->ex_mul_fu.has_insn = 1;
            cpu->ex_mul_fu.imm = temp->data.imm;
            strcpy(cpu->ex_mul_fu.opcode_str, temp->data.opcode_str);

            cpu->ex_mul_fu.rd_phy_res = temp->data.phy_rd;
            cpu->ex_mul_fu.rd = temp->data.rd_arch;

            cpu->ex_mul_fu.rs1 = temp->data.rs1_arch;
            cpu->ex_mul_fu.rs1_phy_res = temp->data.rs1_tag;
            cpu->ex_mul_fu.rs1_value = temp->data.rs1_value;
            cpu->ex_mul_fu.rs1_ready = temp->data.rs1_ready;

            cpu->ex_mul_fu.rs2 = temp->data.rs2_arch;
            cpu->ex_mul_fu.rs2_phy_res = temp->data.rs2_tag;
            cpu->ex_mul_fu.rs2_value = temp->data.rs2_value;
            cpu->ex_mul_fu.rs2_ready = temp->data.rs2_ready;

            cpu->ex_mul_fu.rs3 = temp->data.rs3_arch;
            cpu->ex_mul_fu.rs3_phy_res = temp->data.rs3_tag;
            cpu->ex_mul_fu.rs3_value = temp->data.rs3_value;
            cpu->ex_mul_fu.rs3_ready = temp->data.rs3_ready;

            //deleting this node from the q
            deQueueAnyNode(temp->data.pc);

            //setting flag to 1
            // mul_fu_flag = 1;
        }

        if (temp->data.FU_Type == Int_FU && !cpu->int_fu_free && temp->data.rs1_ready && temp->data.rs2_ready)
        {
            cpu->int_fu_free = 1; // making int fu not free. Also by doing this we ensure only one instruction is sent to int fu , mul fu
            // and ... even if the while loop continues till the end of the q. Early stopping is not possible

            // only copying all the important data from iq to next stage i.e int_fu
            cpu->ex_int_fu.pc = temp->data.pc;
            cpu->ex_int_fu.opcode = temp->data.opcode;
            cpu->ex_int_fu.fu_type = temp->data.FU_Type;
            cpu->ex_int_fu.has_insn = 1;
            cpu->ex_int_fu.imm = temp->data.imm;
            strcpy(cpu->ex_int_fu.opcode_str, temp->data.opcode_str);

            cpu->ex_int_fu.rd_phy_res = temp->data.phy_rd;
            cpu->ex_int_fu.rd = temp->data.rd_arch;

            cpu->ex_int_fu.rs1 = temp->data.rs1_arch;
            cpu->ex_int_fu.rs1_phy_res = temp->data.rs1_tag;
            cpu->ex_int_fu.rs1_value = temp->data.rs1_value;
            cpu->ex_int_fu.rs1_ready = temp->data.rs1_ready;

            cpu->ex_int_fu.rs2 = temp->data.rs2_arch;
            cpu->ex_int_fu.rs2_phy_res = temp->data.rs2_tag;
            cpu->ex_int_fu.rs2_value = temp->data.rs2_value;
            cpu->ex_int_fu.rs2_ready = temp->data.rs2_ready;

            cpu->ex_int_fu.rs3 = temp->data.rs3_arch;
            cpu->ex_int_fu.rs3_phy_res = temp->data.rs3_tag;
            cpu->ex_int_fu.rs3_value = temp->data.rs3_value;
            cpu->ex_int_fu.rs3_ready = temp->data.rs3_ready;

            //deleting this node from the q
            deQueueAnyNode(temp->data.pc);
            //setting flag to 1
            // int_fu_flag = 1;
        }
        temp = temp->next;
    }

    //using the above flag to ensure has instruction is made 0 if data is not pushed from iq to fu
    //this help us in apex_cpu.c to decide if we need to run int fu or mul fu or ... unit
    // if (!int_fu_flag)
    // {
    //     cpu->ex_int_fu.has_insn = 0;
    // }
    // if (!mul_fu_flag)
    // {
    //     cpu->ex_mul_fu.has_insn = 0;
    // }

    free(temp);
}

ROB_entry create_ROB_data(APEX_CPU *cpu)
{
    ROB_entry entry;

    entry.pc_value = cpu->decode.pc;
    strcpy(entry.opcode_str, cpu->decode.opcode_str);

    entry.rs1_arch = cpu->decode.rs1;
    entry.rs1_tag = cpu->decode.rs1_phy_res;

    entry.rs2_arch = cpu->decode.rs2;
    entry.rs2_tag = cpu->decode.rs2_phy_res;

    entry.rs3_arch = cpu->decode.rs3;
    entry.rs3_tag = cpu->decode.rs3_phy_res;

    // data.FU_Type = cpu->decode.fu_type;

    entry.imm = cpu->decode.imm;
    entry.status = 1;

    entry.phy_rd = cpu->decode.rd_phy_res;
    entry.rd_arch = cpu->decode.rd;

    return entry;
}
/*Add entry to ROB*/
void add_instr_to_ROB(APEX_CPU *cpu)
{
    ROB_entry data = create_ROB_data(cpu);
    ROB_push(data);
}

/*Update ROB with computed result and make its status valid*/
void update_ROB(CPU_Stage cpu_stage)
{
    forward_to_rob(cpu_stage.pc, cpu_stage.result_buffer);
}

/*Utility functions end*/

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
        cpu->fetch.rs1_phy_res = -1;
        cpu->fetch.rs2 = current_ins->rs2;
        cpu->fetch.rs2_phy_res = -1;
        cpu->fetch.rs3 = current_ins->rs3;
        cpu->fetch.rs3_phy_res = -1;
        cpu->fetch.imm = current_ins->imm;

        /* Update PC for next instruction */
        cpu->pc += 4;

        if (!cpu->decode.stalled)
        {
            /* Copy data from fetch latch to decode latch*/
            cpu->decode = cpu->fetch;
        }
        else
        {
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
    }
    else
    {
        if (!cpu->decode.stalled)
        {
            cpu->fetch.stalled = 0;

            /* Copy data from fetch latch to decode latch*/
            cpu->decode = cpu->fetch;
        }
        if (ENABLE_DEBUG_MESSAGES && cpu->fetch.has_insn)
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
    int val = 0;
    if (fu_type == Int_FU)
    {
        int phy_reg_dest = 0;

        //Shweta ::: Rename source registers
        //MOVC instruction has no source registers
        if (strcmp(cpu->decode.opcode_str, "MOVC") != 0 && strcmp(cpu->decode.opcode_str, "HALT") != 0)
        {
            //Rename with free physical free register from URF
            cpu->decode.rs1_phy_res = renameSrcWithPhyReg(cpu->decode.rs1);
            cpu->decode.rs2_phy_res = renameSrcWithPhyReg(cpu->decode.rs2);

            //Set the ready bit for source regsiter

            val = readSrcFromURF(cpu->decode.rs1_phy_res);
            if (val != -1)
            {
                cpu->decode.rs1_value = val;
                cpu->decode.rs1_ready = 1;
            }
            else
            {
                cpu->decode.rs1_ready = 0;
            }

            val = readSrcFromURF(cpu->decode.rs2_phy_res);
            if (val != -1)
            {
                cpu->decode.rs2_value = val;
                cpu->decode.rs2_ready = 1;
            }
            else
            {
                cpu->decode.rs2_ready = 0;
            }

            //Set the ready bit for third source register to always 1
            cpu->decode.rs3_ready = 1;
        }
        else
        {
            cpu->decode.rs1_ready = 1;
            cpu->decode.rs2_ready = 1;
            cpu->decode.rs3_ready = 1;
        }

        //Shweta ::: Rename destination register
        //CMP instruction has no destination register
        if (strcmp(cpu->decode.opcode_str, "CMP") != 0 && strcmp(cpu->decode.opcode_str, "HALT") != 0)
        {
            phy_reg_dest = allocate_phy_dest_RAT(cpu->decode.rd);
        }

        if (phy_reg_dest != -1)
        {

            //CMP instruction has no destination register
            if (strcmp(cpu->decode.opcode_str, "CMP") != 0 && strcmp(cpu->decode.opcode_str, "HALT") != 0)
                cpu->decode.rd_phy_res = phy_reg_dest;

            //Shweta :::fu_type
            cpu->decode.fu_type = Int_FU;

            //Pass all instructions to Issue Queue
            node_attr data = createData(cpu);
            enQueue(data);

            // adding instruction to rob
            add_instr_to_ROB(cpu);
        }
        else
        {
            //No physical register available in URF
            cpu->decode.stalled = 1;
        }
    }
    else if (fu_type == Mul_FU)
    {
        int phy_reg_dest = 0;
        //Shweta ::: Rename source registers
        //Rename with free physical free register from URF
        cpu->decode.rs1_phy_res = renameSrcWithPhyReg(cpu->decode.rs1);
        cpu->decode.rs2_phy_res = renameSrcWithPhyReg(cpu->decode.rs2);

        //Set the ready bit for source regsiter

        val = readSrcFromURF(cpu->decode.rs1_phy_res);
        if (val != -1)
        {
            cpu->decode.rs1_value = val;
            cpu->decode.rs1_ready = 1;
        }
        else
        {
            cpu->decode.rs1_ready = 0;
        }

        val = readSrcFromURF(cpu->decode.rs2_phy_res);
        if (val != -1)
        {
            cpu->decode.rs2_value = val;
            cpu->decode.rs2_ready = 1;
        }
        else
        {
            cpu->decode.rs2_ready = 0;
        }

        //Set the ready bit for third source register to always 1
        cpu->decode.rs3_ready = 1;

        //Shweta ::: Rename destination register
        phy_reg_dest = allocate_phy_dest_RAT(cpu->decode.rd);
        if (phy_reg_dest != -1)
        {

            cpu->decode.rd_phy_res = phy_reg_dest;

            //Shweta :::fu_type
            cpu->decode.fu_type = Mul_FU;

            node_attr data = createData(cpu);
            enQueue(data);

            // adding instruction to rob
            add_instr_to_ROB(cpu);
        }
        else
        {
            //No physical register available in URF
            cpu->decode.stalled = 1;
        }
    }
    else if (fu_type == Mem_FU)
    {
        int phy_reg_dest = 0;
        //Shweta ::: Rename source registers
        if (strcmp(cpu->decode.opcode_str, "LOAD") == 0 ||
            strcmp(cpu->decode.opcode_str, "LDR") == 0 ||
            strcmp(cpu->decode.opcode_str, "STORE") == 0 ||
            strcmp(cpu->decode.opcode_str, "STR") == 0)
        {
            cpu->decode.rs1_phy_res = renameSrcWithPhyReg(cpu->decode.rs1);
            val = readSrcFromURF(cpu->decode.rs1_phy_res);
            if (val != -1)
            {
                cpu->decode.rs1_value = val;
                cpu->decode.rs1_ready = 1;
            }
            else
            {
                cpu->decode.rs1_ready = 0;
            }
        }

        if (strcmp(cpu->decode.opcode_str, "LDR") == 0 ||
            strcmp(cpu->decode.opcode_str, "STORE") == 0 ||
            strcmp(cpu->decode.opcode_str, "STR") == 0)
        {
            cpu->decode.rs2_phy_res = renameSrcWithPhyReg(cpu->decode.rs2);
            val = readSrcFromURF(cpu->decode.rs2_phy_res);
            if (val != -1)
            {
                cpu->decode.rs2_value = val;
                cpu->decode.rs2_ready = 1;
            }
            else
            {
                cpu->decode.rs2_ready = 0;
            }
        }
        else
        {
            cpu->decode.rs2_ready = 1; //Shweta ::: LOAD instruction
        }

        if (strcmp(cpu->decode.opcode_str, "STR") == 0)
        {
            cpu->decode.rs3_phy_res = renameSrcWithPhyReg(cpu->decode.rs3);
            val = readSrcFromURF(cpu->decode.rs3_phy_res);
            if (val != -1)
            {
                cpu->decode.rs3_value = val;
                cpu->decode.rs3_ready = 1;
            }
            else
            {
                cpu->decode.rs3_ready = 0;
            }
        }
        else
        {
            cpu->decode.rs3_ready = 1; //Shweta ::: LOAD, LDR, STORE instruction
        }

        //Shweta ::: Rename destination register
        //CMP instruction has no destination register
        if (strcmp(cpu->decode.opcode_str, "STORE") != 0 && strcmp(cpu->decode.opcode_str, "STR") != 0)
        {
            phy_reg_dest = allocate_phy_dest_RAT(cpu->decode.rd);
        }

        if (phy_reg_dest != -1)
        {

            //CMP instruction has no destination register
            if (strcmp(cpu->decode.opcode_str, "STORE") != 0 && strcmp(cpu->decode.opcode_str, "STR") != 0)
                cpu->decode.rd_phy_res = phy_reg_dest;

            //Shweta :::fu_type
            cpu->decode.fu_type = Mem_FU;

            //Pass all instructions to Issue Queue
            node_attr data = createData(cpu);
            enQueue(data);

            // adding instruction to rob
            add_instr_to_ROB(cpu);
        }
        else
        {
            //No physical register available in URF
            cpu->decode.stalled = 1;
        }
    }
    else if (fu_type == JBU_FU)
    {
        int phy_reg_dest = 0;
        //Shweta ::: Rename source registers
        if (strcmp(cpu->decode.opcode_str, "JUMP") == 0 ||
            strcmp(cpu->decode.opcode_str, "JAL") == 0)
        {
            cpu->decode.rs1_phy_res = renameSrcWithPhyReg(cpu->decode.rs1);
            val = readSrcFromURF(cpu->decode.rs1_phy_res);
            if (val != -1)
            {
                cpu->decode.rs1_value = val;
                cpu->decode.rs1_ready = 1;
            }
            else
            {
                cpu->decode.rs1_ready = 0;
            }
        }
        else
        {
            //BZ, BNZ
            cpu->decode.rs1_ready = 1;
        }
        //BZ, BNZ, JUMP, JAL
        cpu->decode.rs2_ready = 1;
        cpu->decode.rs3_ready = 1;

        //Shweta ::: Rename destination register
        //Only JAL instruction has destionation register
        if (strcmp(cpu->decode.opcode_str, "JAL") == 0)
        {
            phy_reg_dest = allocate_phy_dest_RAT(cpu->decode.rd);
        }

        if (phy_reg_dest != -1)
        {

            //Only JAL instruction has destionation register
            if (strcmp(cpu->decode.opcode_str, "JAL") == 0)
                cpu->decode.rd_phy_res = phy_reg_dest;

            //Shweta :::fu_type
            cpu->decode.fu_type = JBU_FU;

            //Pass all instructions to Issue Queue
            node_attr data = createData(cpu);
            enQueue(data);

            // adding instruction to rob
            add_instr_to_ROB(cpu);
        }
        else
        {
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
    if (cpu->decode.has_insn)
    {
        if (!isQueueFull() && !cpu->stoppedDispatch && !ROB_is_full())
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

                if (!cpu->decode.stalled)
                {
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

                if (!cpu->decode.stalled)
                {
                    /* Copy data from decode latch to execute latch*/
                    // cpu->mem1 = cpu->decode;
                    cpu->decode.has_insn = FALSE;
                }
                break;
            }

            case OPCODE_MUL:
            {
                dispatch_instr_to_IQ(cpu, Mul_FU);

                if (!cpu->decode.stalled)
                {
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

            case OPCODE_BZ:
            case OPCODE_BNZ:
            case OPCODE_JAL:
            case OPCODE_JUMP:
            {
                dispatch_instr_to_IQ(cpu, JBU_FU);
                if (!cpu->decode.stalled)
                {
                    ///Create we are going to stopped dispatching
                    cpu->stoppedDispatch = 1;
                    cpu->decode.has_insn = FALSE; //As we have pushed branch instruction to Issue Queue clear the decode stage
                }
                break;
            }
            }

            if (ENABLE_DEBUG_MESSAGES)
            {
                print_stage_content("Decode/RF", &cpu->decode);
            }
        }
        else
        {
            cpu->decode.stalled = 1;
            if (ENABLE_DEBUG_MESSAGES)
            {
                print_stage_content("Decode/RF", &cpu->decode);
            }
        }
    }
}

//Broadcasting mechanism also know as data forwarding
/*Here we will broadcast destination register value for those instructions who are waiting in issue queue with same
source regsiter as this destination register*/
void broadcastData(APEX_CPU *cpu, int result, int phy_res, enum FU fu_type)
{
    //1: Make entry in URF
    updateURF(result, phy_res, fu_type);

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
    issueInstruction(cpu);

    if (cpu->ex_int_fu.has_insn)
    {
        /* Execute logic based on instruction type */
        switch (cpu->ex_int_fu.opcode)
        {
        case OPCODE_ADD:
        {
            cpu->int_fu_free = 1;
            cpu->ex_int_fu.result_buffer = cpu->ex_int_fu.rs1_value + cpu->ex_int_fu.rs2_value;

            broadcastData(cpu, cpu->ex_int_fu.result_buffer, cpu->ex_int_fu.rd_phy_res, Int_FU);

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
            cpu->ex_int_fu.result_buffer = cpu->ex_int_fu.rs1_value - cpu->ex_int_fu.rs2_value;

            broadcastData(cpu, cpu->ex_int_fu.result_buffer, cpu->ex_int_fu.rd_phy_res, Int_FU);

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
            cpu->ex_int_fu.result_buffer = cpu->ex_int_fu.rs1_value & cpu->ex_int_fu.rs2_value;

            broadcastData(cpu, cpu->ex_int_fu.result_buffer, cpu->ex_int_fu.rd_phy_res, Int_FU);
            break;
        }

        case OPCODE_OR:
        {
            cpu->int_fu_free = 1;
            cpu->ex_int_fu.result_buffer = cpu->ex_int_fu.rs1_value | cpu->ex_int_fu.rs2_value;

            broadcastData(cpu, cpu->ex_int_fu.result_buffer, cpu->ex_int_fu.rd_phy_res, Int_FU);
            break;
        }

        case OPCODE_XOR:
        {
            cpu->int_fu_free = 1;
            cpu->ex_int_fu.result_buffer = cpu->ex_int_fu.rs1_value ^ cpu->ex_int_fu.rs2_value;

            broadcastData(cpu, cpu->ex_int_fu.result_buffer, cpu->ex_int_fu.rd_phy_res, Int_FU);
            break;
        }

        case OPCODE_MOVC:
        {
            cpu->int_fu_free = 1;
            cpu->ex_int_fu.result_buffer = cpu->ex_int_fu.imm;

            broadcastData(cpu, cpu->ex_int_fu.result_buffer, cpu->ex_int_fu.rd_phy_res, Int_FU);
            break;
        }

        case OPCODE_ADDL:
        {
            cpu->int_fu_free = 1;
            cpu->ex_int_fu.result_buffer = cpu->ex_int_fu.rs1_value + cpu->ex_int_fu.imm;

            broadcastData(cpu, cpu->ex_int_fu.result_buffer, cpu->ex_int_fu.rd_phy_res, Int_FU);
            break;
        }

        case OPCODE_SUBL:
        {
            cpu->int_fu_free = 1;
            cpu->ex_int_fu.result_buffer = cpu->ex_int_fu.rs1_value - cpu->ex_int_fu.imm;

            broadcastData(cpu, cpu->ex_int_fu.result_buffer, cpu->ex_int_fu.rd_phy_res, Int_FU);
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
            else
            {
                cpu->zero_flag = FALSE;
            }
            break;
        }
        }

        /* Copy data from execute latch to memory latch*/
        // cpu->memory = cpu->ex_int_fu;  //Shweta ::: Instead of passing it to memory update ROB entry

        update_ROB(cpu->ex_int_fu);
        //Clear int unit stage
        cpu->ex_int_fu.has_insn = FALSE;
        cpu->insn_completed++;
        cpu->int_fu_free = 0;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Execute INT_FU", &cpu->ex_int_fu);
        }
    }
}

static void
APEX_mul_fu(APEX_CPU *cpu)
{
    issueInstruction(cpu);
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
            cpu->ex_mul_fu.result_buffer = cpu->ex_mul_fu.rs1_value * cpu->ex_mul_fu.rs2_value;

            cpu->mul_cycles = 0;  //Reset
            cpu->mul_fu_free = 0; //free mul unit

            broadcastData(cpu, cpu->ex_mul_fu.result_buffer, cpu->ex_mul_fu.rd_phy_res, Mul_FU); // only when completed 3 cycles

            /* Copy data from execute latch to memory latch*/
            update_ROB(cpu->ex_mul_fu); //Shweta ::: Instead of passing it to memory update ROB entry

            break;
        }
        }

        cpu->insn_completed++;
        cpu->ex_mul_fu.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Execute MUL_FU", &cpu->ex_mul_fu);
        }
    }
    else if (cpu->ex_mul_fu.has_insn)
    {
        cpu->mul_cycles += 1;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Execute MUL_FU", &cpu->ex_mul_fu);
        }
    }
}

static void
APEX_jbu1(APEX_CPU *cpu)
{
    if (cpu->jbu1.has_insn)
    {
        switch (cpu->jbu1.opcode)
        {
        case OPCODE_JUMP:
        case OPCODE_JAL:
        {
            //Shweta ::: Need clarification what needs to be done in this stage?
            /* Copy data from jbu1 latch to jbu2 latch*/
            cpu->jbu2 = cpu->jbu1;
            cpu->jbu1.has_insn = FALSE;
            break;
        }

        case OPCODE_BZ:
        {
            if (cpu->zero_flag == TRUE)
            {
                /* Calculate new PC, and send it to fetch unit */
                cpu->pc = cpu->jbu1.pc + cpu->jbu1.imm;

                /* Since we are using reverse callbacks for pipeline stages, 
                            * this will prevent the new instruction from being fetched in the current cycle*/
                cpu->fetch_from_next_cycle = TRUE;

                /* Flush previous stages */
                cpu->decode.has_insn = FALSE;

                /* Make sure fetch stage is enabled to start fetching from new PC */
                cpu->fetch.has_insn = TRUE;

                /*Shweta ::: Open for dispatching*/
                cpu->stoppedDispatch = 0;
                cpu->insn_completed++;
                cpu->jbu1.has_insn = FALSE;
            }
            break;
        }

        case OPCODE_BNZ:
        {
            if (cpu->zero_flag == FALSE)
            {
                /* Calculate new PC, and send it to fetch unit */
                cpu->pc = cpu->jbu1.pc + cpu->jbu1.imm;

                /* Since we are using reverse callbacks for pipeline stages, 
                            * this will prevent the new instruction from being fetched in the current cycle*/
                cpu->fetch_from_next_cycle = TRUE;

                /* Flush previous stages */
                cpu->decode.has_insn = FALSE;

                /* Make sure fetch stage is enabled to start fetching from new PC */
                cpu->fetch.has_insn = TRUE;

                /*Shweta ::: Open for dispatching*/
                cpu->stoppedDispatch = 0;
                cpu->insn_completed++;
                cpu->jbu1.has_insn = FALSE;
            }
            break;
        }
        }

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("JBU1", &cpu->jbu1);
        }
    }
}

static void
APEX_jbu2(APEX_CPU *cpu)
{
    if (cpu->jbu1.has_insn)
    {
        switch (cpu->jbu1.opcode)
        {
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
        }

        /*Shweta ::: Open for dispatching*/
        cpu->stoppedDispatch = 0;
        update_ROB(cpu->jbu2);
        cpu->insn_completed++;
        cpu->jbu2.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("JBU1", &cpu->jbu1);
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

            broadcastData(cpu, cpu->mem2.result_buffer, cpu->mem2.rd_phy_res, Mem_FU);
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

        cpu->insn_completed++;
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
    createQueue();

    // Initiliaze URF
    initializeURF();

    // Initialize RAT
    initializeRAT();

    // Initialize RRAT
    initializeRRAT();

    //Initialize ROB. The ROB will be accessed from here but like issue q we have not assigned it to
    // the cpu just try to keep seperate.
    createROB();

    if (ENABLE_DEBUG_MESSAGES)
    {
        fprintf(stderr,
                "APEX_CPU: Initialized APEX CPU, loaded %d instructions\n",
                cpu->code_memory_size);
        fprintf(stderr, "APEX_CPU: PC initialized to %d\n", cpu->pc);
        fprintf(stderr, "APEX_CPU: Printing Code Memory\n");
        printf("%-9s %-9s %-9s %-9s %-9s %-9s\n", "opcode_str", "rd", "rs1", "rs2", "rs3",
               "imm");

        for (i = 0; i < cpu->code_memory_size; ++i)
        {
            printf("%-9s %-9d %-9d %-9d %-9d %-9d\n", cpu->code_memory[i].opcode_str,
                   cpu->code_memory[i].rd, cpu->code_memory[i].rs1,
                   cpu->code_memory[i].rs2, cpu->code_memory[i].rs3, cpu->code_memory[i].imm);
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
void APEX_cpu_run(APEX_CPU *cpu)
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

        /*Shweta ::: Stop excutation once reached code_memory_size*/
        if (cpu->insn_completed == cpu->code_memory_size)
        {
            /* Halt in writeback stage */
            printf("APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
            break;
        }

        //Commit from ROB head and update RRAT with commited architectural register
        //check ROB head status is valid
        int phy_rd = ROB_headEntryValid();
        if (phy_rd != -1)
        {
            int rd_arch_idx = ROB_pop();
            updateRRAT(phy_rd, rd_arch_idx);
        }

        APEX_memory2(cpu);
        APEX_memory1(cpu);
        APEX_jbu2(cpu);
        APEX_jbu1(cpu);

        APEX_int_fu(cpu);

        APEX_mul_fu(cpu);

        APEX_decode(cpu);
        APEX_fetch(cpu);

        // print_reg_file(cpu);

        /*Shweta ::: Print Issue/ROB/RAT/RRAT entries*/
        printQueue();

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
void APEX_cpu_stop(APEX_CPU *cpu)
{
    free(cpu->code_memory);
    free(cpu);
}