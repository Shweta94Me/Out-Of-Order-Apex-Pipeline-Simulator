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
    int size;
}BTB;

BTB *btb;
BIS *bis;


void createBIS();
void createBTB();
int BIS_is_full();
int BIS_is_empty();
int BIS_incr_top();
int BIS_pop();
int get_BIS_top_idx();
int get_size_of_BTB();
int BTB_is_full();
int BTB_is_empty();
int BTB_lookup(int pc);
int BTB_entry_exist(int pc);
int BTB_push(int pc);
int BTB_update_target_addr(int pc, int target_addr);

#endif