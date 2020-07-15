#include <kbhit.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char** argv)
{
    int **key_val;
    key_val  = (int **)malloc(sizeof(int *));
    *key_val = (int  *)malloc(sizeof(int  ));

    while(1)
    {
        _kbhit(*key_val,argv[1]);
        if(**key_val)
        {
            printf("%d\r\n",**key_val);
            **key_val = 0;
        }
    }

    return 0;
}