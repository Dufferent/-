#ifndef __TREE_H
#define __TREE_H

typedef unsigned int Status;

#define TREE_DATA_TYPE int

/* BINARY TREE */
typedef struct BTree{
    TREE_DATA_TYPE data;
    struct BTree *rchild;
    struct BTree *lchild;
}*BNode;

/* APT */
Status Creat_BTree(BNode *t,unsigned int deep);
Status Input_BTree(BNode *t);
Status Show_BTree(BNode t);

#endif