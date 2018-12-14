#include "hook.h"
#include "mylib.h"


void* get_module_base(pid_t pid, const char* module_name)
{  
    FILE *fp;  
    long addr = 0;  
    char *pch;  
      
    //保存模块的名称  
    char filename[32];  
      
    //保存读取的信息  
    char line[1024];  
  
    if (pid < 0)  {  
        //获取当前进程的模块的基址  
        snprintf(filename, sizeof(filename), "/proc/self/maps");  
    }  
    else  {  
        //获取其他进程的模块的基址  
        snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);  
    }  
  
    //打开"/proc/pid/maps"文件  
    fp = fopen(filename, "r");  
  
    if (fp != NULL)  {  
        //循环读取"/proc/pid/maps"文件的信息，每次一行  
        while (fgets(line, sizeof(line), fp))  {  
            //判断读取的信息line中是否包含要查找的模块名称  
            if (strstr(line, module_name))  {  
                //以"-"为标记拆分字符串  
                pch = strtok(line, "-");                 
                //字符串转无符号长整型的模块基址  
                addr = strtoul(pch, NULL, 16 );  
                //排除特殊情况  
                if (addr == 0x8000)  
                    addr = 0;  
                break;  
            }  
        }  
        fclose( fp );  
    }  
    //返回获取到的模块的基址  
    return (void *)addr;  
}  

/*
void* get_module_base(pid_t pid, const char* module_name)
{
	int fd;
	long addr = 0;
	char *pch = NULL;
	//保存模块的名称  
    char filename[32];  
    //保存读取的信息  
    char line[1024];

    if(pid < 0 ) {
    	snprintf(filename, sizeof(filename), "/proc/self/maps"); 
    } else {
    	snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
    }

    fd = open(filename);
    if(fd < 0) {
    	LOGD("[+]open file '%s' failed[+]\n", filename);
    	return NULL;
    }

}

*/



int Hook(void* new_func,char* so_path,void* old_func)
{
    void* base_addr = NULL;
    int fd = -1;
    Elf32_Shdr shdr;
    Elf32_Ehdr ehdr;
    unsigned long shdr_addr;
    int shnum, shent_size,i;  
    unsigned long stridx;

    uint32_t out_addr = 0;
    uint32_t out_size = 0;
    uint32_t got_item = 0;
    int32_t got_found = 0;
    char * string_table = NULL;

    LOGD("[+]so path = %s [+]\n", so_path);
    if(so_path != NULL)
        base_addr = get_module_base(getpid(), so_path);
    LOGD("[+] %s address = %p [+]\n",so_path, base_addr);
 
    fd = open(so_path, O_RDONLY);
    if (fd == -1) {
        LOGD("[+] error: Open %s failed [+]\n", so_path);
        return -1;
    }
 
    read(fd, &ehdr, sizeof(Elf32_Ehdr));

    shdr_addr = ehdr.e_shoff;
    shnum = ehdr.e_shnum;
    shent_size = ehdr.e_shentsize;
    stridx = ehdr.e_shstrndx;
 
    lseek(fd, shdr_addr + stridx * shent_size, SEEK_SET);
    read(fd, &shdr, shent_size);

    string_table = (char *)malloc(shdr.sh_size);
    lseek(fd, shdr.sh_offset, SEEK_SET);
    read(fd, string_table, shdr.sh_size);
    lseek(fd, shdr_addr, SEEK_SET);

    for (i = 0; i < shnum; i++) {
        read(fd, &shdr, shent_size);
        if (shdr.sh_type == SHT_PROGBITS) {
            int name_idx = shdr.sh_name;
            if (strcmp(&(string_table[name_idx]), ".got.plt") == 0
                    || strcmp(&(string_table[name_idx]), ".got") == 0) {
                out_addr = (uint32_t)base_addr + shdr.sh_addr;
                out_size = shdr.sh_size;
                LOGD("[+] out_addr = %x, out_size = %x [+]\n", out_addr, out_size);

                for (i = 0; i < out_size; i += 4) {
                    got_item = *(uint32_t *)(out_addr + i);
                    
                    if (got_item  == (uint32_t)old_func) {
                        LOGD("[+] Found target function in got[+]\n");
                        got_found = 1;

                        uint32_t page_size = getpagesize();
                        uint32_t entry_page_start = (out_addr + i) & (~(page_size - 1));
                        mprotect((uint32_t *)entry_page_start, page_size, PROT_READ | PROT_WRITE);
                        *(uint32_t *)(out_addr + i) = (uint32_t)new_func;

                        break;
                    } else if (got_item == (uint32_t)new_func) {
                        LOGD("[+] Already hooked [+]\n");
                        break;
                    }
                }
                if (got_found)
                    break;
            }
        }
    }
    free(string_table);
    close(fd);

    return 0;
}