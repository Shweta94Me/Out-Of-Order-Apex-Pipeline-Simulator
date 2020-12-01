#include "apex_macros.h"
 #include "apex_cpu.h"

#define ROB_SIZE 24

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


// // Unit test code for rob 
// int main(){

//     createROB();
//     CPU_Stage cpustage;

//     cpustage.pc = 1;
//     cpustage.opcode = OPCODE_ADD;
//     cpustage.rd = 1;


//     printf("size -> %d \n", rob->size);  // 0
//     ROB_push(cpustage);
//     cpustage.pc = 2;
//     ROB_push(cpustage);
//     cpustage.pc = 3;
//     ROB_push(cpustage);
//     cpustage.pc = 4;
//     ROB_push(cpustage);
//     cpustage.pc = 5;
//     ROB_push(cpustage);
//     cpustage.pc = 6;
//     ROB_push(cpustage);
//     cpustage.pc = 7;
//     ROB_push(cpustage);
//     cpustage.pc = 8;
//     ROB_push(cpustage);  // this is 8th

//     printf("size -> %d \n", rob->size); // 7
//     ROB_pop();
//     ROB_pop();
//     ROB_pop();
//     ROB_pop();
//     ROB_pop();
//     ROB_pop();
//     ROB_pop();
//     ROB_pop();  // this is 8th pop()  

//     printf("size -> %d \n ", rob->size); // 0
//     cpustage.pc = 9;
//     ROB_push(cpustage);
//     ROB_pop();  //this pop cause segmentation fault
//     ROB_pop();
//     printf("size -> %d \n", rob->size); // 1

//     return 0;
// }

//-------------------------------- Please dont uncomment this below code---------------------------------///
// if(rob->head == NULL){
    //     rob->head = node;
    //     rob->size++;
    // }else{
    //     rob->tail->next = node;
    //     rob->size++;
    // }

    // rob->tail = node;
    // rob->tail->next = rob->head;



    // ROB_entry temp;
    // if(rob->head == rob->tail){
    //     temp = rob->head->entry;
    //     free(rob->head);
    //     rob->head = NULL;
    //     rob->tail = NULL;
    //     rob->size--;
    // }
    // else{
        
    //     ROB_entry_node* node = rob->head;
    //     temp = node->entry;
    //     rob->head = rob->head->next;
    //     rob->tail->next = rob->head;
    //     free(node);
    //     rob->size--;
    // }