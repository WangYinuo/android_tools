#include <unistd.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <android/log.h>  
#include <elf.h>  
#include <fcntl.h>  
#include "inlineHook.h" 


#define LOG_TAG "DEBUG"  
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args) 


int (*old_puts)(const char *) = NULL;

int new_puts(const char *string)
{
	LOGD("new_puts");
    old_puts("inlineHook success");
}

int hook()
{
    if (registerInlineHook((uint32_t) puts, (uint32_t) new_puts, (uint32_t **) &old_puts) != ELE7EN_OK) {
        return -1;
    }
    if (inlineHook((uint32_t) puts) != ELE7EN_OK) {
        return -1;
    }

    return 0;
}

int unHook()
{
    if (inlineUnHook((uint32_t) puts) != ELE7EN_OK) {
        return -1;
    }

    return 0;
}

__attribute__ ((__constructor__))
int Main(){
    LOGD("On Main pid = %d\n", getpid());
    hook();
    
    return 0;
}