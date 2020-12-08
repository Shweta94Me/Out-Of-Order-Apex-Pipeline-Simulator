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
        printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rs1, stage->rs2,
               stage->rs3);
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

    data.branch_tag = cpu->decode.branch_tag;

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
        else if (fu_type == JBU_FU)
        {
            if (temp->data.rs1_tag == cpu->jbu2.rd_phy_res)
            {
                temp->data.rs1_value = cpu->jbu2.result_buffer;
                temp->data.rs1_ready = 1;
            }
            if (temp->data.rs2_tag == cpu->jbu2.rd_phy_res)
            {
                temp->data.rs2_value = cpu->jbu2.result_buffer;
                temp->data.rs2_ready = 1;
            }
            if (temp->data.rs3_tag == cpu->jbu2.rd_phy_res)
            {
                temp->data.rs3_value = cpu->jbu2.result_buffer;
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
    struct node *prev = NULL;

    // int int_fu_flag = 0; // using this flag to know if we sent any instruction in int fu
    // int mul_fu_flag = 0; // using thus flag to know if we sent any instruction in mul fu

    while (temp != NULL && !isQueueEmpty())
    {

        int nodeDeleted = 0;
        /* suppose we have mul instruction at front of the q and mul fu is free then we issue mul instruction to the mul fu
		but in the same clock cycle we will try to issue instruction that uses int fu if condition to dispatch is satisfied. 
		By this way we can achiveve parallelism in function unit. thats why seperate if instead of if else
		*/

        //0 is free 1 is not free
        if (temp != NULL && temp->data.FU_Type == Mul_FU && !cpu->ex_mul_fu.has_insn && temp->data.rs1_ready && temp->data.rs2_ready)
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

            // delete node
            if (temp->next == NULL)
            {
                if (prev == NULL)
                {
                    //Only one node
                    iq->front = NULL;
                    iq->rear = NULL;
                    // free(temp);
                    temp = NULL;
                }
                else
                {
                    //last Node
                    prev->next = temp->next;
                    // free(temp);
                    temp = NULL;
                    iq->rear = prev;
                }
            }
            else if (temp->next != NULL)
            {
                if (prev == NULL)
                {
                    //first node
                    struct node *temp1 = temp->next;
                    temp->data = temp1->data;
                    temp->next = temp1->next;
                    free(temp1);
                }
                else
                {
                    prev->next = temp->next;
                    free(temp);
                    temp = prev->next;
                }
            }
            iq->sizeOfQueue--;
            nodeDeleted = 1;
        }

        if (temp != NULL && temp->data.FU_Type == Int_FU && !cpu->ex_int_fu.has_insn && temp->data.rs1_ready && temp->data.rs2_ready)
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

            // delete node
            if (temp->next == NULL)
            {
                if (prev == NULL)
                {
                    //Only one node
                    iq->front = NULL;
                    iq->rear = NULL;
                    // free(temp);
                    temp = NULL;
                }
                else
                {
                    //last Node
                    prev->next = temp->next;
                    // free(temp);
                    temp = NULL;
                    iq->rear = prev;
                }
            }
            else if (temp->next != NULL)
            {
                if (prev == NULL)
                {
                    //first node
                    struct node *temp1 = temp->next;
                    temp->data = temp1->data;
                    temp->next = temp1->next;
                    free(temp1);
                }
                else
                {
                    prev->next = temp->next;
                    free(temp);
                    temp = prev->next;
                }
            }
            iq->sizeOfQueue--;
            nodeDeleted = 1;
        }

        if (temp != NULL && temp->data.FU_Type == Mem_FU && temp->data.rs1_ready && temp->data.rs2_ready && temp->data.rs3_ready)
        {
            set_rob_mready_bit(temp->data.pc);

            // delete node
            if (temp->next == NULL)
            {
                if (prev == NULL)
                {
                    //Only one node
                    iq->front = NULL;
                    iq->rear = NULL;
                    // free(temp);
                    temp = NULL;
                }
                else
                {
                    //last Node
                    prev->next = temp->next;
                    // free(temp);
                    temp = NULL;
                    iq->rear = prev;
                }
            }
            else if (temp->next != NULL)
            {
                if (prev == NULL)
                {
                    //first node
                    struct node *temp1 = temp->next;
                    temp->data = temp1->data;
                    temp->next = temp1->next;
                    free(temp1);
                }
                else
                {
                    prev->next = temp->next;
                    free(temp);
                    temp = prev->next;
                }
            }
            iq->sizeOfQueue--;
            nodeDeleted = 1;
        }

        if (temp != NULL && temp->data.FU_Type == JBU_FU && !cpu->jbu1.has_insn && temp->data.rs1_ready && temp->data.rs2_ready && temp->data.rs3_ready)
        {
            cpu->jbu1.pc = temp->data.pc;
            cpu->jbu1.opcode = temp->data.opcode;
            cpu->jbu1.fu_type = temp->data.FU_Type;
            cpu->jbu1.has_insn = 1;
            cpu->jbu1.imm = temp->data.imm;
            strcpy(cpu->jbu1.opcode_str, temp->data.opcode_str);

            cpu->jbu1.rd_phy_res = temp->data.phy_rd;
            cpu->jbu1.rd = temp->data.rd_arch;

            cpu->jbu1.rs1 = temp->data.rs1_arch;
            cpu->jbu1.rs1_phy_res = temp->data.rs1_tag;
            cpu->jbu1.rs1_value = temp->data.rs1_value;
            cpu->jbu1.rs1_ready = temp->data.rs1_ready;

            cpu->jbu1.rs2 = temp->data.rs2_arch;
            cpu->jbu1.rs2_phy_res = temp->data.rs2_tag;
            cpu->jbu1.rs2_value = temp->data.rs2_value;
            cpu->jbu1.rs2_ready = temp->data.rs2_ready;

            cpu->jbu1.rs3 = temp->data.rs3_arch;
            cpu->jbu1.rs3_phy_res = temp->data.rs3_tag;
            cpu->jbu1.rs3_value = temp->data.rs3_value;
            cpu->jbu1.rs3_ready = temp->data.rs3_ready;

            // delete node
            if (temp->next == NULL)
            {
                if (prev == NULL)
                {
                    //Only one node
                    iq->front = NULL;
                    iq->rear = NULL;
                    // free(temp);
                    temp = NULL;
                }
                else
                {
                    //last Node
                    prev->next = temp->next;
                    // free(temp);
                    temp = NULL;
                    iq->rear = prev;
                }
            }
            else if (temp->next != NULL)
            {
                if (prev == NULL)
                {
                    //first node
                    struct node *temp1 = temp->next;
                    temp->data = temp1->data;
                    temp->next = temp1->next;
                    free(temp1);
                }
                else
                {
                    prev->next = temp->next;
                    free(temp);
                    temp = prev->next;
                }
            }
            iq->sizeOfQueue--;
            nodeDeleted = 1;
        }

        if (!nodeDeleted)
        {
            if (!isQueueEmpty())
            {
                prev = temp;
                temp = temp->next;
            }
            else
            {
                temp = NULL;
            }
        }
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

ROB_entry create_ROB_data(APEX_CPU *cpu, int mready)
{
    ROB_entry entry;

    entry.pc_value = cpu->decode.pc;
    strcpy(entry.opcode_str, cpu->decode.opcode_str);
    entry.opcode = cpu->decode.opcode;

    entry.rs1_arch = cpu->decode.rs1;
    entry.rs1_value = cpu->decode.rs1_value;
    entry.rs1_tag = cpu->decode.rs1_phy_res;

    entry.rs2_arch = cpu->decode.rs2;
    entry.rs2_value = cpu->decode.rs2_value;
    entry.rs2_tag = cpu->decode.rs2_phy_res;

    entry.rs3_arch = cpu->decode.rs3;
    entry.rs3_value = cpu->decode.rs3_value;
    entry.rs3_tag = cpu->decode.rs3_phy_res;

    // data.FU_Type = cpu->decode.fu_type;

    entry.imm = cpu->decode.imm;

    if (strcmp(cpu->decode.opcode_str, "HALT") == 0)
        entry.status = 1;
    else
        entry.status = 0;

    entry.phy_rd = cpu->decode.rd_phy_res;
    entry.rd_arch = cpu->decode.rd;

    entry.mready = mready;

    entry.branch_tag = cpu->decode.branch_tag;

    return entry;
}
/*Add entry to ROB*/
void add_instr_to_ROB(APEX_CPU *cpu, int mready)
{
    ROB_entry data = create_ROB_data(cpu, mready);
    ROB_push(data);
}

/*Update ROB with computed result and make its status valid*/
void update_ROB(CPU_Stage cpu_stage)
{
    forward_to_rob(cpu_stage.pc, cpu_stage.result_buffer);
}

// push instruction from rob to mem stage if instruction is is mem stage ins like load store ldr str
// and mready bit is set. mready bit is used for mem type ins only and not r2r ins

void pass_to_mem_stage(APEX_CPU *cpu, ROB_entry entry)
{

    //if the mem ins like ldr str store load and mready is one lets pop and pass to rob

    // let pass the rob entry to mem stage 1

    cpu->mem1.fu_type = Mem_FU;
    cpu->mem1.has_insn = 1;
    cpu->mem1.imm = entry.imm;
    cpu->mem1.opcode = entry.opcode;
    strcpy(cpu->mem1.opcode_str, entry.opcode_str);
    cpu->mem1.pc = entry.pc_value;

    cpu->mem1.rd = entry.rd_arch;
    cpu->mem1.rd_phy_res = entry.phy_rd;

    // cpu->mem1.result_buffer
    cpu->mem1.rs1 = entry.rs1_arch;
    cpu->mem1.rs1_value = entry.rs1_value;
    cpu->mem1.rs1_phy_res = entry.rs1_tag;
    cpu->mem1.rs1_ready = 1; //Shweta ::: remove if not working properly

    cpu->mem1.rs2 = entry.rs2_arch;
    cpu->mem1.rs2_value = entry.rs2_value;
    cpu->mem1.rs2_phy_res = entry.rs2_tag;
    cpu->mem1.rs2_ready = 1;

    cpu->mem1.rs3 = entry.rs3_arch;
    cpu->mem1.rs3_phy_res = entry.rs3_tag;
    cpu->mem1.rs3_value = entry.rs3_value;
    cpu->mem1.rs3_ready = 1;
}

void printMemory(APEX_CPU *cpu)
{
    printf("===============STATE OF DATA MEMORY===============\n");
    for (int i = 0; i < 8; i++)
    {
        printf("|\t\tD[%d]\t\t|\t\tData Value = %d\t\t|\n", i, cpu->data_memory[i]);
    }
}

/*Make instruction entry to Issue Queue*/
void dispatch(APEX_CPU *cpu)
{
    /*Shweta ::: Branch prediction and speculative execution
    1. For non branch instruction - get the BIS top index and use it as a branch tag
    2. For branch instruction:
        i. Check whether BIS is not full
        ii. Create new entry in BIS 
        iii. If running branch instruction doesn't exist in BTB then add it to BTB
        iv. Pass the BIS top index to IQ
        v. Set the ROB tail pointer to newly created BIS entry
    */
    if (cpu->decode.has_insn && !cpu->decode.stalled)
    {
        if (cpu->decode.fu_type == Int_FU)
        {
            //Shweta ::: Do not push HALT instrc to IQ; only add entry to ROB
            if (strcmp(cpu->decode.opcode_str, "HALT") != 0)
            {
                int branch_tag = get_BIS_top_idx();
                cpu->decode.branch_tag = branch_tag;
                //Pass all instructions to Issue Queue
                node_attr data = createData(cpu);
                enQueue(data);
            }
            // adding instruction to rob
            add_instr_to_ROB(cpu, 0);
            cpu->decode.has_insn = FALSE;
        }
        else if (cpu->decode.fu_type == Mul_FU)
        {
            int branch_tag = get_BIS_top_idx();
            cpu->decode.branch_tag = branch_tag;
            node_attr data = createData(cpu);
            enQueue(data);
            // adding instruction to rob
            add_instr_to_ROB(cpu, 0);
            cpu->decode.has_insn = FALSE;
        }
        else if (cpu->decode.fu_type == Mem_FU)
        {
            int branch_tag = get_BIS_top_idx();
            cpu->decode.branch_tag = branch_tag;

            //Shweta ::: If one of the source operand is not read then add instruction to Issue Queue
            int mready = 1; //1 - set and 0 - not set
            if ((strcmp(cpu->decode.opcode_str, "LOAD") == 0 && !cpu->decode.rs1_ready) ||
                (strcmp(cpu->decode.opcode_str, "LDR") == 0 && (!cpu->decode.rs1_ready || !cpu->decode.rs2_ready)) ||
                (strcmp(cpu->decode.opcode_str, "STORE") == 0 && (!cpu->decode.rs1_ready || !cpu->decode.rs2_ready)) ||
                (strcmp(cpu->decode.opcode_str, "STR") == 0 && (!cpu->decode.rs1_ready || !cpu->decode.rs2_ready || !cpu->decode.rs3_ready)))
            {
                mready = 0;
                //Pass all instructions to Issue Queue
                node_attr data = createData(cpu);
                enQueue(data);
            }

            // adding instruction to rob
            add_instr_to_ROB(cpu, mready);
            cpu->decode.has_insn = FALSE;
        }
        else if (cpu->decode.fu_type == JBU_FU)
        {
            //Check if BTB entry exist for this branch else create a new one
            if (!BTB_entry_exist(cpu->decode.pc))
            {
                BTB_push(cpu->decode.pc);
            }
            BIS_incr_top(); //Create new entry in BIS with incremented top index
            int branch_tag = get_BIS_top_idx();
            cpu->decode.branch_tag = branch_tag;

            //Pass all instructions to Issue Queue
            node_attr data = createData(cpu);
            enQueue(data);

            // adding instruction to rob
            add_instr_to_ROB(cpu, 0);

            //Add ROB tail pointer to newly created BIS entry
            bis->bis_entry[branch_tag].rob_entry = rob->tail;

            ///Now Checkpoint the RAT and URF table and update the BIS entry to point new created checkpoint tables
            ///Siddhesh is working on it now
            if(strcmp(cpu->decode.opcode_str, "JUMP") == 0 || strcmp(cpu->decode.opcode_str, "JAL") == 0)
                cpu->stoppedDispatch = 1;

            // checkpoint rat and urf and provide the index
            int rat_checkpoint_idx = insertCheckpointRat();
            int urf_checkpoint_idx = insertCheckpointURF();

            if(rat_checkpoint_idx != -1 && urf_checkpoint_idx != -1){
                bis->bis_entry[branch_tag].checkpoint_rat_idx = rat_checkpoint_idx;
                bis->bis_entry[branch_tag].checkpoint_urf_idx = urf_checkpoint_idx;
            }
            cpu->decode.has_insn = FALSE;
        }
    }
}

void renameRegister(APEX_CPU *cpu)
{
    int val = 0;
    cpu->decode.stalled = 0;
    if (cpu->decode.fu_type == Int_FU)
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
        }
        else
        {
            //No physical register available in URF
            cpu->decode.stalled = 1;
        }
    }
    else if (cpu->decode.fu_type == Mul_FU)
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
        }
        else
        {
            //No physical register available in URF
            cpu->decode.stalled = 1;
        }
    }
    else if (cpu->decode.fu_type == Mem_FU)
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
        }
        else
        {
            //No physical register available in URF
            cpu->decode.stalled = 1;
        }
    }
    else if (cpu->decode.fu_type == JBU_FU)
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
        }
        else
        {
            //No physical register available in URF
            cpu->decode.stalled = 1;
        }
    }
}

