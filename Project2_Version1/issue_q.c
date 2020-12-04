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
// gcc -o issueq_test issue_q.c   /// this command is for individual issue q testing
//   ./issueq_test.exe

// int main(){

//     createQueue();

// 	printf("Val -> %d \n ", iq->sizeOfQueue);  // 0
// 	node_attr data1;
// 	data1.pc = 1;
// 	enQueue(data1);

// 	node_attr data2;
// 	data2.pc = 2;
// 	enQueue(data2);

// 	node_attr data3;
// 	data3.pc = 3;
// 	enQueue(data3);

// 	node_attr data4;
// 	data4.pc = 4;
// 	enQueue(data4);

// 	node_attr data5;
// 	data5.pc = 5;
// 	enQueue(data5);

// 	node_attr data6;
// 	data6.pc = 6;
// 	enQueue(data6);

// 	printf("Val -> %d \n ", iq->sizeOfQueue); // 5

// 	deQueueAnyNode(2);
// 	deQueueAnyNode(3);
// 	deQueueAnyNode(5);
// 	deQueueAnyNode(6);
// 	printQueue();
// 	deQueueAnyNode(1);
// 	deQueueAnyNode(4);

// 	printf("Val -> %d \n ", iq->sizeOfQueue); // 0
	

// 	node_attr data7;
// 	data7.pc = 7;
// 	enQueue(data7);

// 	node_attr data8;
// 	data8.pc = 8;
// 	enQueue(data8);

// 	printQueue();
// 	printf("Val -> %d \n ", iq->sizeOfQueue); // 2 

// 	deQueueAnyNode(8);
// 	deQueueAnyNode(7);

// 	printf("Val -> %d \n ", iq->sizeOfQueue); // 0

// 	return 0;
// }


