/*
Shweta ::: Structure for URF, RAT, R-RAT 
*/

#include <stdbool.h>

#define URFMaxSize 48
#define RATMaxSize 16
#define RRATMaxSize 16

/*Unified Register File*/
typedef struct URF{
    int value; //Value of Physical Register
    bool free; //Physical register is free(0) or allocated(1)
    bool status; //Physical register is valid(1) or invalid(0)
}URF;

/*Register Alias Table (Front End)*/
typedef struct RAT{
    int phy_reg_num; /* Physical register mapping for most recent Architectural Register*/
}RAT;

/*Retirement - Register Alias Table (Back End)*/
typedef struct RRAT{
    int phy_reg_after_comit; /*Physical register for commited Architectural Register*/
}RRAT;


struct URF URF[URFMaxSize];
struct RAT RAT[RATMaxSize];
struct RRAT RRAT[RRATMaxSize];

