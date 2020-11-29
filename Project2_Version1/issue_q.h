/*
Siddhesh ::: Structure code for Issue Queue 
*/

#include "apex_macros.h"

#define maxIQSize 24


enum FU{
    Int_FU,
    Mul_FU
};

typedef struct node_attr{
    int val;
    int pc;
    char opcode_str[128];
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

    // src 2 fields
    int rs3_arch; //rs3 arch
    int rs3_ready;
    int rs3_value;
    int rs3_tag;  //src3 tag : store physical register address 

    // destination
    int rd_arch; //rd arch
    int phy_rd; //Destination physical register

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

// Create the q. creation done in apex_cpu.c initialize
struct Queue* createQueue();

// Check if q is full
bool isQueueFull(struct Queue* q);

// check if q is empty
bool isQueueEmpty(struct Queue* q);

// print the q
void printQueue(struct Queue* q);

// add entry to the tail of the q
void enQueue(struct Queue* q, int data);

// delete the entry from q based on program counter
void deQueueAnyNode(struct Queue* q,int val);

// issue the instruction from q based on 2 condition mentioned in this method
void issueInstruction(APEX_CPU *cpu)





