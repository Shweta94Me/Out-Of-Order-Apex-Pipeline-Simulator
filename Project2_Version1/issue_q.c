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

		if(temp->data.pc == val){
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

	data.rs1_arch = cpu->decode.rs1;
	data.rs1_tag = cpu->decode.rs1_phy_res;
	data.rs1_ready = cpu->decode.rs1_ready;
	data.rs1_value = cpu->decode.rs1_value;

	data.rs2_arch = cpu->decode.rs2;
	data.rs2_tag =cpu->decode.rs2_phy_res;
	data.rs2_ready = cpu->decode.rs2_ready;
	data.rs2_value = cpu->decode.rs2_value;

	data.rs3_arch = cpu->decode.rs3;
	data.rs3_tag =cpu->decode.rs3_phy_res;
	data.rs3_ready = cpu->decode.rs3_ready;
	data.rs3_value = cpu->decode.rs3_value;

	data.FU_Type = cpu->decode.fu_type;

	data.imm = cpu->decode.imm;
	data.status = 1;

	data.phy_rd = cpu->decode.rd_phy_res;
	data.rd_arch = cpu->decode.rd;

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
		else if (fu_type == Mul_FU)
		{
			if(temp->data.rs1_tag == cpu->ex_mul_fu.rd_phy_res){
				temp->data.rs1_value = cpu->ex_mul_fu.result_buffer;
				temp->data.rs1_ready = 1;
			}
			if(temp->data.rs2_tag == cpu->ex_mul_fu.rd_phy_res){
				temp->data.rs2_value = cpu->ex_mul_fu.result_buffer;
				temp->data.rs2_ready = 1;
			}
			if(temp->data.rs3_tag == cpu->ex_mul_fu.rd_phy_res){
				temp->data.rs3_value = cpu->ex_mul_fu.result_buffer;
				temp->data.rs3_ready = 1;
			}
		}
		else if (fu_type == Mem_FU)
		{
			if(temp->data.rs1_tag == cpu->mem2.rd_phy_res){
				temp->data.rs1_value = cpu->mem2.result_buffer;
				temp->data.rs1_ready = 1;
			}
			if(temp->data.rs2_tag == cpu->mem2.rd_phy_res){
				temp->data.rs2_value = cpu->mem2.result_buffer;
				temp->data.rs2_ready = 1;
			}
			if(temp->data.rs3_tag == cpu->mem2.rd_phy_res){
				temp->data.rs3_value = cpu->mem2.result_buffer;
				temp->data.rs3_ready = 1;
			}
		}
		temp = temp->next;
	}

	free(temp);
	
}

/*
Logic for isusing a instruction from the issue queue is based on two conditions if that fu type required by instruction and operands are
available then issue the instruction from the queue
else
iterate in the q and look for the instruction that satisfies this condition
*/

void issueInstruction(APEX_CPU *cpu){

	struct node* temp = cpu->iq->front;

	int int_fu_flag = 0; // using this flag to know if we sent any instruction in int fu
	int mul_fu_flag = 0; // using thus flag to know if we sent any instruction in mul fu

	while (temp){

		/* suppose we have mul instruction at front of the q and mul fu is free then we issue mul instruction to the mul fu
		but in the same clock cycle we will try to issue instruction that uses int fu if condition to dispatch is satisfied. 
		By this way we can achiveve parallelism in function unit. thats why seperate if instead of if else
		*/

	    //0 is free 1 is not free
		if(temp->data.FU_Type == Mul_FU && !cpu->mul_fu_free && temp->data.rs1_ready && temp->data.rs2_ready){
			cpu->mul_fu_free = 1; // making mul fu not free. Also by doing this we ensure only one instruction is sent to int fu , mul fu  
			// and ... even if the while loop continues till the end of the q. Early stopping is not possible

			// only copying all the important data from iq to next stage i.e int_fu
			cpu->ex_mul_fu.fu_type = temp->data.FU_Type;
			cpu->ex_mul_fu.has_insn = 1;
			cpu->ex_mul_fu.imm = temp->data.imm;
			strcpy(cpu->ex_mul_fu.opcode_str,temp->data.opcode_str);

			cpu->ex_mul_fu.rd_phy_res  =temp->data.phy_rd;
			cpu->ex_mul_fu.rd = temp->data.rd_arch;

			cpu->ex_mul_fu.rs1 =temp->data.rs1_arch;
			cpu->ex_mul_fu.rs1_phy_res = temp->data.rs1_tag;
			cpu->ex_mul_fu.rs1_value = temp->data.rs1_value;

			cpu->ex_mul_fu.rs2 =temp->data.rs2_arch;
			cpu->ex_mul_fu.rs2_phy_res = temp->data.rs2_tag;
			cpu->ex_mul_fu.rs2_value = temp->data.rs2_value;

			cpu->ex_mul_fu.rs3 =temp->data.rs3_arch;
			cpu->ex_mul_fu.rs3_phy_res = temp->data.rs3_tag;
			cpu->ex_mul_fu.rs3_value = temp->data.rs3_value;
			
			//deleting this node from the q 
			deQueueAnyNode(cpu->iq, temp->data.pc);

			//setting flag to 1
			int_fu_flag = 1;
		}


		if(temp->data.FU_Type == Int_FU && !cpu->int_fu_free  && temp->data.rs1_ready && temp->data.rs2_ready){
			cpu->int_fu_free = 1;  // making int fu not free. Also by doing this we ensure only one instruction is sent to int fu , mul fu  
			// and ... even if the while loop continues till the end of the q. Early stopping is not possible

			// only copying all the important data from iq to next stage i.e int_fu
			cpu->ex_int_fu.fu_type = temp->data.FU_Type;
			cpu->ex_int_fu.has_insn = 1;
			cpu->ex_int_fu.imm = temp->data.imm;
			strcpy(cpu->ex_int_fu.opcode_str,temp->data.opcode_str);

			cpu->ex_int_fu.rd_phy_res  =temp->data.phy_rd;
			cpu->ex_int_fu.rd = temp->data.rd_arch;

			cpu->ex_int_fu.rs1 =temp->data.rs1_arch;
			cpu->ex_int_fu.rs1_phy_res = temp->data.rs1_tag;
			cpu->ex_int_fu.rs1_value = temp->data.rs1_value;

			cpu->ex_int_fu.rs2 =temp->data.rs2_arch;
			cpu->ex_int_fu.rs2_phy_res = temp->data.rs2_tag;
			cpu->ex_int_fu.rs2_value = temp->data.rs2_value;

			cpu->ex_int_fu.rs3 =temp->data.rs3_arch;
			cpu->ex_int_fu.rs3_phy_res = temp->data.rs3_tag;
			cpu->ex_int_fu.rs3_value = temp->data.rs3_value;

			//deleting this node from the q  
			deQueueAnyNode(cpu->iq, temp->data.pc);

			//setting flag to 1
			mul_fu_flag = 1
		}

		temp = temp->next;
	}

	//using the above flag to ensure has instruction is made 0 if data is not pushed from iq to fu
	//this help us in apex_cpu.c to decide if we need to run int fu or mul fu or ... unit
	if(!int_fu_flag){
		cpu->ex_int_fu.has_insn = 0;
	}
	if(!mul_fu_flag){
		cpu->ex_mul_fu.has_insn = 0;
	}

	free(temp);
}


