#include <tree.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"
#include "string.h"

char *ptr;
char *get;
Status Creat_Bt(BNode *t)
{
    BNode v;//操作节点

    ptr = (char*)malloc(100*sizeof(char));
    memset(ptr,0,100);

    (*t) = (BNode)malloc(sizeof(struct BTree));//根节点
    v = (*t);

    printf("please input by preoder!\r\n");
    printf("such as:dca--b--e-f--\r\n");
    printf("\t\t      d     \r\n");
    printf("\t\t     /\\    \r\n");
    printf("\t\t    c  e    \r\n");
    printf("\t\t   /\\  \\  \r\n");
    printf("\t\t  a  b  f   \r\n");

    scanf("%s",ptr);
    //1.preorder vist
    PreOrder_Bt(&v);
}

BNode PreOrder_Bt(BNode *t)
{
    BNode v;
    v = (*t);
    if(v)
    {
        v->data = *ptr;
        printf("%c\r\n",*ptr);
        ptr++;
        v->lchild = NULL;
        v->rchild = NULL;
        if(*ptr != 45)//'-'的ascill是45
            v->lchild = (BNode)malloc(sizeof(struct BTree));
        else
            ptr++;
        
        v->lchild = PreOrder_Bt(&v->lchild);
        if(*ptr != 45)
            v->rchild = (BNode)malloc(sizeof(struct BTree));
        else
            ptr++;
        
        v->rchild = PreOrder_Bt(&v->rchild);
        return v;
    }
    else
    {
        return NULL;
    }
}
Status Show_BTree(BNode t)
{
    /* 先遍历 */
    if(t)
        printf("%c",t->data);
    
    if(t->lchild != NULL)
        Show_BTree(t->lchild);
    if(t->rchild != NULL)
        Show_BTree(t->rchild);
    
    return 0;
}

void Get_Ptr(BNode t)
{
    /* 先遍历 */
    static int i=1;
    if(i)
    {   
        get = (char*)malloc(100*sizeof(char));
        i--;
    }
    if(t)
    {
        *get = t->data;
        printf("%c",*get);
        get++;
    }
    
    if(t->lchild != NULL)
        Get_Ptr(t->lchild);
    if(t->rchild != NULL)
        Get_Ptr(t->rchild);
}

void Show_Graph(void)
{
    printf("\r\n");
    printf("\t\t      %c        \r\n",*(get-6));
    printf("\t\t     /\\        \r\n");
    printf("\t\t    %c  %c      \r\n",*(get-5),*(get-2));
    printf("\t\t   /\\  /\\     \r\n");
    printf("\t\t  %c %c    %c   \r\n",*(get-4),*(get-3),*(get-1));
}