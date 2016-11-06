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
#if 1
int main(int argc,char *argv[])
{
	char id[1024];
	fdfs_upload_file("aa",id);
	printf("%s",id);
	return 0;
}
#endif
