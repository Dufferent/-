#ifndef __QUEUE_H
#define __QUEUE_H

typedef unsigned int Status;

#define QUEUE_TYPE char
#define QUEUE_ELEM_DEEP 32
#define QUEUE_SIZE QUEUE_ELEM_DEEP*sizeof(QUEUE_ELEM_DEEP)

#define OVERFLOW -1
#define INCREABLE -2
#define OKAY 0

/* qeueu structor */
typedef struct{
    QUEUE_TYPE *head;
    QUEUE_TYPE *end;
    QUEUE_TYPE *data;
    unsigned int len;
    unsigned int count;
}Queue;

/* API */
Status Creat_Queue(Queue *s,unsigned int len);
Status In_Queue(Queue *s,QUEUE_TYPE val);
QUEUE_TYPE Out_Queue(Queue *s);

#endif