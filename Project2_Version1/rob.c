#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <rob.h>

// function to create a queue
// of given capacity.
// It initializes size of queue as 0
void createROB()
{
    rob = (struct REORDER_BUFFER *)malloc(
        sizeof(struct REORDER_BUFFER));

    rob->entry = (ROB_entry *)malloc(sizeof(ROB_entry) * ROB_ENTRY_SIZE);

    rob->head = -1;
    rob->tail = 0;
    rob->size = 0;

    return rob
}

// Queue is full when size becomes
// equal to the capacity
int ROB_is_full()
{
    return (rob->size >= ROB_ENTRY_SIZE);
}

// Queue is empty when size is 0
int ROB_is_empty()
{
    return (rob->size == 0);
}

char instruction_type(CPU_Stage *instruction)
{
    switch (instruction->opcode_str)
    {
    case OPCODE_STORE:
    case OPCODE_LOAD:
    case OPCODE_LDR:
    case OPCODE_STR:
        return 'm';
    default:
    {
        return 'r';
    }
    }
}

// Function to add an item to the queue.
// It changes rear and size
void ROB_push(CPU_Stage *instruction)
{
    if (ROB_is_full())
        return;
    rob->entry[rob->tail].pc_value = instruction->pc;
    rob->entry[rob->tail].ar_address = instruction->rd;
    rob->entry[rob->tail].result = 0;
    rob->entry[rob->tail].sval_valid = 0; //for store instruction
    rob->entry[rob->tail].status = 0;     //marked as invalid
    rob->entry[rob->tail].inst_type = instruction_type(instruction);
    rob->tail++;
    rob->size++;
}

// Function to remove an item from queue.
// It changes front and size
ROB_entry ROB_pop()
{
    if (ROB_is_empty())
        return NULL;
    ROB_entry entry = rob->entry[rob->head];
    rob->entry[rob->head].status = 1; //marked as valid
    rob->head++;
    rob->size--;
    return entry;
}

void forward_to_rob(CPU_Stage *instruction)
{
    for (int i = 0; i < ROB_ENTRY_SIZE; i++)
    {
        if (instruction->pc == rob->entry[i].pc_value)
        {
            rob->entry[i].result = instruction->result_buffer;
            rob->entry[i].status = 1;
        }
    }
}
