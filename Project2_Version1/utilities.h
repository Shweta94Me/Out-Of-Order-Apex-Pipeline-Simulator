/*
Shweta ::: Structure for URF, RAT, R-RAT 
*/

#define URFMaxSize 48
#define RATMaxSize 16
#define RRATMaxSize 16


/*Unified Register File*/

struct URF{
    int value; //Value of Physical Register
    int free; //Physical register is free(0) or allocated(1)
    int status; //Physical register is valid(1) or invalid(0)
};



/*Register Alias Table (Front End)*/
struct RAT{
    int phy_reg_num; /* Physical register mapping for most recent Architectural Register*/
};


/*Retirement - Register Alias Table (Back End)*/
struct RRAT{
    int phy_reg_after_comit; /*Physical register for commited Architectural Register*/
};

int traverseURF(struct URF* urf);
int allocate_phy_dest_RAT(struct URF* urf, struct RAT* rat, int rd);
int renameSrcWithPhyReg(struct RAT* rat, int rs);
int readSrcFromURF(struct URF* urf, int phy_reg);
void updateURF(struct URF* urf, int result, int phy_res, enum FU fu_type);

