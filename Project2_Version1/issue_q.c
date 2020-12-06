/*
Siddhesh ::: Implementation for Issue Queue 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "issue_q.h"

node *newNode(node_attr data)
{
	node *temp = (node*)malloc(sizeof(node));
	temp->data = data;
	temp->next = NULL;
	return temp;
}

void createQueue()
{
	iq = (Queue*)malloc(sizeof(Queue));
	iq->front = iq->rear = NULL;
	iq->sizeOfQueue = 0;
}


int isQueueFull()
{
	if (iq->sizeOfQueue < maxIQSize)
	{
		return 0;
	}
	return 1;
}

int isQueueEmpty()
{
	if (iq->front == NULL)
	{
		return 1;
	}
	return 0;
}

void enQueue(node_attr data)
{
	if(!isQueueFull()){

		// struct node_attr data = createData(cpu);

		node *temp = newNode(data);

		if (iq->rear == NULL)
		{
			iq->front = iq->rear = temp;
			iq->sizeOfQueue++;
			return;
		}
		iq->rear->next = temp;
		iq->rear = temp;
		iq->sizeOfQueue++;
	}
	else{
		// printf("Queue is Full");
		return;
	}
	// return 1;
}


void deQueueAnyNode(int val){
	if(!isQueueEmpty())
	{
		struct node* temp = iq->front, *prev;

		if(temp->data.pc == val){
			iq->front = iq->front->next;
			iq->sizeOfQueue--;
			if (iq->front == NULL)
			{
				iq->rear = NULL;
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
		iq->sizeOfQueue--;
		free(temp);

		return;

	}
	else{
		//printf("Queue is Empty");
		return;
	}
}

void printQueue()
{
	if(!isQueueEmpty()){
		struct node* temp = iq->front;

		while(temp)
		{
			printf("%d->", temp->data.pc);

			temp = temp->next;
		}
		printf("\n");
	}
	else
	{
		printf("Issue Queue is Empty. \n");
	}
	
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


