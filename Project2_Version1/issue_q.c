/*
Siddhesh ::: Implementation for Issue Queue 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "issue_q.h"
#include "apex_cpu.h"

struct node* newNode(node_attr data)
{
	struct node* temp = (struct node*)malloc(sizeof(struct node));
	temp->data = data;
	temp->next = NULL;
	return temp;
}

struct Queue* createQueue()
{
	struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
	q->front = q->rear = NULL;
	q->sizeOfQueue = 0;
	return q;
}


bool isQueueFull(struct Queue* q)
{
	if (q->sizeOfQueue < maxIQSize)
	{
		return false;
	}
	return true;
}

bool isQueueEmpty(struct Queue* q)
{
	if (q->front == NULL)
	{
		return true;
	}
	return false;
}

void enQueue(struct Queue* q, APEX_CPU *cpu)
{
	if(!isFull(q)){

		struct node_attr data = createData(cpu);

		struct node* temp = newNode(data);

		if (q->rear == NULL)
		{
			q->front = q->rear = temp;
			return;
		}
		q->rear->next = temp;
		q->rear = temp;
		q->sizeOfQueue += 1;
	}
	else{
		printf("Queue is Full");
		// return 0;
	}
	// return 1;
}


void deQueueAnyNode(struct Queue* q,int val){
	if(!isEmpty(q))
	{
		struct node* temp = q->front, *prev;

		if(temp->data.val == val){
			q->front = q->front->next;
			if (q->front == NULL)
			{
				q->rear = NULL;
			}

			free(temp);
			return;
		}
		
		while (temp != NULL && temp->data.val != val)
		{
			prev = temp;
			temp = temp->next;
		}

		if (temp == NULL) return;

		prev->next = temp->next;
		free(temp);

		return;

	}
	else{
		printf("Queue is Empty");
	}
}

void printQueue(struct Queue* q)
{
	struct node* temp = q->front;

	while(temp)
	{
		printf("%d ->", temp->data);
		temp = temp->next;
	}
	printf("\n");
}

node_attr createData(APEX_CPU *cpu){
    struct node_attr data;
    data.pc = cpu->decode.pc;
	strcpy(data.opcode_str, cpu->decode.opcode_str);

	data.rs1_tag = cpu->decode.rs1_phy_res;
	data.rs1_ready = cpu->decode.rs1_ready;
	data.rs1_value = cpu->decode.rs1_value;

	data.rs2_tag =cpu->decode.rs2_phy_res;
	data.rs2_ready = cpu->decode.rs2_ready;
	data.rs2_value = cpu->decode.rs2_value;

	data.rs3_tag =cpu->decode.rs3_phy_res;
	data.rs3_ready = cpu->decode.rs3_ready;
	data.rs3_value = cpu->decode.rs3_value;

	data.FU_Type = cpu->decode.fu_type;

	data.imm = cpu->decode.imm;
	data.status = 1;

	data.phy_rd = cpu->decode.rd_phy_res;

    return data;
}

void updateIQ(APEX_CPU *cpu, enum FU fu_type)
{
	struct node* temp = cpu->iq->front;

	while (temp){
		if(fu_type == Int_FU){
			if(temp->data.rs1_tag == cpu->ex_int_fu.rd_phy_res){
				temp->data.rs1_value = cpu->ex_int_fu.result_buffer;
				temp->data.rs1_ready = 1;
			}
			if(temp->data.rs2_tag == cpu->ex_int_fu.rd_phy_res){
				temp->data.rs2_value = cpu->ex_int_fu.result_buffer;
				temp->data.rs2_ready = 1;
			}
			if(temp->data.rs3_tag == cpu->ex_int_fu.rd_phy_res){
				temp->data.rs3_value = cpu->ex_int_fu.result_buffer;
				temp->data.rs3_ready = 1;
			}
		}

		temp = temp->next;
	}
	
}


