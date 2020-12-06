#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "bis.h"

// function to create a queue
// of given capacity.
// It initializes size of queue as 0
void createBIS()
{
    bis = (struct REORDER_BUFFER *)malloc(
        sizeof(struct REORDER_BUFFER));

    bis->head = NULL;
    bis->tail = NULL;
    bis->size = 0;
}

// Queue is full when size becomes
// equal to the capacity
int BIS_is_full()
{
    return (bis->size >= BIS_SIZE);
}

// Queue is empty when size is 0
int BIS_is_empty()
{
    return (bis->size == 0);
}

BIS_node *BIS_create_entry(ROB_entry_node *rob_node)
{
    BIS_node *node = (BIS_node *)malloc(sizeof(BIS_node));

    node->entry = rob_node;
    node->next = NULL;
    return node;
}

// Function to add an item to the queue.(if it is a branch instruction)
// It changes rear and size
void BIS_push(ROB_entry_node *rob_node)
{
    if (ROB_is_full())
        return;
    BIS_node *node = BIS_create_entry(rob_node);
    if (!bis->tail)
    {
        bis->head = node;
    }
    else
    {
        bis->tail->next = node;
    }
    bis->tail = node;
    bis->size++;
}

void BIS_pop()
{

    BIS_node *node = bis->head;

    bis->head = bis->head->next;
    free(node);
    bis->size--;
    if (!bis->size)
        bis->head = bis->tail = NULL;
}

void BIS_squash(ROB_entry_node *rob_node)
{
    if (BIS_is_empty())
        return;
    BIS_node *node = bis->head, *tmp, *prev = NULL;

    while (node)
    {
        if (node->entry == rob_node)
            break;
        prev = node;
        node = node->next;
    }

    if (bis->head == node)
        bis->head = bis->tail = NULL;
    else
    {
        prev->next = NULL;
        bis->tail = prev;
    }

    while (node)
    {
        tmp = node->next;
        free(node);
        node = tmp;
        bis->size--;
    }
}

// Driver Program to test above functions
// int main()
// {
// 	struct Queue* q = createQueue();
// 	enQueue(q, 10);
// 	enQueue(q, 20);
// 	deQueue(q);
// 	deQueue(q);
// 	enQueue(q, 30);
// 	enQueue(q, 40);
// 	enQueue(q, 50);
// 	deQueue(q);
// 	printf("Queue Front : %d \n", q->front->key);
// 	printf("Queue Rear : %d", q->rear->key);
// 	return 0;
// }

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
