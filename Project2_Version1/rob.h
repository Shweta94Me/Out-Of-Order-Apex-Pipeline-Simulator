#ifndef _ROB_H_
#define _ROB_H_

#include "apex_macros.h"

#define ROB_SIZE 64

typedef struct ROB_entry
{
    int pc_value;   //instruction address
    int ar_address; //address of destination architectural register
    int result;     //result of reg-to-reg or mem address
    int sval_valid; //only for STORE
                    // int ex_codes;   //exception codes
    int status;     //indication if the result is valid
    // char inst_type; //

    int opcode;

    int rs1_arch; //rs1 arch
    int rs1_tag; //src1 tag : store physical register address
    int rs1_value; // rs1 value

    int rs2_arch; //rs2 arch 
    int rs2_tag; //src2 tag : store physical register address
    int rs2_value; // rs2 value

    // src 3 fields
    int rs3_arch; //rs3 arch
    int rs3_tag;  //src3 tag : store physical register address 
    int rs3_value; // rs3 value

    // destination
    int rd_arch; //rd arch
    int phy_rd; //Destination physical register

    int imm;

    char opcode_str[128];

    int mready;


} ROB_entry;

typedef struct ROB_entry_node ROB_entry_node;

struct ROB_entry_node
{
    ROB_entry entry;
    ROB_entry_node *next;
};

typedef struct REORDER_BUFFER
{
    ROB_entry_node *head;
    ROB_entry_node *tail;
    int size;
} REORDER_BUFFER;

REORDER_BUFFER *rob;

void createROB();
int ROB_is_full();
int ROB_is_empty();
void ROB_push(ROB_entry);
int ROB_headEntryValid();
int rob_head_peek();
void printROB();
void set_rob_mready_bit(int pc);
ROB_entry ROB_pop();
void forward_to_rob(int pc, int result_buffer);
#endif




