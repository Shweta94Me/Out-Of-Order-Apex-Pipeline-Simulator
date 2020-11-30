/*
Shweta ::: Structure for URF, RAT, R-RAT 
*/
#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#define URFMaxSize 48
#define RATMaxSize 16
#define RRATMaxSize 16


#include "issue_q.h"


/*Unified Register File*/

typedef struct URF{
    int value; //Value of Physical Register
    int free; //Physical register is free(0) or allocated(1)
    int status; //Physical register is valid(1) or invalid(0)
}URF;



/*Register Alias Table (Front End)*/
typedef struct RAT{
    int phy_reg_num; /* Physical register mapping for most recent Architectural Register*/
}RAT;


/*Retirement - Register Alias Table (Back End)*/
typedef struct RRAT{
    int phy_reg_after_comit; /*Physical register for commited Architectural Register*/
}RRAT;

int traverseURF(URF* urf);
int allocate_phy_dest_RAT(URF* urf, RAT* rat, int rd);
int renameSrcWithPhyReg(RAT* rat, int rs);
int readSrcFromURF(URF* urf, int phy_reg);
void updateURF(URF* urf, int result, int phy_res, enum FU fu_type);
#endif
