#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "rob.h"

// function to create a queue
// of given capacity.
// It initializes size of queue as 0
void createROB()
{
    rob = (struct REORDER_BUFFER *)malloc(
        sizeof(struct REORDER_BUFFER));

    rob->head = NULL;
    rob->tail = NULL;
    rob->size = 0;
}

// Queue is full when size becomes
// equal to the capacity
int ROB_is_full()
{
    return (rob->size >= ROB_SIZE);
}

// Queue is empty when size is 0
int ROB_is_empty()
{
    return (rob->size == 0);
}

ROB_entry_node *ROB_create_entry(ROB_entry entry)
{
    ROB_entry_node *node = (ROB_entry_node *)malloc(sizeof(ROB_entry_node));

    node->entry = entry;
    node->next = NULL;
    return node;
}

// Function to add an item to the queue.
// It changes rear and size
void ROB_push(ROB_entry entry)
{
    if (ROB_is_full())
        return;
    ROB_entry_node *node = ROB_create_entry(entry);
    // printf("Val -> %d \n ", node->entry.pc_value);
    if (!rob->tail)
    {
        rob->head = node;
    }
    else
    {
        rob->tail->next = node;
    }
    rob->tail = node;
    rob->size++;
}

// this function is dedicate to only to ins with destination register and not memory ins
int ROB_headEntryValid()
{

    if (!ROB_is_empty() && rob->head->entry.status &&
        strcmp(rob->head->entry.opcode_str, "LOAD") != 0 &&
        strcmp(rob->head->entry.opcode_str, "LDR") != 0 &&
        strcmp(rob->head->entry.opcode_str, "STORE") != 0 &&
        strcmp(rob->head->entry.opcode_str, "STR") != 0 &&
        strcmp(rob->head->entry.opcode_str, "CMP") != 0 &&
        strcmp(rob->head->entry.opcode_str, "BZ") != 0 &&
        strcmp(rob->head->entry.opcode_str, "BNZ") != 0 &&
        strcmp(rob->head->entry.opcode_str, "JUMP") != 0 &&
        strcmp(rob->head->entry.opcode_str, "HALT") != 0)
        return rob->head->entry.phy_rd;
    else
        return -1;
}

ROB_entry ROB_pop()
{
    ROB_entry ret_entry;
    if (ROB_is_empty() || !rob->head->entry.status)
    {
        //ret_entry.rd_arch = -1; //Instruction with no destionation register
        ret_entry.status = -1;
        return ret_entry;
    }

    ROB_entry_node *node = rob->head;
    // printf("Val -> %d \n ", node->entry.pc_value);
    // ROB_update_RF(node->entry);

    ret_entry.imm = node->entry.imm;
    ret_entry.mready = node->entry.mready;
    ret_entry.opcode = node->entry.opcode;
    strcpy(ret_entry.opcode_str, node->entry.opcode_str);
    ret_entry.pc_value = node->entry.pc_value;

    ret_entry.phy_rd = node->entry.phy_rd;
    ret_entry.rd_arch = node->entry.rd_arch;

    ret_entry.result = node->entry.result;

    ret_entry.rs1_arch = node->entry.rs1_arch;
    ret_entry.rs1_tag = node->entry.rs1_tag;
    ret_entry.rs1_value = node->entry.rs1_value;

    ret_entry.rs2_arch = node->entry.rs2_arch;
    ret_entry.rs2_tag = node->entry.rs2_tag;
    ret_entry.rs2_value = node->entry.rs2_value;

    ret_entry.rs3_arch = node->entry.rs3_arch;
    ret_entry.rs3_tag = node->entry.rs3_tag;
    ret_entry.rs3_value = node->entry.rs3_value;

    ret_entry.status = node->entry.status;

    rob->head = rob->head->next;
    free(node);
    rob->size--;
    if (!rob->size)
        rob->head = rob->tail = 0;

    return ret_entry;
}

// peek to check if head has mem ins
int rob_head_peek()
{

    if (!ROB_is_empty() && rob->head->entry.mready && rob->head->entry.status &&
        (strcmp(rob->head->entry.opcode_str, "LOAD") == 0 ||
         strcmp(rob->head->entry.opcode_str, "LDR") == 0 ||
         strcmp(rob->head->entry.opcode_str, "STORE") == 0 ||
         strcmp(rob->head->entry.opcode_str, "STR") == 0))
        return 1;
    return 0;
}

