#include "mylib.h"
static ssize_t buffer_read(int fd, char *ptr)
{
	if (read_cnt <= 0)
	{
		again:
			if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0)
			{
				if (errno == EINTR)
					goto again;
				return(-1);
			} else if (read_cnt == 0)
				return(0);
			read_ptr = read_buf;
	}
	read_cnt--;
	*ptr = *read_ptr++;
	return(1);
}

ssize_t readline(int fd, void *vptr, ssize_t maxlen)
{
	ssize_t n, rc;
	char c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++)
	{
		if ((rc = buffer_read(fd, &c)) == 1)
		{
			*ptr++ = c;
			if (c == '\n')
				break;
		}
		else if (rc == 0)
		{
			*ptr = 0;
			return(n - 1);
		} else
			return(-1);
	}
	*ptr = 0;
	return(n);
}

long _atol(const char * str)
{
	long value = 0;
    long sign = 1;
    int radix;

	if(*str == '-'){
		sign = -1;
		str++;
	}
        
	if(*str == '0' && (*(str+1) == 'x' || *(str+1) == 'X')){
		radix = 16;
		str += 2;
	}else if(*str == '0') {
		radix = 8;
		str++;
	}else
		radix = 10;
        
	while(*str){
		if(radix == 16){
			if(*str >= '0' && *str <= '9')
				value = value * radix + *str - '0';    //0到9的字符串
			else if(*str >= 'A' && *str <= 'F')
				value = value * radix + *str - 'A' + 10;   //大写十六进制的ABCDEF的字符串的情况
			else if(*str >= 'a' && *str <= 'f')
				value = value * radix + *str - 'a' + 10;  // 小写十六进制的abcdef的字符串的情况
		}else
			value = value * radix + *str - '0';
		str++;
	}
	return sign*value;
}


unsigned long hex2dec(const char* str)
{
	return strtol(str, NULL, 16);
}

unsigned long bit2dec(const char* str)
{
	return strtol(str, NULL, 2);
}

unsigned long oct2dec(const char* str)
{
	return strtol(str, NULL, 8);
}