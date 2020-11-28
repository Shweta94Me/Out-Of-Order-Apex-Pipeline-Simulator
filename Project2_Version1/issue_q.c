/*
Siddhesh ::: Implementation for Issue Queue 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "issue_q.h"

struct node* newNode(int data)
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


bool isFull(struct Queue* q)
{
	if (q->sizeOfQueue < maxIQSize)
	{
		return false;
	}
	return true;
}

bool isEmpty(struct Queue* q)
{
	if (q->front == NULL)
	{
		return true;
	}
	return false;
}

void enQueue(struct Queue* q, int data)
{
	if(!isFull(q)){
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
	}
}


void deQueueAnyNode(struct Queue* q,int val){
	if(!isEmpty(q))
	{
		struct node* temp = q->front, *prev;

		if(temp->data == val){
			q->front = q->front->next;
			if (q->front == NULL)
			{
				q->rear = NULL;
			}

			free(temp);
			return;
		}
		
		while (temp != NULL && temp->data != val)
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

int main()
{
	struct Queue* q = createQueue();

	enQueue(q, 1);
	enQueue(q, 2);
	enQueue(q, 3);
	printQueue(q);
	deQueueAnyNode(q, 2);
	deQueueAnyNode(q, 1);
	printQueue(q);
	deQueueAnyNode(q, 3);
	enQueue(q, 4);
	printf("Queue Front: %d \n", q->front->data);
	printf("Queue Rear : %d", q->rear->data);
	return 0;
}


