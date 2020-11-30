/*
Shweta ::: Implementation of URF, RAT, R-RAT
*/

#include "utilities.h"


//Check if URF has a free entry and return that free entry index

int traverseURF(struct URF* urf)
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

int allocate_phy_dest_RAT(struct URF* urf, struct RAT* rat, int rd)
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

int renameSrcWithPhyReg(struct RAT* rat, int rs){
    int phy_reg = rat[rs].phy_reg_num; //Read physical register num from RAT
    return phy_reg;
}

int readSrcFromURF(struct URF* urf, int phy_reg){
    if(urf[phy_reg].status){
        return urf[phy_reg].value;
    }
    return -1;
}

void updateURF(struct URF* urf, int result, int phy_res, enum FU fu_type){

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




