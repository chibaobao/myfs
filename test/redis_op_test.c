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
    int i = 0;

    redis_conn = rop_connectdb_nopwd("127.0.0.1", "6379");
    if (redis_conn == NULL) {
        LOG(REIDS_TEST_MODULE, REIDS_TEST_PROC, "conn error");
        exit(1);
    }

    //string
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
    //hash
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
    //list
    rop_list_push(redis_conn,"list","sa");
    rop_list_push(redis_conn,"list","sb");
    rop_list_push(redis_conn,"list","sc");
    rop_list_push(redis_conn,"list","=====");
    rop_list_push(redis_conn,"list","1sa");
    rop_list_push(redis_conn,"list","1sb");
    rop_list_push(redis_conn,"list","1sc");

    RVALUES list_value_p   = malloc(9 * VALUES_ID_SIZE);
    int get_num = 0;
    rop_range_list(redis_conn, "list", 3, 8, list_value_p, &get_num);
    printf("num= %d\n", get_num);

    for(i=0;i<get_num;i++)
    {
        printf("value[%d]:[%s]\n",i,list_value_p[i]);
    }
    free(list_value_p);

    //sorted sets
    rop_zset_increment(redis_conn,"myset" , "one");
    printf("one value:%d\n", rop_zset_get_score(redis_conn,"myset" , "one"));
    rop_zset_increment(redis_conn,"myset" , "one");
    printf("one value:%d\n", rop_zset_get_score(redis_conn,"myset" , "one"));
    

	return 0;
}
