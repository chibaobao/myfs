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
#include "fdfs_upload_file.h"
#define FDFS_LOG_MODULE "test"
#define FDFS_LOG_PROC "fdfs_test"

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
static void PrintEnv(char *label, char **envp)
{
    printf("%s:<br>\n<pre>\n", label);
    for ( ; *envp != NULL; envp++) {
        printf("%s\n", *envp);
    }
    printf("</pre><p>\n");
}

int main ()
{
    char **initialEnv = environ;
    int count = 0;
	char *out_data = NULL;

    while (FCGI_Accept() >= 0) {
        char *contentLength = getenv("CONTENT_LENGTH");
        int len;


	printf("Content-type: text/html\r\n"
	    "\r\n"
	    "<title>FastCGI echo2</title>"
	    "<h1>FastCGI echo2</h1>\n"
            "Request number %d,  Process ID: %d<p>\n", ++count, getpid());

        if (contentLength != NULL) 
		{
            len = strtol(contentLength, NULL, 10);
        }
        else 
		{
            len = 0;
        }

        if (len <= 0) 
		{
			printf("No data from standard input.<p>\n");
        }
        else 
		{
            int i, ch;
			out_data = (char*)malloc(sizeof(char)*len);
			printf("Standard input:<br>\n<pre>\n");
            for (i = 0; i < len; i++) 
			{
                if ((ch = getchar()) < 0) 
				{
                    printf("Error: Not enough bytes received on standard input<p>\n");
                    break;
				}
				//putchar(ch);
				out_data[i] = ch;
            }

			//取得文件内容，和文件名，并把数据写入对应文件名
			char file_name[1024]={0};
			get_file(out_data,len,file_name);
			if(out_data !=NULL)
			{
				free(out_data);
			}


			//将文件数据上传到fdfs中,并获取id
			char id[128] = {0};
			fdfs_upload_file(file_name,id);

			//去除id中末尾的换行
			id[strlen(id)-1]=0;
			LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "id:[%s]",id);

            printf("\n</pre><p>\n");
        }

        PrintEnv("Request environment", environ);
        PrintEnv("Initial environment", initialEnv);
		char *str = getenv("QUERY_STRING");
		LOG(FDFS_LOG_MODULE,FDFS_LOG_PROC, "query:[%s]",str);

    } /* while */

    return 0;
}
