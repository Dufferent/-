#include <tree.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"

Status Creat_BTree(BNode *t,unsigned int deep)
{
    BNode vl;//操作节点
    BNode vr;
    (*t) = (BNode)malloc(sizeof(struct BTree));
    vl = *t; //将操作节点与传入节点合并
    vr = *t;
    for(int i=0;i<deep-1;i++)
    {
        //一个结点的两个分支
        BNode newR;
        BNode newL;
        newL = (BNode)malloc(sizeof(struct BTree));
        newR = (BNode)malloc(sizeof(struct BTree));
        vl->lchild = newL;
        vl->rchild = newR;

        //切换到下一个节点
        vl=vl->lchild;
    }
    vr=vr->rchild;//根节点已经展开
    for(int i=0;i<deep-2;i++)
    {
        //一个结点的两个分支
        BNode newR;
        BNode newL;
        newL = (BNode)malloc(sizeof(struct BTree));
        newR = (BNode)malloc(sizeof(struct BTree));
        vr->lchild = newL;
        vr->rchild = newR;

        //切换到下一个节点
        vr=vr->rchild;
    }

    return 0;
}

Status Input_BTree(BNode *t)
{
    BNode v;
    v=*t;
    if(v->lchild != NULL)
        Input_BTree(&v->lchild);
    scanf("%d",&v->data);
    if(v->rchild != NULL)
        Input_BTree(&v->rchild);
    return 0;
}

Status Show_BTree(BNode t)
{
    /* 左中右遍历 */
    if(t->lchild != NULL)
        Show_BTree(t->lchild);
    printf("%d",t->data);
    if(t->rchild != NULL)
        Show_BTree(t->rchild);
    
    return 0;
}

Status Graph_BTree(BNode t)
{
    /* 左中右遍历 */
    if(t->lchild != NULL)
        Show_BTree(t->lchild);
    printf("%d",t->data);
    if(t->rchild != NULL)
        Show_BTree(t->rchild);
    
    return 0;
}