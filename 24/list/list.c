#include "list.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"

Status List_Creat(LNode *p,int len)
{
    /* 创建一个操作指针 */
    LNode vp;
    /* 给表头分配空间 */
    *p = (LNode)malloc(sizeof(struct list));
    if(p == NULL)
    {
        printf("malloc failed!\r\n");
        exit(-1);
    }
    /* 创建表头 */
    (*p)->data = 0;
    (*p)->num  = 1;
    (*p)->next = NULL;

    /* 将操作指针和传入指针合并 */
    vp = *p;

    for(int i=1;i<len;i++)
    {
        LNode new;
        new = (LNode)malloc(sizeof(struct list));
        if(new == NULL)
        {
            printf("malloc failed!\r\n");
            exit(-1);
        }
        new->data = 0;
        new->next = NULL;
        new->num  = i+1;        //因为有表头
        vp->next = new;
        vp = vp->next;            //转到新节点，继续追加
        //这里不能free回收资源，因为回收的将会是链表资源     
    }

    return 0;
}

int Vist_List(LNode vp)
{
    int len = 0;
    while(vp)//判断链表末尾
    {
        len++;
        printf("num is :%d\tdata is :%d\r\n",vp->num,vp->data);
        vp=vp->next;//转到下一个节点
        if(vp->next == NULL)//单独处理最后一个节点
        {
            printf("num is :%d\tdata is :%d\r\n",vp->num,vp->data);
            len++;
            break;
        }
    }
    return len;
}

Status Insert_List(LNode *p,int val,int where)
{
    /* 创建一个操作指针 */
    LNode vp;
    /* 将操作指针和传入指针合并 */
    vp = *p;
    int len = Vist_List(vp);

    if(where > len)
    {
        for(int i=0;i<where - len;i++)
        {
            LNode new;//因为超过链表最大长度，需要增加节点
            new = (LNode)malloc(sizeof(struct list));
            if(new == NULL)
            {
                printf("malloc failed!\r\n");
                exit(-1);
            }
            new->data = 0;
            new->next = NULL;
            new->num  = len+i+1;
            if(new->num == where)
            {
                new->data = val;
            }

            while(vp->next)//将指针移到尾部
            {
                vp = vp->next;
            }
            vp->next = new;//插入新节点
        }
    }

    if(where <= len)
    {
        LNode new;//需要增加插入节点
        new = (LNode)malloc(sizeof(struct list));
        for(int i=0;i<where-1-1;i++)//将指针移动到指定位置处的前一个位置
        {
            vp=vp->next;
        }
        new->data = val;
        new->num  = where;
        new->next = vp->next;
        vp->next  = new;

        //插入项后的节点标号递增1
        vp = vp->next;
        while(vp->next)
        {
            vp=vp->next;
            vp->num += 1;
        }
        //********************
    }

    return 0;
}

int Search_List(LNode vp,int f_val)//返回找到的节点num
{
    int len = 0;
    while(vp)//判断链表末尾
    {
        len++;
        //printf("num is :%d\tdata is :%d\r\n",vp->num,vp->data);
        if(vp->data == f_val)
        {
            printf("find the node by the val and the num is:%d\r\n",len);
            return len;
        }
        vp=vp->next;//转到下一个节点
        if(vp->next == NULL)//单独处理最后一个节点
        {
            //printf("num is :%d\tdata is :%d\r\n",vp->num,vp->data);
            len++;
            if(vp->data == f_val)
            {
                printf("find the node by the val and the num is:%d\r\n",len);
                return len;
            }
            break;
        }
    }
    return 0;//到尾部还没有匹配到返回0
}

Status Outof_List(LNode *p,int where)
{
    LNode vp;
    LNode dp;//要删除的节点的操作指针
    vp = *p;
    int len = Vist_List(vp);
    if(where > len)//非法删除
    {
        printf("give num is too big to the list!\r\n");
        return -1;
    }
    for(int i=1;i<=where-1-1;i++)//移动指针到要删除的列表项的前一个节点
    {
        vp=vp->next;
    }
    dp = vp->next;
    vp->next=vp->next->next;     //断开所要删除的节点的前驱
    dp->next=NULL;               //断开所要删除的节点的后继

    //删除列表项后，从删除节点以后列表项递减1
    while(vp->next)
    {
        vp=vp->next;
        vp->num -= 1;
    }

    return 0;
}