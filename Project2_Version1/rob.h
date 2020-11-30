#include "apex_macros.h"
#include "apex_cpu.h"

#define ROB_ENTRY_SIZE 24

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

typedef struct REORDER_BUFFER
{
    ROB_entry *entry;
    int head, tail;
    int size;
} REORDER_BUFFER;

REORDER_BUFFER *rob;

void createROB();
int ROB_is_full();
int ROB_is_empty();
void ROB_push(CPU_Stage *instruction);
ROB_entry ROB_pop();
