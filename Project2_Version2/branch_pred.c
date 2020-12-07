#include <stdlib.h>
#include "branch_pred.h"

void createBIS()
{
    bis = (struct BIS*)malloc(sizeof(struct BIS));
    bis->bis_entry = (BIS_entry *)malloc(BIS_SIZE * sizeof(BIS_entry));
    bis->size = 0;
    bis->bottom = 0;
    bis->top = -1;
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


int BIS_push(BIS_entry entry){
    
    if(BIS_is_full())
        return -1;
    
    bis->top = (bis->top + 1) % BIS_SIZE;
    bis->bis_entry[bis->top] = entry;
    bis->bis_entry[bis->top].bis_tag = bis->top; //BIS tag
    bis->size++; 
    return 1;
}

int BIS_pop()
{
    if(BIS_is_empty())
        return -1;
    
    bis->bottom = (bis->bottom + 1) % BIS_SIZE;
    bis->size--;
    return 1;
}