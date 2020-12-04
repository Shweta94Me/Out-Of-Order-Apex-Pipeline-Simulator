/*
Siddhesh ::: Implementation for Issue Queue 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "issue_q.h"

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


int isQueueFull(struct Queue* q)
{
	if (q->sizeOfQueue < maxIQSize)
	{
		return 0;
	}
	return 1;
}

int isQueueEmpty(struct Queue* q)
{
	if (q->front == NULL)
	{
		return 1;
	}
	return 0;
}

void enQueue(struct Queue* q, node_attr data)
{
	if(!isQueueFull(q)){

		// struct node_attr data = createData(cpu);

		struct node* temp = newNode(data);

		if (q->rear == NULL)
		{
			q->front = q->rear = temp;
			q->sizeOfQueue++;
			return;
		}
		q->rear->next = temp;
		q->rear = temp;
		q->sizeOfQueue++;
	}
	else{
		// printf("Queue is Full");
		return;
	}
	// return 1;
}


void deQueueAnyNode(struct Queue* q,int val){
	if(!isQueueEmpty(q))
	{
		struct node* temp = q->front, *prev;

		if(temp->data.pc == val){
			q->front = q->front->next;
			q->sizeOfQueue--;
			if (q->front == NULL)
			{
				q->rear = NULL;
			}
			free(temp);
			return;
		}
		
		while (temp != NULL && temp->data.pc != val)
		{
			prev = temp;
			temp = temp->next;
		}

		if (temp == NULL) return;

		prev->next = temp->next;
		q->sizeOfQueue--;
		free(temp);

		return;

	}
	else{
		//printf("Queue is Empty");
		return;
	}
}

void printQueue(struct Queue* q)
{
	struct node* temp = q->front;

	while(temp)
	{
		printf("%d ->", temp->data.pc);
		temp = temp->next;
	}
	printf("\n");
}

// // Unit test code for Issue q 
// int main(){

//     Queue* q = createQueue();

// 	printf("Val -> %d \n ", q->sizeOfQueue);  // 0
// 	node_attr data1;
// 	data1.pc = 1;
// 	enQueue(q,data1);

// 	node_attr data2;
// 	data2.pc = 2;
// 	enQueue(q,data2);

// 	node_attr data3;
// 	data3.pc = 3;
// 	enQueue(q,data3);

// 	node_attr data4;
// 	data4.pc = 4;
// 	enQueue(q,data4);

// 	node_attr data5;
// 	data5.pc = 5;
// 	enQueue(q,data5);

// 	node_attr data6;
// 	data6.pc = 6;
// 	enQueue(q,data6);

// 	printf("Val -> %d \n ", q->sizeOfQueue); // 5

// 	deQueueAnyNode(q,2);
// 	deQueueAnyNode(q,3);
// 	deQueueAnyNode(q,5);
// 	deQueueAnyNode(q,6);
// 	printQueue(q);
// 	deQueueAnyNode(q,1);
// 	deQueueAnyNode(q,4);

// 	printf("Val -> %d \n ", q->sizeOfQueue); // 0
	

// 	node_attr data7;
// 	data7.pc = 7;
// 	enQueue(q,data7);

// 	node_attr data8;
// 	data8.pc = 8;
// 	enQueue(q,data8);

// 	printQueue(q);
// 	printf("Val -> %d \n ", q->sizeOfQueue); // 2 

// 	deQueueAnyNode(q,8);
// 	deQueueAnyNode(q,7);

// 	printf("Val -> %d \n ", q->sizeOfQueue); // 0

// 	return 0;
// }


