#include <stdio.h>
#include <tree.h>
#include <stdlib.h>
int main(void)
{
    BNode my_bt;
    printf("请输入二叉树...\r\n");
    //Creat_BTree(&my_bt,3);
    //Input_BTree(&my_bt);
    //Show_BTree(my_bt);

    Creat_Bt(&my_bt);
    Show_BTree(my_bt);
    Get_Ptr(my_bt);
    Show_Graph();

    return 0;
}