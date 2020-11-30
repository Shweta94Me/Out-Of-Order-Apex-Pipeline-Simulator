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

void enQueue(struct Queue* q, node_attr data)
{
	if(!isFull(q)){

		// struct node_attr data = createData(cpu);

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




