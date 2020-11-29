/*
Shweta ::: Implementation of URF, RAT, R-RAT
*/

#include "utilities.h"
#include "apex_cpu.h"


//Check if URF has a free entry and return that free entry index

int traverseURF(URF *URF)
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

int allocate_phy_dest_RAT(URF *URF, RAT *RAT, int rd)
{
    //Check free entry in URF
    int phy_reg = traverseURF(URF);

    if(phy_reg != -1){
        //Update RAT with available free physical register
        RAT[rd].phy_reg_num = phy_reg; 
        //Mark free and status bit in URF 
        URF[phy_reg].free = 1; //Allocated
        URF[phy_reg].status = 0; //Invalid as there is an instruction which is updating this destination register
        return phy_reg;
    }

    return -1; // Can not allocate new physical register
}

int renameSrc1_readSrc1(URF *URF, RAT *RAT, APEX_CPU *cpu)
{
    int phy_reg = RAT[cpu->decode.rs1].phy_reg_num; //Read physical register num from RAT
    cpu->decode.rs1_phy_res = phy_reg; //Store physical regsiter number
    //Read physical register value from URF only if status is valid
    if(URF[phy_reg].status){
        cpu->decode.rs1_value = URF[phy_reg].value;
        return 1; //Mark src1 ready bit to 1
    }
    return 0; //Mark src1 ready bit to 0
}

int renameSrc2_readSrc2(URF *URF, RAT *RAT, APEX_CPU *cpu)
{
    int phy_reg = RAT[cpu->decode.rs1].phy_reg_num; //Read physical register num from RAT
    cpu->decode.rs2_phy_res = phy_reg; //Store physical regsiter number
    //Read physical register value from URF only if status is valid
    if(URF[phy_reg].status){
        cpu->decode.rs2_value = URF[phy_reg].value;
        return 1; //Mark src1 ready bit to 1
    }
    return 0; //Mark src1 ready bit to 0
}

int renameSrc3_readSrc3(URF *URF, RAT *RAT, APEX_CPU *cpu)
{
    int phy_reg = RAT[cpu->decode.rs3].phy_reg_num; //Read physical register num from RAT
    cpu->decode.rs3_phy_res = phy_reg; //Store physical regsiter number
    //Read physical register value from URF only if status is valid
    if(URF[phy_reg].status){
        cpu->decode.rs3_value = URF[phy_reg].value;
        return 1; //Mark src1 ready bit to 1
    }
    return 0; //Mark src1 ready bit to 0
}

void updateURF(APEX_CPU *cpu, enum FU fu_type)
{
    if(fu_type == Int_FU)
    {
        cpu->URF[cpu->ex_int_fu.rd_phy_res].value = cpu->ex_int_fu.result_buffer;
        cpu->URF[cpu->ex_int_fu.rd_phy_res].status = 1;
    }
    else if(fu_type == Mul_FU)
    {
        cpu->URF[cpu->ex_mul_fu.rd_phy_res].value = cpu->ex_mul_fu.result_buffer;
        cpu->URF[cpu->ex_mul_fu.rd_phy_res].status = 1;
    }
    else if(fu_type == Mem_FU)
    {
        cpu->URF[cpu->mem2.rd_phy_res].value = cpu->mem2.result_buffer;
        cpu->URF[cpu->mem2.rd_phy_res].status = 1;
    }
}




