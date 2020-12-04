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

void printROB(){

    ROB_entry_node* temp = rob->head;
    printf("\nElements in Circular Queue are: \n");
    while(!ROB_is_empty() && temp->next){
        printf("%d<-", temp->entry.pc_value);
        temp = temp->next;
    }
    printf("%d<-\n", temp->entry.pc_value);
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


