#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "make_log.h"
#define FDFS_LOG_MODULE "test"
#define FDFS_LOG_PROC "fdfs_file_info"

int fdfs_file_info(char *id,char *info)
{
	int fd[2];
/*	char *file_name = argv[1];
	if(argc < 2)
	{
		printf("usage: ./a.out <filename path>\n");
		return -1;
	}*/
	if(pipe(fd) == -1)
	{
		LOG(FDFS_LOG_MODULE, FDFS_LOG_PROC, "pip error");
		return -1;
	}
	pid_t pid = fork();
	if(pid == -1)
	{
		LOG(FDFS_LOG_MODULE, FDFS_LOG_PROC, "fork error");
		return -2;
	}
	if(pid > 0)
	{
		char buf[1024];
		memset(buf,0,1024);
		close(fd[1]);
		read(fd[0],buf,1024);
		strcpy(info,buf);
		close(fd[0]);
		wait(NULL);
	}
	else
	{
		close(fd[0]);
		dup2(fd[1],STDOUT_FILENO);
		execl("/usr/bin/fdfs_file_info" ,"fdfs_file_info","/etc/fdfs/client.conf", id,NULL);
		close(fd[1]);
	}
	return 0;
}
int fdfs_file_ip(char *id,char *ip)
{
	char *tmp_ip;
	char *tmp_ip_end;
	char info[1024] = {0};
	int ret;
	fdfs_file_info(id,info);
	tmp_ip = strstr(info,"source ip address");
	if(ip == NULL)
	{
		ret = -1;
		strcpy(ip,"wrong fileid");
		goto END;
	}
	tmp_ip = tmp_ip + strlen("source ip address")+ 2;
	tmp_ip_end = strstr(tmp_ip,"\n");
	*tmp_ip_end = '\0';
	strcpy(ip,tmp_ip);

	//printf("[%s]\n",tmp_ip);
END:
	return ret;
}
#if 1
int main(int argc,char *argv[])
{
	char info[1024];
	fdfs_file_info("group1/M00/00/00/wKiVglgfHdKAR_sOAAABxeCuiM8637.png",info);
	printf("%s",info);
	fdfs_file_ip("group1/M00/00/00/wKiVglgfHdKAR_sOAAABxeCuiM8637.png",info);
	printf("%s",info);
	return 0;
}
#endif
