/*
Shweta ::: Structure for URF, RAT, R-RAT 
*/
#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#define URFMaxSize 48
#define RATMaxSize 16
#define RRATMaxSize 16
#define CheckPointMaxSize 4

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

typedef struct JALStackEntry{
    int reg_tag;
    int val;

}JALStackEntry;

typedef struct checkpoint_RAT_entry{
    int ratFree;
    RAT Rat[RATMaxSize];
}checkpoint_RAT_entry;

typedef struct checkpoint_RAT
{
    checkpoint_RAT_entry entry;
}checkpoint_RAT;

typedef struct checkpoint_URF_entry{
    int urfFree;
    URF Urf[URFMaxSize];
}checkpoint_URF_entry;

typedef struct checkpoint_URF{
    checkpoint_URF_entry entry;
}checkpoint_URF;

// A structure to represent a stack
typedef struct StackNode {
    JALStackEntry data;
    struct StackNode* next;
}StackNode;

URF urf[URFMaxSize];
RAT rat[RATMaxSize];
RRAT rrat[RRATMaxSize];


checkpoint_RAT checkpointRat[CheckPointMaxSize];
checkpoint_URF checkpointUrf[CheckPointMaxSize];

StackNode* jalstk;

void initializeURF();
void initializeRAT();
void initializeRRAT();

void initializeCheckPointRat();
void initializeCheckPointURF();
int insertCheckpointRat(RAT checkpoint_rat[]);
int insertCheckpointURF(URF checkpoint_urf[]);

int traverseURF();
int allocate_phy_dest_RAT(int rd);
int renameSrcWithPhyReg(int rs);
int readSrcFromURF(int phy_reg);
void updateURF(int result, int phy_res);
void updateRRAT(int phy_rd, int arch_idx);
void printURF();
void printRAT();
void printRRAT();
void printArchToPhys();
void jal_push(StackNode** root, JALStackEntry data);
int jal_pop(StackNode** root);
int jal_peek(StackNode* root);
#endif
