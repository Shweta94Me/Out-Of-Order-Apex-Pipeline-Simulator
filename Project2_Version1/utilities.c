/*
Shweta ::: Implementation of URF, RAT, R-RAT
*/
#include <stdio.h>
#include <string.h>
#include "utilities.h"

void initializeURF(){
    for (int i = 0; i < URFMaxSize; i++)
    {
        urf[i].free = 0;
        urf[i].status = 1;
        urf[i].value = 0;
    }
}

void initializeRAT(){
    for (int i = 0; i < RATMaxSize; i++)
    {
        rat[i].phy_reg_num = -1;
    }
}

void initializeRRAT(){
    for (int i = 0; i < RATMaxSize; i++)
    {
        rrat[i].phy_reg_after_comit = -1;
    }
}


//Check if URF has a free entry and return that free entry index

int traverseURF()
{
    for(int i = 0; i < URFMaxSize; i++)
    {
        if(urf[i].free == 0)
        {
            return i;
        }
    }

    return -1; //No free entry available
}

int allocate_phy_dest_RAT(int rd)
{
    //Check free entry in URF
    int phy_reg = traverseURF(urf);

    if(phy_reg != -1){
        //Update RAT with available free physical register
        rat[rd].phy_reg_num = phy_reg; 
        //Mark free and status bit in URF 
        urf[phy_reg].free = 1; //Allocated
        urf[phy_reg].status = 0; //Invalid as there is an instruction which is updating this destination register
        return phy_reg;
    }

    return -1; // Can not allocate new physical register
}

int renameSrcWithPhyReg(int rs){
    int phy_reg = rat[rs].phy_reg_num; //Read physical register num from RAT
    return phy_reg;
}

int readSrcFromURF(int phy_reg){
    if(urf[phy_reg].status){
        return urf[phy_reg].value;
    }
    return -1;
}

void updateURF(int result, int phy_res, enum FU fu_type){

    if(fu_type == Int_FU)
    {
        urf[phy_res].value = result;
        urf[phy_res].status = 1;
    }
    else if(fu_type == Mul_FU)
    {
        urf[phy_res].value = result;
        urf[phy_res].status = 1;
    }
    else if(fu_type == Mem_FU)
    {
        urf[phy_res].value = result;
        urf[phy_res].status = 1;
    }
}

void printURF(){
    printf("|\tRegister\t|\tValue\t|\tfree\t|\tstatus\n");
    for (int i = 0; i < URFMaxSize; i++)
    {
        char free[10];
        if(urf[i].free){
            strcpy(free,"alloc");
        }
        else{
             strcpy(free,"free");
        }

        char status[10];
        if(urf[i].status){
            strcpy(status,"valid");
        }
        else{
             strcpy(status,"invalid");
        }
        printf("|\tR[%d]\t|\t%d\t|\t%s\t|\t%s\n", i, urf[i].value,free, status);
    }
}

void printRAT(){
    printf("|\tarchitecture\t|\tphysical\t|\n");
    for (int i = 0; i < RATMaxSize; i++)
    {
       
        printf("|\tR[%d]\t|\tR[%d]\t|\n", i, rat[i].phy_reg_num);
    }

}

void printRRAT(){
    printf("|\tarchitecture\t|\tphysical\t|\n");
    for (int i = 0; i < RRATMaxSize; i++)
    {
       
        printf("|\tR[%d]\t|\tR[%d]\t|\n", i, rrat[i].phy_reg_after_comit);
    }

}




