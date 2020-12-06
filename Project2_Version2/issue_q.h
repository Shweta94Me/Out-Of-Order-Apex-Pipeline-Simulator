/*
Siddhesh ::: Structure code for Issue Queue 
*/

#ifndef _ISSUE_Q_H_
#define _ISSUE_Q_H_

#include "apex_macros.h"

#define maxIQSize 24


enum FU{
    Int_FU,
    Mul_FU,
    Mem_FU,
    JBU_FU
};

typedef struct node_attr{
    int val;
    int pc;
    char opcode_str[128];

    int opcode;

    enum FU FU_Type;
    int imm;
    int status; 
    
    // src 1 fields
    int rs1_arch; //rs1 arch 
    int rs1_ready; 
    int rs1_value; 
    int rs1_tag; //src1 tag : store physical register address 
    
    // src 2 fields
    int rs2_arch; //rs2 arch 
    int rs2_ready;
    int rs2_value;
    int rs2_tag; //src2 tag : store physical register address 

    // src 3 fields
    int rs3_arch; //rs3 arch
    int rs3_ready;
    int rs3_value;
    int rs3_tag;  //src3 tag : store physical register address 

    // destination
    int rd_arch; //rd arch
    int phy_rd; //Destination physical register

    //Bahaar :: For JBU_FU, needed in order to squash BIS and ROB Entries starting from this branch instr
    ROB_entry_node *rob_entry;

}node_attr;

typedef struct node {
    node_attr data;
    struct node *next;
}node;


typedef struct Queue
{
	/* data */
	struct node *front, *rear;
	int sizeOfQueue;
}Queue;

Queue *iq;

// Create the q. creation done in apex_cpu.c initialize
void createQueue();

// Check if q is full
int isQueueFull();

// check if q is empty
int isQueueEmpty();

// print the q
void printQueue();

// add entry to the tail of the q
void enQueue(node_attr data);

// delete the entry from q based on program counter
void deQueueAnyNode(int val);
#endif




