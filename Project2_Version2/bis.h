#ifndef _BIS_
#define _BIS_

#include "apex_macros.h"
#include "rob.h"

#define BIS_SIZE 4

typedef struct BIS_node BIS_node;

struct BIS_node
{
    ROB_entry_node *entry;
    BIS_node *next;
};

typedef struct BIS
{
    BIS_node *head;
    BIS_node *tail;
    size_t size;
} BIS;

BIS *bis;



void createBIS();
int BIS_is_full();
int BIS_is_empty();
void BIS_push(ROB_entry_node *);
void *BIS_pop();
void BIS_squash(ROB_entry_node *);

#endif


// typedef struct ROB_entry
// {
//     int pc_value;   //instruction address
//     int ar_address; //address of destination architectural register
//     int result;     //result of reg-to-reg or mem address
//     int sval_valid; //only for STORE
//                     // int ex_codes;   //exception codes
//     int status;     //indication if the result is valid
//     // char inst_type; //

//     int opcode;

//     int rs1_arch; //rs1 arch
//     int rs1_tag; //src1 tag : store physical register address
//     int rs1_value; // rs1 value

//     int rs2_arch; //rs2 arch 
//     int rs2_tag; //src2 tag : store physical register address
//     int rs2_value; // rs2 value

//     // src 3 fields
//     int rs3_arch; //rs3 arch
//     int rs3_tag;  //src3 tag : store physical register address 
//     int rs3_value; // rs3 value

//     // destination
//     int rd_arch; //rd arch
//     int phy_rd; //Destination physical register

//     int imm;

//     char opcode_str[128];

//     int mready;


// } ROB_entry;

// typedef struct ROB_entry_node ROB_entry_node;

// struct ROB_entry_node
// {
//     ROB_entry entry;
//     ROB_entry_node *next;
// };

// typedef struct REORDER_BUFFER
// {
//     ROB_entry_node *head;
//     ROB_entry_node *tail;
//     int size;
// } REORDER_BUFFER;

// REORDER_BUFFER *rob;
