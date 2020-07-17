#include "list.h"
#include "stdio.h"

int main(void)
{
    LNode my_list;
    int len;
    int ret;

    List_Creat(&my_list,10);
    len = Vist_List(my_list);
    Insert_List(&my_list,18,4);
    len = Vist_List(my_list);
    len = Search_List(my_list,18);
    printf("18 is in node %d\r\n",len);
    ret = Outof_List(&my_list,len);
    if(ret == 0)
    {
        printf("delete success!\r\n");
    }
    len = Vist_List(my_list);

    return 0;
}