void forward_to_rob(int pc, int result_buffer)
{
    ROB_entry_node *node = rob->head;
    while (node)
    {
        if (pc == node->entry.pc_value)
        {
            node->entry.result = result_buffer;
            node->entry.status = 1;
            return;
        }
        node = node->next;
    }
}

// this method is used to indicate the load store ins can be passed to mem stage or not
// if all the source register are avialble either at the time of dispatch or made avilable when the same
// entry of load store is in iq and after broadcast all the src are avialable now set the m ready bit
// discard the entry from iq
void set_rob_mready_bit(int pc, int rs1Val, int rs2Val, int rs3Val){
    ROB_entry_node *node = rob->head;
    while (node)
    {
        if (pc == node->entry.pc_value)
        {
            node->entry.rs1_value = rs1Val;
            node->entry.rs2_value = rs2Val;
            node->entry.rs3_value = rs3Val;
            node->entry.mready = 1;
            node->entry.status = 1;
            return;
        }
        node = node->next;
    }
}

void ROB_squash_after_misprediction(ROB_entry_node *sch_node)
{
    if (ROB_is_empty())
        return;

    ROB_entry_node *tmp;

    tmp = sch_node->next;
    sch_node->next = NULL;
    sch_node = tmp;

    while (sch_node)
    {
        tmp = sch_node->next;
        free(sch_node);
        sch_node = tmp;
        rob->size--;
    }
}

void printROB()
{

    ROB_entry_node *temp = rob->head;
    printf("\nDetails of ROB (Reorder Buffer) State :");
    while (!ROB_is_empty() && temp)
    {
        printf("%d->", temp->entry.pc_value);
        temp = temp->next;
    }
    printf("\n");
}

// //    Unit test code for rob
// //    gcc -o rob_test issue_q.c   /// this command is for individual rob testing
// //    ./rob_test.exe
// int main(){

//     createROB();

//     printf("size -> %d \n", rob->size);  // 0
//     ROB_entry entry;
//     entry.pc_value = 1;
//     ROB_push(entry);
//     entry.pc_value = 2;
//     ROB_push(entry);
//     entry.pc_value = 3;
//     ROB_push(entry);
//     entry.pc_value = 4;
//     ROB_push(entry);
//     entry.pc_value = 5;
//     ROB_push(entry);
//     entry.pc_value = 6;
//     ROB_push(entry);
//     entry.pc_value = 7;
//     ROB_push(entry);
//     entry.pc_value = 8;
//     ROB_push(entry);  // this is 8th

//     printROB();

//     printf("size -> %d \n", rob->size); // 7
//     ROB_pop();
//     ROB_pop();
//     ROB_pop();
//     ROB_pop();
//     ROB_pop();
//     ROB_pop();
//     ROB_pop();
//     ROB_pop();  // this is 8th pop()

//     printf("size -> %d \n ", rob->size); // 0
//     entry.pc_value = 9;
//     ROB_push(entry);
//     printROB();
//     printf("size -> %d \n", rob->size); // 1
//     ROB_pop();
//     ROB_pop();
//     printf("size -> %d \n", rob->size); // 0

//     return 0;
// }

/////////////////////////////////do not uncomment this code//////////////////////////////////////////////////////////

//Rob push

// if (ROB_is_full())
//     return;
// ROB_entry_node *node = ROB_create_entry(instruction);

// printf("Val -> %d \n ", node->entry.pc_value);
// if(rob->head == NULL){
//     rob->head = node;
//     rob->size++;
// }else{
//     rob->tail->next = node;
//     rob->size++;
// }

// rob->tail = node;
// rob->tail->next = rob->head;

/// rob pop

// if (ROB_is_empty() || !rob->head->entry.status)
//     return;
// ROB_entry temp;
// if(rob->head == rob->tail){
//     temp = rob->head->entry;
//     free(rob->head);
//     rob->head = NULL;
//     rob->tail = NULL;
//     rob->size--;
// }
// else{
//     ROB_entry_node* node = rob->head;
//     temp = node->entry;
//     rob->head = rob->head->next;
//     rob->tail->next = rob->head;
//     free(node);
//     rob->size--;
// }
// printf("Val -> %d \n ", temp.pc_value);