void flush(int branch_tag, int target_addr)
{

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

        //Lookup in BTB for this branch instruction
        int target_addrs = BTB_lookup(cpu->fetch.pc); //Pass current stage pc
        if (target_addrs != -1)
            cpu->pc = target_addrs;
        else
            cpu->pc += 4; /* Update PC for next instruction */

        if (!cpu->decode.stalled)
        {
            /* Copy data from fetch latch to decode latch*/
            cpu->decode = cpu->fetch;
        }
        else
        {
            cpu->fetch.stalled = 1;
        }

        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            print_stage_content("Fetch", &cpu->fetch);
        }

        /* Stop fetching new instructions if HALT is fetched */
        if (cpu->fetch.opcode == OPCODE_HALT)
        {
            cpu->fetch.has_insn = FALSE;
        }
    }
    else if (cpu->fetch.has_insn)
    {
        if (!cpu->decode.stalled)
        {
            cpu->fetch.stalled = 0;

            /* Copy data from fetch latch to decode latch*/
            cpu->decode = cpu->fetch;
        }
        if (ENABLE_DEBUG_MESSAGES && cpu->fetch.has_insn && !cpu->simulate)
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
        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            printf("Fetch: Empty\n");
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
            cpu->decode.fu_type = Int_FU;
            if (!isQueueFull() && !ROB_is_full() && !cpu->stoppedDispatch)
            {
                renameRegister(cpu);
                if (!cpu->decode.stalled)
                {
                    dispatch(cpu);
                }
                else
                {
                    cpu->decode.stalled = 1;
                }
            }
            else
            {
                cpu->decode.stalled = 1;
            }
            break;
        }

        case OPCODE_LOAD:
        case OPCODE_LDR:
        case OPCODE_STORE:
        case OPCODE_STR:
        {
            cpu->decode.fu_type = Mem_FU;
            if (!isQueueFull() && !ROB_is_full() && !cpu->stoppedDispatch)
            {
                renameRegister(cpu);
                if (!cpu->decode.stalled)
                {
                    dispatch(cpu);
                }
                else
                {
                    cpu->decode.stalled = 1;
                }
            }
            else
            {
                cpu->decode.stalled = 1;
            }
            break;
        }

        case OPCODE_MUL:
        {
            cpu->decode.fu_type = Mul_FU;
            if (!isQueueFull() && !ROB_is_full() && !cpu->stoppedDispatch)
            {
                renameRegister(cpu);
                if (!cpu->decode.stalled)
                {
                    dispatch(cpu);
                }
                else
                {
                    cpu->decode.stalled = 1;
                }
            }
            else
            {
                cpu->decode.stalled = 1;
            }
            break;
        }

        case OPCODE_HALT:
        {
            cpu->decode.fu_type = Int_FU;
            if (!cpu->decode.stalled)
            {
                if (!isQueueFull() && !ROB_is_full() && !cpu->stoppedDispatch)
                {
                    dispatch(cpu);
                }
                else
                {
                    cpu->decode.stalled = 1;
                }
            }
            break;
        }

        case OPCODE_BZ:
        case OPCODE_BNZ:
        case OPCODE_JAL:
        case OPCODE_JUMP:
        {
            cpu->decode.fu_type = JBU_FU;
            int BTB_decision = 0;
            if (BTB_entry_exist(cpu->decode.pc) == -1 && BTB_is_full())
            {
                BTB_decision = 1; ///Rename and dispatching should not happen
            }
            if (!BTB_decision && !isQueueFull() && !ROB_is_full() && !cpu->stoppedDispatch && !BIS_is_full())
            {
                renameRegister(cpu);
                if (!cpu->decode.stalled)
                {
                    dispatch(cpu);
                }
                else
                {
                    cpu->decode.stalled = 1;
                }
            }
            else
            {
                cpu->decode.stalled = 1;
            }
            break;
        }
        }
        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            print_stage_content("Decode/RF", &cpu->decode);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            printf("Decode/RF: Empty\n");
        }
    }
}

