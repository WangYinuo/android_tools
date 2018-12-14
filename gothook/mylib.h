#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#define MAXLINE 40


static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t buffer_read(int fd, char *ptr);
ssize_t readline(int fd, void *vptr, ssize_t maxlen);
//long _atol(char * str);

