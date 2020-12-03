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

ROB_entry_node *ROB_create_entry(ROB_entry entry)
{
    ROB_entry_node *node = (ROB_entry_node *)malloc(sizeof(ROB_entry_node));
    
    // ROB_entry entry = {
    //     .ar_address = 0,
    //     .pc_value = val,
    //     .result = 0,
    //     .sval_valid = 0,
    //     .status = 1,
    // };

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

}

void ROB_update_RF(ROB_entry entry)
{
    // TODO:
}

void ROB_pop()
{
    if (ROB_is_empty() || !rob->head->entry.status)
        return;
    ROB_entry_node *node = rob->head;
    // printf("Val -> %d \n ", node->entry.pc_value);
    ROB_update_RF(node->entry);
    rob->head = rob->head->next;
    free(node);
    rob->size--;
    if (!rob->size)
        rob->head = rob->tail = 0;

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

// // // Unit test code for rob 
// int main(){

//     createROB();

//     printf("size -> %d \n", rob->size);  // 0
//     ROB_push(1);
//     ROB_push(2);
//     ROB_push(3);
//     ROB_push(4);
//     ROB_push(5);
//     ROB_push(6);
//     ROB_push(7);
//     ROB_push(8);  // this is 8th

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
//     ROB_push(9);
//     printf("size -> %d \n", rob->size); // 1
//     ROB_pop();  //this pop cause segmentation fault
//     ROB_pop();
//     printf("size -> %d \n", rob->size); // 0

//     return 0;
// }


