#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <hiredis.h>
#include "make_log.h"
#include "redis_op.h"

#define REIDS_TEST_MODULE "test"
#define REIDS_TEST_PROC "redis_test"
int main(int argc, char *argv[])
{
    redisContext *redis_conn = NULL;
    int ret = 0;
    char value[100];

    redis_conn = rop_connectdb_nopwd("127.0.0.1", "6379");
    if (redis_conn == NULL) {
        LOG(REIDS_TEST_MODULE, REIDS_TEST_PROC, "conn error");
        exit(1);
    }


    ret = rop_set_string(redis_conn, "gailun", "lol");
    if (ret == -1) {
        LOG(REIDS_TEST_MODULE, REIDS_TEST_PROC, "set %s %s error", "gailun", "lol");
        exit(1);
    }
    ret = rop_get_string(redis_conn, "gailun", value);
    if(ret == 0)
    {
        printf("%s\n",value);
    }
     else
    {
        printf("%d\n",ret );
    }

    ret = rop_get_string(redis_conn, "gailun22", value);
    if(ret == 0)
    {
        printf("%s\n",value);
    }
    else
    {
        printf("errortype:%d\n",ret );
    }

	printf("ret:%d\n",rop_set_hash(redis_conn, "myhash", "keya1","11"));
	printf("ret:%d\n",rop_set_hash(redis_conn, "myhash", "keya1","ss"));
	printf("ret:%d\n",rop_set_hash(redis_conn, "myhash", "keya14","11"));
	
    ret = rop_get_hash(redis_conn, "myhash","keya1", value);
    if(ret == 0)
    {
        printf("%s\n",value);
    }
     else
    {
		printf("error get type:%d",ret);
	}

	return 0;
}
