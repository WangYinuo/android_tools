#include "mylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <errno.h>

#define BUFF 1024

//参数: pid dump file
int main(int argc, char const *argv[])
{
	int fd;
	int fd_dest;
	unsigned long startaddr = 0,endaddr = 0,map_len;
	size_t pid = 0;
	char buffer[BUFF] = {0};
	int buff;
    char* tmp = NULL;
	char path[100] = "";
	char* delim = NULL;
	char* str =NULL;
	char* ptr = NULL;
	size_t result = 0;

	if(argc < 4) {
		printf("[+] Error: less args [+]\n");
		printf("[+] ./memdump pid startaddr endaddr save [+]\n");
		goto Failed;
	}
	tmp = (char*)malloc(BUFF);
	pid = atoi(argv[1]);
	sprintf(path,"/proc/%d/maps",pid);
	printf("[+] path:%s [+]\n",path);
	fd = open(path,O_RDONLY|O_CREAT);

	if(fd < 0) {
		printf("[+] Error: Open file %s failed [+]\n",path);
		goto Failed;
	}

    startaddr = _atol(argv[2]);
    endaddr = _atol(argv[3]);
    printf("[+] startaddr:%lu [+]\n",startaddr);
    printf("[+] endaddr:%lu [+]\n",endaddr);

    map_len = endaddr - startaddr - 1;
    printf("[+] map_len:%lu [+]\n",map_len);
    memset(path,0,sizeof(path));
    sprintf(path,"/proc/%d/mem",pid);

	if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
		fprintf(stderr, "[+] ptrace attach failed. [+]\n");
		perror("ptrace");
		goto Failed;
	}
	if (waitpid(pid, NULL, 0) == -1) {
		fprintf(stderr, "[+] waitpid failed. [+]\n");
		perror("waitpid");
		ptrace(PTRACE_DETACH, pid, NULL, NULL);
		goto DETACH;
	}

    fd = open(path,O_RDONLY);
    if(fd <= 0) {
    	printf("[+] Error: Open file %s failed [+]\n",path);
		goto DETACH;
    }

    lseek(fd,startaddr,SEEK_SET);
    result = 0;
    fd_dest = open(argv[4], O_CREAT | O_TRUNC | O_RDWR, 0666);
    if(fd_dest == -1) {
    	fprintf(stderr, "[+] fopen failed. [+]\n");
		perror("fopen");
		ptrace(PTRACE_DETACH, pid, NULL, NULL);
		goto DETACH;
    }

    for(;startaddr < endaddr;startaddr += sizeof(int)) {
        errno = 0;
        if(((buff = ptrace(PTRACE_PEEKDATA,pid,(void*)startaddr,NULL)) == -1) && errno) {
            perror("PTRACE_KEEPDATA");
            if(ptrace(PTRACE_DETACH,pid,NULL,NULL))
                perror("PTRACE_DETACH");
            goto DETACH;
        }
        if(write(fd_dest,&buff,sizeof(buff)) != 4) {
            perror("write");
            if(ptrace(PTRACE_DETACH,pid,NULL,NULL))
                perror("PTRACE_DETACH\n");
            goto DETACH;
        }
    }
DETACH:
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
Failed:
	if(fd > 0)
		close(fd);
	if(fd_dest > 0)
		close(fd_dest);
	if(tmp != NULL) {
		free(tmp);
		tmp = NULL;
	}
	return 0;
}
