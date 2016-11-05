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
int get_file(char *out_data,int len)
{
	char *ch = out_data;
	char *tmp = "\r\n";
	char *head =NULL;
	int head_len;
	char *file[10];
	FILE *fd;
	fd = fopen("aa.png","wb");


	file[0]=out_data;
	ch = memstr(out_data, len, tmp);
	LOG("echo","mylog", "dispot line[%s]",ch+2);
	file[1] = ch;//头标志行结束

	//存储头(分割线)数据
	head_len = ch-out_data;
	head = malloc(head_len+1);
	strncpy(head,out_data,head_len);
	head[head_len] = 0;


	ch = ch+2;
	file[2] = ch;//Content-Disposition行开始
	ch = memstr(ch, len-(ch-out_data), tmp);
	LOG("echo","mylog", "type [%s]",ch+2);
	file[3] = ch;//Content-Disposition行结束

	ch = ch+2;
	file[4] = ch;//Content-Type开始
	ch = memstr(ch, len-(ch-out_data), tmp);
	LOG("echo","mylog", "data [%s]",ch+4);
	file[5] = ch;//Content-Type结束

	ch = ch+4;
	file[6] = ch;//数据开始
	ch = memstr(ch, len-(ch-out_data), head);
	LOG("echo","mylog", "second head line[%s]",ch);
	file[7] = ch-2;//数据结束因为data尾有"\r\n"所有减2

	fwrite(file[6],1,file[7]-file[6],fd);
	fclose(fd);
	free(head);

//	ch = memstr(tmp, 2, ch);
//	LOG("echo","mylog", "first line[%s]",ch+2);

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
				putchar(ch);
				out_data[i] = ch;
            }
			get_file(out_data,len);
			if(out_data !=NULL)
			{
				free(out_data);
			}
            printf("\n</pre><p>\n");
        }

        PrintEnv("Request environment", environ);
        PrintEnv("Initial environment", initialEnv);
		char *str = getenv("QUERY_STRING");
		LOG("echo","mylog", "myecho[%s]",str);
    } /* while */

    return 0;
}
