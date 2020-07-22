#ifndef __TREE_H
#define __TREE_H

typedef unsigned int Status;

#define TREE_DATA_TYPE char

/* BINARY TREE */
typedef struct BTree{
    TREE_DATA_TYPE data;
    struct BTree *rchild;
    struct BTree *lchild;
}*BNode;

extern char *ptr;
extern char *get;
/* APT */
Status Creat_Bt(BNode *t);
BNode PreOrder_Bt(BNode *t);
Status Show_BTree(BNode t);
void Get_Ptr(BNode t);
void Show_Graph(void);
#endif