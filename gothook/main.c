#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <elf.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include "hook.h"

#define LOG_TAG "DEBUG"  
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args) 

#define LIBSF_PATH "/data/app/com.example.hooktest-5kadmco69xW2f-DjxCRpVg==/lib/arm/libtest.so"

void *(*old_memcpy)(void *dest, const void *src, size_t n);
void *new_memcpy(void *dest, const void *src, size_t n)
{
	char *tmp = dest;
	int i = 0;
	LOGD("new_memcpy called\n");
	for(;i < n; i ++) {
		*(tmp + i) = '0';
	}
	return dest;
}

int (*old_rand)(void);
int new_rand(void)
{ 
	int ret;
    LOGD("new_rand called\n");  
    ret = old_rand();
    return 0;
}

int hook_entry(char * a)
{  
    LOGD("Hhook_entry\n");
    return 0;
}

__attribute__ ((__constructor__))
int Main()
{
    LOGD("Main Start hooking\n");
    old_memcpy = memcpy;
    old_rand = rand;
    Hook(new_memcpy, LIBSF_PATH, memcpy);
    Hook(new_rand, LIBSF_PATH, rand);
    LOGD("Hook done\n");
    return 0;
}