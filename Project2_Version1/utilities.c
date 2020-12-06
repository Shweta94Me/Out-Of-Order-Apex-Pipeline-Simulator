/*
Shweta ::: Implementation of URF, RAT, R-RAT
*/
#include <stdio.h>
#include <string.h>
#include "utilities.h"

void initializeURF(){
    for (int i = 0; i < URFMaxSize; i++)
    {
        urf[i].free = 0; //Intialize as free
        urf[i].status = 0; //Intialize as Invalid 
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

//Shweta ::: Change status to Valid and load value from result
void updateURF(int result, int phy_res){
    urf[phy_res].value = result;
    urf[phy_res].status = 1;
}

void freeRegFromURF(int last_commited_phy_reg)
{   
    //Free URF physical register
    urf[last_commited_phy_reg].free = 0;
}

void updateRRAT(int phy_rd, int arch_idx)
{
    int last_commited_phy_reg = rrat[arch_idx].phy_reg_after_comit;
    if(last_commited_phy_reg != -1){
        //Renamer instruction came in
        freeRegFromURF(last_commited_phy_reg);
        rrat[arch_idx].phy_reg_after_comit = phy_rd; 
    }
    else{
        //Not a renamer instruction
        rrat[arch_idx].phy_reg_after_comit = phy_rd; 
        // freeRegFromURF(phy_rd);
    }
    //update with latest commited physical register
}

void printURF(){
    printf("\n-----------------URF---------------------\n");
    // printf("|\tRegister |\tValue |\tfree |\tstatus\n");
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
        printf("|\tP[%d]\t|\t%d\t|\t%s\t|\t%s\n", i, urf[i].value,free, status);
    }
}

void printRAT(){
    printf("\n-----------------RAT---------------------\n");
    // printf("|\tarch\t|\tphys\t|\n");
    for (int i = 0; i < RATMaxSize; i++)
    {
       
        printf("|\tR[%d]\t|\t%d\t|\n", i, rat[i].phy_reg_num);
    }

}

void printRRAT(){
    printf("\n-----------------RRAT---------------------\n");
    // printf("|\tarch\t|\tphys\t|\n");
    for (int i = 0; i < RRATMaxSize; i++)
    {
       
        printf("|\tR[%d]\t|\t%d\t|\n", i, rrat[i].phy_reg_after_comit);
    }

}

void printArchToPhys(){
    printf("\n-----------------A-Phy---------------------\n");
    printf("|\tarch-reg\t|\tphy-reg-val\t|\n");
    for (int i = 0; i < RRATMaxSize; i++)
    {
       
        printf("|\t  R[%d]  \t|\t  %d      \t|\n", i, urf[rrat[i].phy_reg_after_comit].value);
    }
}




