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
#include "cJSON.h"


int main ()
{
    cJSON *root = NULL;
    cJSON *array = NULL;
    cJSON *tmp = NULL;
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

    strcpy(id,"12235id");
    strcpy(title_m,"12235tile_m");
    strcpy(title_s,"12235 title_s");
    strcpy(descrip,"1223 descrip");
    strcpy(picurl_m,"http://192.168.149.130/static/file_png/pdf.png");
    strcpy(url,"12235id url");
    kind = 2;
    pv =1;
    hot = 0;

    while (FCGI_Accept() >= 0) 
    {
        printf("Content-type: text/html\r\n"
            "\r\n");
        root = cJSON_CreateObject();
        array = cJSON_CreateArray();
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
        cJSON_AddItemToObject(root,"games",array);
        
        out_root = cJSON_Print(root);

        printf("%s",out_root);

        cJSON_Delete(root);
        free(out_root);

    } /* while */

    return 0;
}
