/*
 * echo.c --
 *
 *	Produce a page containing all FastCGI inputs
 *
 *
 * Copyright (c) 1996 Open Market, Inc.
 *
 * See the file "LICENSE.TERMS" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */
#ifndef lint
static const char rcsid[] = "$Id: echo.c,v 1.5 1999/07/28 00:29:37 roberts Exp $";
#endif /* not lint */

#include "fcgi_config.h"

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif

#include <string.h>
#include "fcgi_stdio.h"
#include "util_cgi.h"
#include "make_log.h"
#include "redis_op.h"
#include "cJSON.h"

#define FDFS_LOG_MODULE "test"
#define FDFS_LOG_PROC "fdfs_test"

int main ()
{


    char *query = NULL;
    char c_fromid[32];
    char c_count[32];
    int fromid = 0;
    int count = 0;
    int ch_len = 0;

    cJSON *root = NULL;
    cJSON *array = NULL;
    cJSON *tmp = NULL;

    char c_tmp[100];

	redisContext *redis_conn;
    RVALUES list_value_p;
    int get_num = 8;
    char cmd[100];

    char *out_root = NULL;
    char id[100] = {0};
    int kind = 0;
    char title_m[1024] = {0};
    char title_s[1024] = {0};
    char descrip[64] = {0};//实际是creat_time
    char picurl_m[1024] = {0};
    char url[1024] = {0};
    int pv = 0;
    int hot = 0;
    int i = 0;

    strcpy(id,"12235id");
    strcpy(title_m,"12235tile_m");
    strcpy(title_s,"12235 title_s");
    strcpy(descrip,"1223 descrip");
    strcpy(picurl_m,"http://192.168.149.130/static/file_png/pdf.png");
    strcpy(url,"12235id url");
    kind = 2;
    pv =1;
    hot = 0;
	redis_conn = rop_connectdb_nopwd("127.0.0.1", "6379");
    if (redis_conn == NULL) 
    {
        LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "conn error");
        //缺一个错误处理
    }	
    while (FCGI_Accept() >= 0) 
    {
        printf("Content-type: text/html\r\n"
            "\r\n");
        //读取网页中QUERY_STRING中的变量值
        query = getenv("QUERY_STRING");
        
        query_parse_key_value(query, "cmd", cmd, &ch_len);
        //如果cmd指令是increase，也就是点击了下载按钮
        if(strcmp(cmd,"increase") == 0)
        {

            //读取QUERY_STRING中的变量
            query_parse_key_value(query, "fileId", id, &ch_len);
            LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "url id:[%s]",id);
            char *id_F[5];
            id_F[0] = (char*)id;
            for(i=1;i<5;i++)
            {
                id_F[i] =  strstr(id_F[i-1],"%2F");
                *(id_F[i]) = '/';
                *(id_F[i]+1) = '\0';
                id_F[i] = id_F[i] +3;
            }
            LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "url id:[%s]",id);
            for(i=1;i<5;i++)
            {
                strcpy(c_tmp,id_F[i]);
                strcat(id,c_tmp);
            }
            LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "url id2:[%s]",id);
            if(0 != rop_zset_increment(redis_conn, "FILEID_PV_ZSET", id))
            {
                LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "rop_zset_inc error");
            }

            continue;
        }

        query_parse_key_value(query, "fromId", c_fromid, &ch_len);
        fromid = atoi(c_fromid);
        query_parse_key_value(query, "count", c_count, &ch_len);
        count = atoi(c_count);
        //读取FILEID_LIST列表数据
        list_value_p   = malloc(count * VALUES_ID_SIZE);
        if(0 != rop_range_list(redis_conn, "FILEID_LIST", fromid, fromid+count-1, list_value_p, &get_num))
        {
            LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "rop_rang_list error");
        }
        //创建JSON语句
        root = cJSON_CreateObject();
        array = cJSON_CreateArray();
        for(i=0;i<get_num;i++)
        {

            //从redis中读取数据
            strcpy(id,list_value_p[i]);
            rop_get_hash(redis_conn, "FILEID_NAME_HASH", id, title_m);
            rop_get_hash(redis_conn, "FILEID_TIME_HASH", id, descrip);
            rop_get_hash(redis_conn, "FILEID_URL_HASH", id,url );
            pv = rop_zset_get_score(redis_conn, "FILEID_PV_ZSET", id);


            //将数据存入JSON
            tmp = cJSON_CreateObject();

            cJSON_AddStringToObject(tmp,"id",id);
            cJSON_AddNumberToObject(tmp,"kind",kind);
            cJSON_AddStringToObject(tmp,"title_m",title_m);
            cJSON_AddStringToObject(tmp,"title_s",title_s);
            cJSON_AddStringToObject(tmp,"descrip",descrip);
            cJSON_AddStringToObject(tmp,"picurl_m",picurl_m);
            cJSON_AddStringToObject(tmp,"url",url);
            cJSON_AddNumberToObject(tmp,"pv",pv);
            cJSON_AddNumberToObject(tmp,"hot",hot);

            cJSON_AddItemToArray(array,tmp);
        }
        free(list_value_p);
        cJSON_AddItemToObject(root,"games",array);
        
        out_root = cJSON_Print(root);

        printf("%s",out_root);

        cJSON_Delete(root);
        free(out_root);
    } /* while */
    rop_disconnect(redis_conn);

    return 0;
}