//Broadcasting mechanism also know as data forwarding
/*Here we will broadcast destination register value for those instructions who are waiting in issue queue with same
source regsiter as this destination register*/
void broadcastData(APEX_CPU *cpu, int result, int phy_res, enum FU fu_type)
{
    //1: Make entry in URF
    updateURF(result, phy_res);

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
            cpu->ex_int_fu.result_buffer = cpu->ex_int_fu.rs1_value + cpu->ex_int_fu.rs2_value;

            broadcastData(cpu, cpu->ex_int_fu.result_buffer, cpu->ex_int_fu.rd_phy_res, Int_FU);
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

        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            print_stage_content("Execute INT_FU", &cpu->ex_int_fu);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            printf("INT_FU: Empty\n");
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

        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            print_stage_content("Execute MUL_FU", &cpu->ex_mul_fu);
        }
    }
    else if (cpu->ex_mul_fu.has_insn)
    {
        cpu->mul_cycles += 1;

        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            print_stage_content("Execute MUL_FU", &cpu->ex_mul_fu);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            printf("MUL_FU: Empty\n");
        }
    }
}

static void
APEX_jbu1(APEX_CPU *cpu)
{
    /*Shweta ::: 
            Decision for misprediction
            1. Set the target address
            2. BZ - If history bit = 0 and zero flag is set ---> Mispredicted branch ---> Flush + bring instr from new target address
            3. BZ - If history bit = 1 and zero flag is not set ---> Mispredicted branch ---> Flush + bring instr from new target address
            4. Not a mispredicted branch --> Mean our prediction is correct --> do nothing your good to move
            5. If instrc is BZ and zero flag is set --> set the history bit = 1 else 0
            6. If instrc is BNZ and zero flag is not set --> set the history bit = 1 else 0
            */
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
            //Update the BTB target address
            int btb_entry_idx = BTB_update_target_addr(cpu->jbu1.pc, cpu->jbu1.pc + cpu->jbu1.imm);

            if ((btb_entry_idx != -1 && btb->btb_entry[btb_entry_idx].branch_direction == 0 && cpu->zero_flag == TRUE) ||
                (btb_entry_idx != -1 && btb->btb_entry[btb_entry_idx].branch_direction == 1 && cpu->zero_flag == FALSE))
            {
                //Mispredicted branch
                //Broadcase this branch instruction tag and Flush unwanted instruction from everywhere and
                //bring new instruction with new target address
                flush(cpu->jbu1.branch_tag, btb->btb_entry[btb_entry_idx].target_addrs);

                //For future prediction
                if (cpu->zero_flag == TRUE)
                {
                    btb->btb_entry[btb_entry_idx].branch_direction = 1;
                }
                else
                {
                    btb->btb_entry[btb_entry_idx].branch_direction = 0;
                }
            }
            //Prediction was correct -- Go with usual flow
            /*Shweta ::: Open for dispatching*/
            update_ROB(cpu->jbu1);
            //No need to unstalled decode or fetch stage as we didn't stop dispatch
            cpu->insn_completed++;
            cpu->jbu1.has_insn = FALSE;
            break;
        }

        case OPCODE_BNZ:
        {
            //Update the BTB target address
            int btb_entry_idx = BTB_update_target_addr(cpu->jbu1.pc, cpu->jbu1.pc + cpu->jbu1.imm);

            if ((btb_entry_idx != -1 && btb->btb_entry[btb_entry_idx].branch_direction == 0 && cpu->zero_flag == FALSE) ||
                (btb_entry_idx != -1 && btb->btb_entry[btb_entry_idx].branch_direction == 1 && cpu->zero_flag == TRUE))
            {
                //Mispredicted branch
                //Broadcase this branch instruction tag and Flush unwanted instruction from everywhere and
                //bring new instruction with new target address
                flush(cpu->jbu1.branch_tag, btb->btb_entry[btb_entry_idx].target_addrs);
                //For future prediction
                if (cpu->zero_flag == FALSE)
                {
                    btb->btb_entry[btb_entry_idx].branch_direction = 1;
                }
                else
                {
                    btb->btb_entry[btb_entry_idx].branch_direction = 0;
                }
            }
            //Prediction was correct -- Go with usual flow
            /*Shweta ::: Open for dispatching*/
            update_ROB(cpu->jbu1);
            //No need to unstalled decode or fetch stage as we didn't stop dispatch
            cpu->insn_completed++;
            cpu->jbu1.has_insn = FALSE;
            break;
        }
        }

        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            print_stage_content("JBU1", &cpu->jbu1);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            printf("JBU1: Empty\n");
        }
    }
}

