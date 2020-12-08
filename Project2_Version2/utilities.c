/*
Shweta ::: Implementation of URF, RAT, R-RAT
*/
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "utilities.h"

void initializeURF(){
    for (int i = 0; i < URFMaxSize; i++)
    {
        urf[i].free = 0; //Intialize as free
        urf[i].status = 0; //Intialize as Invalid 
        urf[i].value = 0;
    }
}

void initializeRAT(){
    for (int i = 0; i < RATMaxSize; i++)
    {
        rat[i].phy_reg_num = -1;
    }
}

void initializeRRAT(){
    for (int i = 0; i < RATMaxSize; i++)
    {
        rrat[i].phy_reg_after_comit = -1;
    }
}


void initializeCheckPointRat(){
    checkpoint_RAT_entry entry;
    entry.ratFree = 1;  // 1 is for free and 0 is not free
    for (int i = 0; i < RATMaxSize; i++)
    {
        entry.Rat[i].phy_reg_num = -1;
    }
    for (int i = 0; i < CheckPointMaxSize; i++)
    {
        checkpointRat[i].entry = entry;
    }
}

void initializeCheckPointURF(){
    checkpoint_URF_entry entry;
    entry.urfFree = 1; // 1 is for free and 0 is not free
    for (int i = 0; i < URFMaxSize ; i++)
    {
        
        entry.Urf[i].free = 0; //Intialize as free
        entry.Urf[i].status = 0; //Intialize as Invalid 
        entry.Urf[i].value = 0;
    }
    for (int i = 0; i < CheckPointMaxSize; i++)
    {
        checkpointUrf[i].entry = entry;
    }
}

// if entry possible then index in checkpoint table rat else -1
int insertCheckpointRat(){

    // return shweta the index of where this checkpoint has insert in the array
    // if there is some space return -1
    checkpoint_RAT_entry entry;
    entry.ratFree = 0;  // making it not free

    // copying all the values
    for (int i = 0; i < RATMaxSize; i++)
    {
       entry.Rat[i].phy_reg_num = rat[i].phy_reg_num;
    }
    for (int i = 0; i < CheckPointMaxSize; i++)
    {
        if(checkpointRat[i].entry.ratFree){
            checkpointRat[i].entry = entry;
            return i;
        }
    }
    return -1;
}

int insertCheckpointURF(){

    checkpoint_URF_entry entry;
    entry.urfFree = 0; // making not free

    for (int i = 0; i < URFMaxSize ; i++)
    {
        entry.Urf[i].free = urf[i].free; //Intialize as free
        entry.Urf[i].status =  urf[i].status; //Intialize as Invalid 
        entry.Urf[i].value =  urf[i].value;
    }

    for (int i = 0; i < CheckPointMaxSize; i++)
    {
        if(checkpointUrf[i].entry.urfFree){
            checkpointUrf[i].entry = entry;
            return i;
        }
    }
    return -1;
}

// get checkpointed Rat based on index provided earlier during insert
Return_RAT getCheckPointedRAT(int index){

    Return_RAT chkptrat;
    chkptrat.valid = 0;
    if(index >= 0 && index < CheckPointMaxSize){
        for (int i = 0; i < CheckPointMaxSize; i++)
        {
            if(i == index){
                chkptrat.valid =1;
                for (int i = 0; i < RATMaxSize; i++)
                {
                    chkptrat.checkpointrat[i].phy_reg_num = checkpointRat[i].entry.Rat->phy_reg_num;
                }
                checkpointRat[i].entry.ratFree = 1; //freeing up the rat entry
                return chkptrat;
            }
        }
    
    }
    return chkptrat;
}

// get checkpointed URF based on index provided earlier during insert
Return_URF getCheckPointedURF(int index){
    Return_URF check_point_urf;
    check_point_urf.valid = 0;
    if(index >= 0 && index < CheckPointMaxSize){

        for (int i = 0; i < CheckPointMaxSize; i++)
        {
            if(i == index){
                check_point_urf.valid = 1;
                for (int i = 0; i < URFMaxSize; i++)
                {
                    check_point_urf.checkpointurf[i].free = checkpointUrf[i].entry.Urf->free;
                    check_point_urf.checkpointurf[i].status = checkpointUrf[i].entry.Urf->status;
                    check_point_urf.checkpointurf[i].value = checkpointUrf[i].entry.Urf->value;

                }
                checkpointUrf[i].entry.urfFree = 1; //freeing up the urf entry
                return check_point_urf;
            }
        }
    }
    return check_point_urf;
}

//Check if URF has a free entry and return that free entry index

int traverseURF()
{
    for(int i = 0; i < URFMaxSize; i++)
    {
        if(urf[i].free == 0)
        {
            return i;
        }
    }

    return -1; //No free entry available
}

int allocate_phy_dest_RAT(int rd)
{
    //Check free entry in URF
    int phy_reg = traverseURF(urf);

    if(phy_reg != -1){
        //Update RAT with available free physical register
        rat[rd].phy_reg_num = phy_reg; 
        //Mark free and status bit in URF 
        urf[phy_reg].free = 1; //Allocated
        urf[phy_reg].status = 0; //Invalid as there is an instruction which is updating this destination register
        return phy_reg;
    }

    return -1; // Can not allocate new physical register
}

int renameSrcWithPhyReg(int rs){
    int phy_reg = rat[rs].phy_reg_num; //Read physical register num from RAT
    return phy_reg;
}

