#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "make_log.h"
#define FDFS_LOG_MODULE "test"
#define FDFS_LOG_PROC "fdfs_test"

int fdfs_upload_file(char *file_name,char *id)
{
	int fd[2];
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
		strcpy(id,buf);
		close(fd[0]);
		wait(NULL);
	}
	else
	{
		close(fd[0]);
		dup2(fd[1],STDOUT_FILENO);
		execl("/usr/bin/fdfs_upload_file" ,"fdfs_upload_file","/etc/fdfs/client.conf", file_name,NULL);
		close(fd[1]);
	}
	return 0;
}
int fdfs_file_info(char *id,char *info)
{
	int fd[2];
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
	int ret = 0;
	fdfs_file_info(id,info);
	tmp_ip = strstr(info,"source ip address");
	if(ip == NULL)
	{
		ret = -1;
		strcpy(ip,"wrong fileid");
		LOG(FDFS_LOG_MODULE, FDFS_LOG_PROC, "do not have this file id");
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
#if 0
//test fdfs_upload_file
int main(int argc,char *argv[])
{
	char id[1024];
	fdfs_upload_file("aa",id);
	printf("%s",id);
	return 0;
}
#endif
#if 0
//test fdfs_file_info fdfs_file_ip
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
