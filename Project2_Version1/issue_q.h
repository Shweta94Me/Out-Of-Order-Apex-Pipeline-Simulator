/*
Siddhesh ::: Structure code for Issue Queue 
*/

#include "apex_macros.h"

#define maxIQSize 24


enum FU{
    Int_FU,
    Mul_FU
};

typedef struct node {
    int data;
    int pc;
    char opcode_str[128];
    enum FU FU_Type;
    int imm;
    int status; 
    
    // src 1 fields
    int rs1_ready; 
    int rs1; //rs1 tag
    int rs1_value; 
    int arc_rs1; 
    
    // src 2 fields
    int rs2_ready;
    int rs2; //rs2 tag
    int rs2_value;
    int arc_rs2;

    // destination
    int rd;
    int arc_rd;

    struct node *next;
}node;


typedef struct Queue
{
	/* data */
	struct node *front, *rear;
	int sizeOfQueue;
}Queue;

struct Queue* createQueue();
bool isFull(struct Queue* q);
bool isEmpty(struct Queue* q);
void printQueue(struct Queue* q);
void enQueue(struct Queue* q, int data);
void deQueueAnyNode(struct Queue* q,int val);





