/*
Shweta ::: Structure code for BIS 
*/

#ifndef _BRANCH_PRED_H_
#define _BRANCH_PRED_H_

#include "rob.h"

#define BTB_SIZE 8
#define BIS_SIZE 4

typedef struct BTB_entry
{
    int branch_instrc_addr;
    int branch_direction; //Shweta ::: (0) default - not taken or 1 for taken
    int target_addrs;
    
}BTB_entry;

typedef struct BIS_entry
{
    int bis_tag;
    ROB_entry_node *rob_entry;
    int checkpoint_idx;
}BIS_entry;


typedef struct BIS
{
    BIS_entry *bis_entry;
    int bottom; //front
    int top; //rear
    int size;
}BIS;


typedef struct BTB
{
    BTB_entry *btb_entry;
}BTB;

BTB *btb;
BIS *bis;

void createBTB();
void createBIS();
int BIS_is_full();
int BIS_is_empty();
int BIS_push(BIS_entry);
int BIS_pop();

#endif