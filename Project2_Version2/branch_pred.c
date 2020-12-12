#include <stdlib.h>
#include <stdio.h>
#include "branch_pred.h"

void createBIS()
{
    bis = (struct BIS*)malloc(sizeof(struct BIS));
    // bis->bis_entry = (BIS_entry *)malloc(BIS_SIZE * sizeof(BIS_entry));

    for (int i = 0; i < BIS_SIZE; i++)
    {
        bis->bis_entry[i].bis_tag = -1;
        bis->bis_entry[i].checkpoint_rat_idx = -1;
        bis->bis_entry[i].checkpoint_urf_idx = -1;
        bis->bis_entry[i].rob_entry = NULL;
    }
    
    bis->size = 0;
    bis->bottom = 0;
    bis->top = -1;
}

void createBTB()
{
    btb = (struct BTB*)malloc(sizeof(struct BTB));
    // btb->btb_entry = (BTB_entry *)malloc(BTB_SIZE * sizeof(BTB_entry));

    for (int i = 0; i < BTB_SIZE; i++)
    {
        btb->btb_entry[i].branch_direction = -1;
        btb->btb_entry[i].branch_instrc_addr = -1;
        btb->btb_entry[i].target_addrs = -1;
    }
    
    btb->size = 0;
}

int get_size_of_BIS()
{
    return bis->size;
}

int BIS_is_full()
{
    return (bis->size >= BIS_SIZE);
}

int BIS_is_empty()
{
    return !get_size_of_BIS();
}

int get_size_of_BTB()
{
    return btb->size;
}

int BTB_is_full()
{
    return (btb->size >= BTB_SIZE);
}

int BTB_is_empty()
{
    return !get_size_of_BTB();
}

int get_BIS_top_idx()
{
    return bis->top;
}

int BIS_incr_top(){
    
    if(BIS_is_full())
        return -1;
    
    bis->top = (bis->top + 1) % BIS_SIZE;
    bis->bis_entry[bis->top].bis_tag = bis->top; //BIS tag
    bis->size++; 
    return 1;
}

BIS_entry peekBottom(){
    return bis->bis_entry[bis->bottom];
}

int BIS_pop()
{
    if(BIS_is_empty())
        return -1;
    
    bis->bottom = (bis->bottom + 1) % BIS_SIZE;
    bis->size--;
    return 1;
}

int BTB_lookup(int pc){
    if(!BTB_is_empty())
    {
        for(int i = 0; i < BTB_SIZE; i++)
        {
            if(btb->btb_entry[i].branch_instrc_addr == pc && btb->btb_entry[i].branch_direction == 1)
            {
                //Branch instruction exists and says take the branch
                //So return the store target address
                return btb->btb_entry[i].target_addrs;
            }
        }
    }
    
    //Two possibilities 1: Branch instruction doesn't exist in BTB or Direction says do not take a branch
    return -1;
}

int BTB_entry_exist(int pc)
{
    if(!BTB_is_empty())
    {
        for(int i = 0; i < BTB_SIZE; i++)
        {
            if(btb->btb_entry[i].branch_instrc_addr == pc)
            {
                //Branch instruction exists
                return 1;
            }
        }
    }
    
    return -1;
}

int BTB_push(int pc)
{
    if(BTB_is_full())
        return -1;
    
    btb->btb_entry[btb->size].branch_direction = 0; //Default is branch not taken
    btb->btb_entry[btb->size].branch_instrc_addr = pc;
    btb->btb_entry[btb->size].target_addrs = -1;
    btb->size++;
    return 1;
}

int BTB_update_target_addr(int pc, int target_addr)
{
    for(int i = 0; i < BTB_SIZE; i++)
    {
        if(btb->btb_entry[i].branch_instrc_addr == pc)
        {
            //Branch instruction exists and says take the branch
            //So return the store target address
            btb->btb_entry[i].target_addrs = target_addr;
            return i;
        }
    }
    return -1;
}

//Return branch_tag for squash instruction
BIS_entry BIS_squash()
{
    BIS_entry entry;

    entry = bis->bis_entry[bis->top];

    bis->top = (bis->top - 1 + BIS_SIZE) % BIS_SIZE;
    bis->size--;

    return entry;    
}

void printBTB()
{
    if(BTB_is_empty())
        return;
    printf("\n-----------------BTB---------------------\n");
    for (int i = 0; i < btb->size; i++)
    {
        printf("|\tPC - %d\t|\tDir- %d \t|\tTAdr - %d|\n",btb->btb_entry[i].branch_instrc_addr, btb->btb_entry[i].branch_direction, btb->btb_entry[i].target_addrs);
    }
    
}