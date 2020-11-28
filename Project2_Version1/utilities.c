/*
Shweta ::: Implementation of URF, RAT, R-RAT
*/

#include "utilities.h"


//Check if URF has a free entry and return that free entry index

int traverseURF()
{
    for(int i = 0; i < URFMaxSize; i++)
    {
        if(URF[i].free == 0)
        {
            return i;
        }
    }

    return -1; //No free entry available
}

int allocate_phy_dest_RAT(int rd)
{
    //Check free entry in URF
    int phy_reg = traverseURF();

    if(phy_reg != -1){
        //Update RAT with available free physical register
        RAT[rd].phy_reg_num = phy_reg; 
        //Mark free and status bit in URF 
        URF[phy_reg].free = 1; //Allocated
        URF[phy_reg].status = 0; //Invalid as there is an instruction which is updated this destination register
        return phy_reg;
    }

    return -1; // Can not allocate new physical register
}




