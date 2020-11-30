#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
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

    return;
}

// Queue is full when size becomes
// equal to the capacity
int ROB_is_full()
{
    return (rob->size >= ROB_ENTRY_SIZE);
}

// Queue is empty when size is 0
int ROB_is_empty()
{
    return (rob->size == 0);
}

char instruction_type(int opcode)
{
    switch (opcode)
    {
    case OPCODE_STORE:
    case OPCODE_LOAD:
    case OPCODE_LDR:
    case OPCODE_STR:
        return 'm';
    default:
        return 'r';
    }
}

ROB_entry_node *ROB_create_entry(CPU_Stage stage)
{
    ROB_entry_node *node = (ROB_entry_node *)malloc(sizeof(ROB_entry_node));
    ROB_entry entry = {
        .ar_address = stage.rd,
        .pc_value = stage.pc,
        .inst_type = instruction_type(stage.opcode),
        .result = 0,
        .sval_valid = 0,
        .status = 1,
    };
    node->entry = entry;
    node->next = NULL;
    return node;
}

// Function to add an item to the queue.
// It changes rear and size
void ROB_push(CPU_Stage instruction)
{
    if (ROB_is_full())
        return;
    ROB_entry_node *node = ROB_create_entry(instruction);

    // if(rob->head == NULL){
    //     rob->head = node;
    //     rob->size++;
    // }else{
    //     rob->tail->next = node;
    //     rob->size++;
    // }

    // rob->tail = node;
    // rob->tail->next = rob->head;

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

void ROB_update_RF(ROB_entry entry)
{
    // TODO:
    
}

// Function to remove an item from queue.
// It changes front and size
void ROB_pop()
{
    if (ROB_is_empty() || !rob->head->entry.status){
        //printf("reached here");
        return;
    }
        
    
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

    ROB_entry_node *node = rob->head;
    // ROB_update_RF(node->entry);
    printf("val -> %d \n", node->entry.pc_value);
    rob->head = rob->head->next;
    free(node);
    rob->size--;
}

void forward_to_rob(CPU_Stage instruction)
{
    ROB_entry_node *node = rob->head;
    while (node)
    {
        if (instruction.pc == node->entry.pc_value)
        {
            node->entry.result = instruction.result_buffer;
            node->entry.status = 1;
            return;
        }
        node = node->next;
    }
}

// Unit test code for rob 
int main(){

    createROB();
    CPU_Stage cpustage;

    cpustage.pc = 1;
    cpustage.opcode = OPCODE_ADD;
    cpustage.rd = 1;


    printf("size -> %d \n", rob->size);  // 0
    ROB_push(cpustage);
    cpustage.pc = 2;
    ROB_push(cpustage);
    cpustage.pc = 3;
    ROB_push(cpustage);
    cpustage.pc = 4;
    ROB_push(cpustage);
    cpustage.pc = 5;
    ROB_push(cpustage);
    cpustage.pc = 6;
    ROB_push(cpustage);
    cpustage.pc = 7;
    ROB_push(cpustage);
    cpustage.pc = 8;
    ROB_push(cpustage);  // this is 8th

    printf("size -> %d \n", rob->size); // 7
    ROB_pop();
    ROB_pop();
    ROB_pop();
    ROB_pop();
    ROB_pop();
    ROB_pop();
    ROB_pop();
    ROB_pop();  // this is 8th pop()  

    printf("size -> %d \n ", rob->size); // 0
    cpustage.pc = 9;
    ROB_push(cpustage);
    ROB_pop();
    ROB_pop();
    printf("size -> %d \n", rob->size); // 1

    return 0;


}


