#ifndef __STACK_H
#define __STACK_H

typedef unsigned int Status;
#define STACK_DEEP 10
#define STACK_TYPE int
#define STACk_SIZE STACK_DEEP*sizeof(STACK_TYPE)

/* Stack Status */
#define OVERFLOW  -1
#define INCREABLE -2
#define OKAY       0

/* stack structor */
/*
typedef struct {
    STACK_TYPE space[STACK_DEEP];
    STACK_TYPE *stack_pt;
}Stack;
*/

typedef struct {
    STACK_TYPE *space;
    STACK_TYPE *stack_pt;
    unsigned int count;
    unsigned int deep;
}Stack;


extern Stack my_stack;

/* API */
Status Creat_Stack(Stack *s,unsigned int deep);
Status Push_Stack(Stack *s,STACK_TYPE val);
STACK_TYPE Jump_Stack(Stack *s);
#endif