int readSrcFromURF(int phy_reg){
    if(urf[phy_reg].status){
        return urf[phy_reg].value;
    }
    return -1;
}

//Shweta ::: Change status to Valid and load value from result
void updateURF(int result, int phy_res){
    urf[phy_res].value = result;
    urf[phy_res].status = 1;
}

void freeRegFromURF(int last_commited_phy_reg)
{   
    //Free URF physical register
    urf[last_commited_phy_reg].free = 0;
}

void updateRRAT(int phy_rd, int arch_idx)
{
    int last_commited_phy_reg = rrat[arch_idx].phy_reg_after_comit;
    if(last_commited_phy_reg != -1){
        //Renamer instruction came in
        freeRegFromURF(last_commited_phy_reg);
        rrat[arch_idx].phy_reg_after_comit = phy_rd; 
    }
    else{
        //Not a renamer instruction
        rrat[arch_idx].phy_reg_after_comit = phy_rd; 
        // freeRegFromURF(phy_rd);
    }
    //update with latest commited physical register
}

void printURF(){
    printf("\n-----------------URF---------------------\n");
    // printf("|\tRegister |\tValue |\tfree |\tstatus\n");
    for (int i = 0; i < URFMaxSize; i++)
    {
        char free[10];
        if(urf[i].free){
            strcpy(free,"alloc");
        }
        else{
             strcpy(free,"free");
        }

        char status[10];
        if(urf[i].status){
            strcpy(status,"valid");
        }
        else{
             strcpy(status,"invalid");
        }
        printf("|\tP[%d]\t|\t%d\t|\t%s\t|\t%s\n", i, urf[i].value,free, status);
    }
}

void printRAT(){
    printf("\n-----------------RAT---------------------\n");
    // printf("|\tarch\t|\tphys\t|\n");
    for (int i = 0; i < RATMaxSize; i++)
    {
       
        printf("|\tR[%d]\t|\t%d\t|\n", i, rat[i].phy_reg_num);
    }

}

void printRRAT(){
    printf("\n-----------------RRAT---------------------\n");
    // printf("|\tarch\t|\tphys\t|\n");
    for (int i = 0; i < RRATMaxSize; i++)
    {
       
        printf("|\tR[%d]\t|\t%d\t|\n", i, rrat[i].phy_reg_after_comit);
    }

}

void printArchToPhys(){
    printf("\n-----------------A-Phy---------------------\n");
    printf("|\tarch-reg\t|\tphy-reg-val\t|\n");
    for (int i = 0; i < RRATMaxSize; i++)
    {
       
        printf("|\t  R[%d]  \t|\t  %d      \t|\n", i, urf[rrat[i].phy_reg_after_comit].value);
    }
}


/////////////////////// JAL Stack Start /////////////////////////

/*
The JAL instruction is used to implement a function call and saves the return address in the destination
register named in the instruction. As an example, the instruction:
JAL R2, R5, #32
Calls the function whose first instruction is at the memory address obtained by adding the contents of R5
with literal 32. If the memory address of the JAL instruction is 64, the instruction following the JAL has
the address 68 (= 64+4), and this is saved as the return address in R2. A return from the function called in
this case can be implemented by the instruction:
JUMP R2, #0

This part is very imp for JAL when R2 is edited and we try to return back to R2

This assumes that R2 is not altered by the called function and anything else it calls. If R2 is used in the
called function, the called function must save it on a stack in memory and restore it to R2 before executing
the JUMP R2, #20 instruction to return to the caller’s code.
*/
 
StackNode* stackNewNode(JALStackEntry data)
{
    StackNode* stackNode = (StackNode*)malloc(sizeof(StackNode));
    stackNode->data = data;
    stackNode->next = NULL;
    return stackNode;
}
 
int isEmpty(struct StackNode* root)
{
    return !root;
}
 
void jal_push(StackNode** root, JALStackEntry data)
{
    // printf("start");
    StackNode* stackNode = stackNewNode(data);
    stackNode->next = *root;
    *root = stackNode;
    // printf("%d pushed to stack\n", data.val);
    // printf("end");

}
 
int jal_pop(StackNode** root)
{
    if (isEmpty(*root))
        return INT_MIN;
    StackNode* temp = *root;
    *root = (*root)->next;
    JALStackEntry popped = temp->data;
    free(temp);
 
    return popped.val;
}
 
int jal_peek(StackNode* root)
{
    if (isEmpty(root))
        return INT_MIN;
    return root->data.reg_tag;
}


// // unit test code for stack usage of test code is  same as rob and issueq
// int main()
// {
//     struct StackNode* root = NULL;

//     JALStackEntry entry;
//     entry.val = 100;
//     entry.reg_tag = 2;
//     jal_push(&root, entry);

//     entry.val = 200;
//     jal_push(&root, entry);

//     entry.val = 300;
//     jal_push(&root, entry);
 
//     printf("%d popped from stack\n", jal_pop(&root));
//     jal_pop(&root);
//     jal_pop(&root);
//     jal_pop(&root);
//     jal_pop(&root);
//     jal_pop(&root);

//     entry.val = 100;
//     jal_push(&root, entry);

//     entry.val = 200;
//     jal_push(&root, entry);

//     entry.val = 300;
//     jal_push(&root, entry);
 
//     printf("Top element is %d\n", jal_peek(root));
 
//     return 0;
// }


/////////////////////// JAL Stack End ///////////////////////////






