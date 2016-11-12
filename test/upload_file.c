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
#include <stdio.h>
#include <string.h>
#include "fcgi_stdio.h"
#include "make_log.h"
#include "util_cgi.h"
#include "fdfs_op.h"
#include "redis_op.h"
#include "mycon.h"
#define FDFS_LOG_MODULE "test"
#define FDFS_LOG_PROC "fdfs_test"
int get_url(char *id,char *url)
{
	char ip[64] = {0};
	char my_url[1024] = {0};
	LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "url_id:[%s]",id);
	if(0 != fdfs_file_ip(id,ip))
	{
		LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "set fdfs_file_ip error");
		return -1;
	}
	strcpy(my_url,"http://");
	strcat(my_url,ip);
	strcat(my_url,"/");
	strcat(my_url,id);
	strcpy(url,my_url);
	return 0;
}
//更加数据流得到文件名和文件内容，并把文件内容存到当前路径下相应文件名
int get_file(char *out_data,int data_len,char *file_name)
{
	char *ch = out_data;
	char *tmp = "\r\n";
	char *head =NULL;
	int head_len;
	char *file[10];
	FILE *fd=NULL;
	char *file_name_begin;
	char *file_name_end;

	//获取文件名	
	file_name_begin =strstr(out_data,"filename=");
	file_name_begin = file_name_begin+strlen("filename=")+1;
	file_name_end = strchr(file_name_begin,'"');
	strncpy(file_name, file_name_begin, file_name_end - file_name_begin);
	file_name[file_name_end - file_name_begin] = 0;
	LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "file name:[%s]",file_name);

	fd = fopen(file_name,"wb");


	file[0]=out_data;
	ch = memstr(out_data, data_len, tmp);
	file[1] = ch;//头标志行结束

	//存储头(分割线)数据
	head_len = ch-out_data;
	head = malloc(head_len+1);
	strncpy(head,out_data,head_len);
	head[head_len] = 0;


	ch = ch+2;
	file[2] = ch;//Content-Disposition行开始
	ch = memstr(ch, data_len-(ch-out_data), tmp);
	file[3] = ch;//Content-Disposition行结束

	ch = ch+2;
	file[4] = ch;//Content-Type开始
	ch = memstr(ch, data_len-(ch-out_data), tmp);
	file[5] = ch;//Content-Type结束

	ch = ch+4;
	file[6] = ch;//数据开始
	ch = memstr(ch, data_len-(ch-out_data), head);
	file[7] = ch-2;//数据结束因为data尾有"\r\n"所有减2

	fwrite(file[6],1,file[7]-file[6],fd);
	fclose(fd);
	free(head);


	return 0;
}

int main ()
{
	char *out_data = NULL;
	redisContext *redis_conn;
	char id[128] = {0};//执行fdfs_upload_file后返回的id
	char file_name[1024]={0};
	char url[1024] = {0};
    char user[512] = {0};

	time_t now;   
	char str_time[100] = {0};   
    char fileid_list_table[512];//存储用户的文件列表的表名如："FILEID_LIST_aa"就是存储aa用户所有文ID的hash表表名
	  

    while (FCGI_Accept() >= 0) {

        int i, ch;
        char *contentLength = getenv("CONTENT_LENGTH");
        int len;
        len = strtol(contentLength, NULL, 10);
        printf("Content-type: text/html\r\n"
            "\r\n");

        out_data = (char*)malloc(sizeof(char)*len);//得到前端发送数据的长度并开辟存储空间
        if(out_data == NULL)
        {
            printf("upload error");
            goto END;
        }

		char *query_string = getenv("QUERY_STRING");
        query_parse_key_value(query_string, "user", user, NULL);
        memset(fileid_list_table,0,sizeof(fileid_list_table));
        strcat(fileid_list_table,"FILEID_LIST_");
        strcat(fileid_list_table,user);
        //读取上传的数据
        for (i = 0; i < len; i++) 
        {
            if ((ch = getchar()) < 0) 
            {
                printf("Error: Not enough bytes received on standard input<p>\n");
                break;
            }
           // putchar(ch);
            out_data[i] = ch;//得到前台发送的数据流
        }

        //取得文件内容，和文件名，并把数据写入对应文件名
        get_file(out_data,len,file_name);
        if(out_data !=NULL)
        {
            free(out_data);
        }


        //将文件数据上传到fdfs中,并获取id
        fdfs_upload_file(file_name,id);
        
        //删除临时文件
        unlink(file_name);

        //去除id中末尾的换行
        id[strlen(id)-1]=0;
        LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "id:[%s]",id);

        //初始化redis数据库
        redis_conn = rop_connectdb_nopwd(REDIS_SERVER_IP, REDIS_SERVER_PORT);
        if (redis_conn == NULL) {
            LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "conn error");
            //缺一个错误处理
        }
        //存储FILEID_LIST_user表（文件id的链表）
        if(rop_list_push(redis_conn, fileid_list_table, id) !=0)
        {
            LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "set FILEID_LIST error");
            //缺一个错误处理
        }

        //存储FILEID_PV_ZSET表
        if(rop_zset_increment(redis_conn, "FILEID_PV_ZSET", id) !=0)
        {
            LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "set FILEID_LIST error");
            //缺一个错误处理
        }
        
        
        //存储FILEID_NAME_HASH表（文件名）
        if(0 != rop_set_hash(redis_conn,"FILEID_NAME_HASH", id, file_name))
        {
            LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "set FILEID_NAME_HASH error");
            //缺一个错误处理
        }

        //存储FILEID_TIME_HASH表（时间）
        time(&now);   
        //timenow = localtime(&now);   
        //asctime_r(timenow,str_time);
        strftime(str_time,sizeof(str_time)-1 , "%Y-%m-%d %H:%M:%S", localtime(&now));
        if(0 != rop_set_hash(redis_conn,"FILEID_TIME_HASH", id, str_time))
        {
            LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "set FILEID_TIME_HASH error");
            //缺一个错误处理
        }
        
        //存储FILEID_URL_HASH表（URL路径）
        get_url(id,url);
        if(0 != rop_set_hash(redis_conn,"FILEID_URL_HASH", id, url))
        {
            LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "set FILEID_TIME_HASH error");
            //缺一个错误处理
        }
        printf("%s",url);//向前台发送存储结果


        //断开数据库
        rop_disconnect(redis_conn);

END:

        printf("\n</pre><p>\n");


    } /* while */

    return 0;
}