static void
APEX_jbu2(APEX_CPU *cpu)
{
    if (cpu->jbu2.has_insn)
    {
        switch (cpu->jbu2.opcode)
        {
        case OPCODE_JUMP:
        {
            int reg_tag = jal_peek(jalstk);
            if (reg_tag != INT_MIN && reg_tag == cpu->jbu2.rs1)
            {
                int reg_val = jal_pop(&jalstk);
                cpu->jbu2.rs1_value = reg_val;
            }

            /*Shweta ::: Calculate the new PC and send it to fetch unit*/
            cpu->pc = cpu->jbu2.rs1_value + cpu->jbu2.imm;

            cpu->fetch_from_next_cycle = TRUE;

            /*Flush previous stages*/
            cpu->decode.has_insn = FALSE;

            /*Enable fetch stage to start fetching from new PC*/
            cpu->fetch.has_insn = TRUE;

            update_ROB(cpu->jbu2); //Shweta ::: Instead of passing it to memory update ROB entry
            break;
        }

        case OPCODE_JAL:
        {
            cpu->jbu2.result_buffer = cpu->jbu2.pc + 4;
            cpu->pc = cpu->jbu2.rs1_value + cpu->jbu2.imm;

            broadcastData(cpu, cpu->jbu2.result_buffer, cpu->jbu2.rd_phy_res, JBU_FU); // only when completed 3 cycles

            /* Copy data from execute latch to memory latch*/
            update_ROB(cpu->jbu2); //Shweta ::: Instead of passing it to memory update ROB entry

            JALStackEntry entry;
            entry.reg_tag = cpu->jbu2.rd;
            entry.val = cpu->jbu2.result_buffer;
            jal_push(&jalstk, entry);

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
        if (cpu->stoppedDispatch)
        {
            cpu->stoppedDispatch = 0;
            cpu->decode.stalled = 0;
            //Shweta ::: added below two lines Try - remove if doesn't work
            cpu->fetch.stalled = 0;
        }

        update_ROB(cpu->jbu2);
        cpu->insn_completed++;
        cpu->jbu2.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            print_stage_content("JBU2", &cpu->jbu2);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            printf("JBU2: Empty\n");
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

        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            print_stage_content("Memory1", &cpu->mem1);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            printf("Memory1: Empty\n");
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
            //Shweta ::: No need to update ROB entry
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
        cpu->mem2.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            print_stage_content("Memory2", &cpu->mem2);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            printf("Memory2: Empty\n");
        }
    }
}

/*
 * This function creates and initializes APEX cpu.
 *
 * Note: You are free to edit this function according to your implementation
 */
APEX_CPU *
APEX_cpu_init(const char *filename, const char *operation, const int cycles)
{
    int i;
    APEX_CPU *cpu;

    /*Shweta : check if valid function and cylces num added by users*/
    if (!filename && !operation && !cycles)
    {
        return NULL;
    }

    cpu = calloc(1, sizeof(APEX_CPU));

    if (!cpu)
    {
        return NULL;
    }

    if (strcmp(operation, "showmem") != 0)
    {
        cpu->operationCycles = cycles;
    }
    else
    {
        if (cycles > 4096 || cycles < 0)
        {
            errorHandler("MemoryError");
        }
        cpu->memLoc = cycles;
        cpu->showMem = 1;
    }

    /*Shweta : If asked for simulation then set simulate property or else do display*/
    if (strcmp(operation, "simulate") == 0)
    {
        cpu->simulate = 1;
    }
    else
    {
        cpu->simulate = 0;
    }

    /* Initialize PC, Registers and all pipeline stages */
    cpu->pc = 4000;
    memset(cpu->regs, 0, sizeof(int) * REG_FILE_SIZE);
    memset(cpu->data_memory, 0, sizeof(int) * DATA_MEMORY_SIZE);

    cpu->single_step = 0;
    if (strcmp(operation, "single_step") == 0)
    {
        cpu->single_step = ENABLE_SINGLE_STEP;
    }

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

    // Initialize JalStack
    jalstk = NULL;

    // init checkpoint rat
    initializeCheckPointRat();

    // init checpoint urf
    initializeCheckPointURF();

    //Initialize ROB. The ROB will be accessed from here but like issue q we have not assigned it to
    // the cpu just try to keep seperate.
    createROB();

    //Initialize BIS
    createBIS();

    //Initialize BTB
    createBTB();

    if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
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
        /*Shweta : Show if not simulate*/
        if (ENABLE_DEBUG_MESSAGES && !cpu->simulate)
        {
            printf("--------------------------------------------\n");
            printf("Clock Cycle #: %d\n", cpu->clock);
            printf("--------------------------------------------\n");
        }
        printf("Program Counter #: %d\n", cpu->pc);

        /*Shweta : Stop execution if enconter HALT instruction 
        or excuted asked number of instructions cycles(for simulate or display)*/
        if (cpu->clock == cpu->operationCycles && !cpu->showMem)
        {
            printf("APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
            break;
        }

        // check if rob head is a mem ins and mready
        if (rob_head_peek() && !cpu->mem1.has_insn)
        {
            ROB_entry entry = ROB_pop();
            if (entry.status != -1)
            {

                // pass the rob popped data to mem stage 1
                pass_to_mem_stage(cpu, entry);
            }
        }
        else
        {
            //Commit from ROB head and update RRAT with commited architectural register
            //check ROB head status is valid
            int phy_rd = ROB_headEntryValid(); ///return valid destination physical register number for those instruction for which dest exist otherwise returns -1
            if (phy_rd != -1)
            {
                ROB_entry entry = ROB_pop();
                if (entry.status != -1 && entry.rd_arch != -1)
                    updateRRAT(phy_rd, entry.rd_arch);
            }
            else if (
                !ROB_is_empty() &&
                (strcmp(rob->head->entry.opcode_str, "CMP") == 0 ||
                 strcmp(rob->head->entry.opcode_str, "BZ") == 0 ||
                 strcmp(rob->head->entry.opcode_str, "BNZ") == 0 ||
                 strcmp(rob->head->entry.opcode_str, "JUMP") == 0))
            {
                ROB_entry entry = ROB_pop(); ///Pop only if status bit is 1
                if(entry.status != -1)
                    BIS_pop();
            }
            else if (!ROB_is_empty() && strcmp(rob->head->entry.opcode_str, "HALT") == 0 && !cpu->mem1.has_insn && !cpu->mem2.has_insn)
            {
                ROB_pop();
                printf("APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
                break;
                //End of the simulation
            }
        }

        // this is another cycle for memory access - d-cache
        APEX_memory2(cpu);

        // this is one cycle for memory computation
        APEX_memory1(cpu);

        //all ex stage this all in one cycle
        APEX_jbu2(cpu);
        APEX_jbu1(cpu);
        APEX_int_fu(cpu);
        APEX_mul_fu(cpu);

        /*Shweta ::: Print Issue/ROB/RAT/RRAT entries*/
        printQueue();
        printROB();

        issueInstruction(cpu);
        // APEX_dispatch(cpu); //However this is not a stage; If your instruction is coming for dispatch it will surely go to IQ and ROB
        // decode stage
        APEX_decode(cpu);

        // fetch stage
        APEX_fetch(cpu);
        printAll(cpu);
        // print_reg_file(cpu);

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
    if (!cpu->single_step)
    {
        printAll(cpu);
    }
}

void printAll(APEX_CPU *cpu)
{
    printMemory(cpu);
    printURF();
    printRAT();
    printRRAT();
    printArchToPhys();
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