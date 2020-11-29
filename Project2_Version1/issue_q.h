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
    int rs1_ready; 
    int rs1_value; 
    int rs1_tag; //src1 tag : store physical register address 
    
    // src 2 fields
    int rs2_ready;
    int rs2_value;
    int rs2_tag; //src2 tag : store physical register address 

    // src 2 fields
    int rs3_ready;
    int rs3_value;
    int rs3_tag;  //src3 tag : store physical register address 

    // destination
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

struct Queue* createQueue();
bool isQueueFull(struct Queue* q);
bool isQueueEmpty(struct Queue* q);
void printQueue(struct Queue* q);
void enQueue(struct Queue* q, int data);
void deQueueAnyNode(struct Queue* q,int val);





