
#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#define MAXLINE 40


static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t buffer_read(int, char *);
ssize_t readline(int, void *, ssize_t);
long _atol(const char *);

