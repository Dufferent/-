#ifndef __LIST_H
#define __LIST_H

typedef unsigned int Status;
/* struct of list */
typedef struct list{
    int data;
    unsigned int num;
    struct list *next;
}*LNode;

extern LNode my_list;

/* API */
Status List_Creat(LNode *p,int len);
int Vist_List(LNode vp);
Status Insert_List(LNode *p,int val,int where);
int Search_List(LNode vp,int f_val);
Status Outof_List(LNode *p,int where);
#endif