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
    int checkpoint_rat_idx;
    int checkpoint_urf_idx;
}BIS_entry;


typedef struct BIS
{
    BIS_entry bis_entry[BIS_SIZE];
    int bottom; //front
    int top; //rear
    int size;
}BIS;


typedef struct BTB
{
    BTB_entry btb_entry[BTB_SIZE];
    int size;
}BTB;


typedef struct mispredictionData
{
    int mispredicted_instrc_pc;
    int mispredicted_branch_tag;
    int target_addr;
}mispredictionData;

BTB *btb;
BIS *bis;
mispredictionData mispredicted_data;


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
BIS_entry BIS_squash();
void printBTB();
BIS_entry peekBottom();

#endif