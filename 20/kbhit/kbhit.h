#ifndef __KBHIT_H
#define __KGHIT_H

//if link kbhit.a please add -lpthread to compile APP

typedef struct{
    int key_val;
    int flag;
}kbhit;

int _kbhit(int *ret,char* argv);
void Task_1(void *arg);

#endif