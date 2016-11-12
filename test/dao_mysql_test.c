#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dao_mysql.h"

int main(int argc, char *argv[])
{
    MYSQL* conn = msql_conn("root", "huang", "mysql");
    if(conn == NULL)
    {
        printf("connn error\n");
    }
    else
    {
        printf("connn ok\n");
    }

	return 0;
}
