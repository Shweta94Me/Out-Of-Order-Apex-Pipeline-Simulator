#include "apex_macros.h"
 #include "apex_cpu.h"

typedef struct ROB_entry
{
    int pc_value;   //instruction address
    int ar_address; //address of destination architectural register
    int result;     //result of reg-to-reg or mem address
    int sval_valid; //only for STORE
                    // int ex_codes;   //exception codes
    int status;     //indication if the result is valid
    char inst_type; //
} ROB_entry;

typedef struct ROB_entry_node ROB_entry_node;

struct ROB_entry_node
{
    ROB_entry entry;
    ROB_entry_node *next;
};

typedef struct REORDER_BUFFER
{
    ROB_entry_node *head;
    ROB_entry_node *tail;
    int size;
} REORDER_BUFFER;

REORDER_BUFFER *rob;

void createROB();
int ROB_is_full();
int ROB_is_empty();
void ROB_push(CPU_Stage);
void ROB_pop();
void forward_to_rob(CPU_Stage);
