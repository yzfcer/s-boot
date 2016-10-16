/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
  *FileName:  
  *Author:      Zhou Jiangcun
  *Version:     1.0
  *Date:        2016/10/08
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#include "port.h"
#include "boot_debug.h"
#include "boot_hw_if.h"
#include "boot_param.h"
#include "mem_driver.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void boot_delay(uint32_t ms)
{
    uint32_t tick = boot_get_sys_ms();
    while(boot_get_sys_ms() - tick < ms);
}

int32_t wait_for_key_input(int32_t to_sec,char *ch,int32_t print_flag)
{
    int32_t second;
    uint32_t tick;
    int32_t ret = -1;
    if(to_sec <= 0)
        return -1;
    second = to_sec > 99?99:to_sec;
    tick = boot_get_sys_ms();
    if(print_flag)
        boot_printf("\r\nwaiting:%ds",second);
    while(1)
    {
        feed_watchdog();
        ret = boot_getchar_noblocking(ch);
        if(ret == 0)
        {
            ret = 0;
            break;
        }
        if(boot_get_sys_ms() - tick >= 1000)
        {
            tick = boot_get_sys_ms();
            second --;
            if(second <= 0)
            {
                break;
            }
            if(print_flag)
            {
                boot_printf("%c%c%c",8,8,8);
                boot_printf("%2ds",second);
            }
        }
    }
    boot_printf("\r\n");
    return ret;
}


int boot_printf(const char *fmt,...)
{
    int cnt;
    va_list argptr;
    static char outbuf[256];
    
    va_start(argptr, fmt);
    cnt = vsprintf(outbuf, fmt, argptr);
    va_end(argptr);
    boot_output(outbuf,cnt);
    return cnt;
}


int32_t read_char_blocking(char *ch)
{
    if(0 == wait_for_key_input(60,ch,0))
        return 0;
    boot_printf("you have NOT input any key in a 60 seconds,boot exit the menu list.\r\n");
    if(0 == wait_for_key_input(30,ch,1))
        return 0;
    return -1;
}

int32_t read_line_blockig(char *buff,int32_t len)
{
    int idx = 0;
    char ch;
	while(1)
    {
        if(0 == boot_getchar_noblocking(&ch))
        {
            if((ch == '\r') || 
                (ch == '\n') ||
                ((idx >= len - 1)))
            {
                buff[idx++] = 0;
                return idx;
            }
            else 
            {
                buff[idx++] = ch;
            }
        }
    }   
}

#if 0
typedef enum
{
    RECV_START,
    RECV_HANDLE,
    RECV_END
}recv_e;

typedef struct
{
    uint32_t stat;
    uint32_t idx;
    uint32_t mstick;
}recv_stat_s;

recv_stat_s g_recvstat;
void init_recv_stat(recv_stat_s *stat)
{
    stat->idx = 0;
    stat->mstick = boot_get_sys_ms();
    stat->stat = RECV_START;
}
static void wait_file_send_compete(void)
{
	int32_t ret;
	char ch;
	g_recvstat.stat = boot_get_sys_ms();
	while(1)
    {
        //ret = wait_for_key_input(3,&ch,0);
    	feed_watchdog();
    	boot_delay(100);
    	ret = boot_getchar_noblocking(&ch);
    	if(0 == ret)
        {
        	g_recvstat.stat = boot_get_sys_ms();
        }
    	if(boot_get_sys_ms() - g_recvstat.stat >= 2000)
    	return;
    }
}

int32_t boot_receive_img(uint32_t addr,uint32_t maxlen)
{
    int32_t ret;
    int32_t i,end;
    char *buf = (char*)addr;
    init_recv_stat(&g_recvstat);
    while(1)
    {
        switch(g_recvstat.stat)
        {
            case RECV_START:
                ret = wait_for_key_input(30,&buf[0],1);
                if(0 == ret)
                {
                    
                    g_recvstat.stat = RECV_HANDLE;
                    g_recvstat.idx = 1;
                    g_recvstat.mstick = boot_get_sys_ms();
                }
                else
                {
                    boot_notice("wait for file data time out.");
                    return -1;
                }
                break;
            case RECV_HANDLE:
                ret = boot_getchar_noblocking(&buf[g_recvstat.idx]);
                if(0 == ret)
                {
                    g_recvstat.idx ++;
                    if(g_recvstat.idx >= maxlen)
                    {
                        boot_warn("img file is too long.");
                                              wait_file_send_compete();
                                              return -1;
                    }
                    g_recvstat.mstick = boot_get_sys_ms();
                    if((g_recvstat.idx & 0x1ff) == 0)
                    {
                        boot_printf("#");
                    }
                    if((g_recvstat.idx & 0x7fff) == 0)
                    {
                        boot_printf("\r\n");
                    }
                }
                else
                {
                    if(boot_get_sys_ms() - g_recvstat.mstick > 1000)
                    {
                        g_recvstat.stat = RECV_END;
                    }
                }
                feed_watchdog();
                break;
            case RECV_END:
                boot_printf("\r\n");
                end = (g_recvstat.idx + BLOCK_SIZE - 1)/BLOCK_SIZE*BLOCK_SIZE;
                for(i = g_recvstat.idx;i < end; i ++)
                    buf[i] = 0;
                return g_recvstat.idx;
        }
    }
}
#endif

static int read_ram(uint32_t memidx,uint32_t realaddr,uint8_t *buf,int32_t lenth)
{
    int i;
    uint8_t *src;
    uint32_t base;
    uint32_t size;

    src = (uint8_t*)realaddr;
    base = get_ram_base(memidx);
    size = get_ram_lenth(memidx);
    if(realaddr - base + lenth >= size)
        return -1;
    for(i = 0;i < lenth;i ++)
    {
        buf[i] = src[i];
    }
    return lenth;
}

static int write_ram(uint32_t memidx,uint32_t realaddr,uint8_t *buf,int32_t lenth)
{
    int i;
    uint8_t *dest;
    uint32_t base;
    uint32_t size;

    dest = (uint8_t*)realaddr;
    base = get_ram_base(memidx);
    size = get_ram_lenth(memidx);
    if(realaddr - base + lenth >= size)
        return -1;

    for(i = 0;i < lenth;i ++)
    {
        dest[i] = buf[i];
    }
    return lenth;
}

int32_t read_block(uint8_t memtype,uint32_t memidx,uint32_t addr,uint8_t *buf,int32_t blkcount)
{
    int len;
    //boot_notice("read block base:0x%x",addr);
    switch(memtype)
    {
        case MEM_TYPE_RAM:
            len = read_ram(memidx,addr,buf,blkcount*BLOCK_SIZE);
            if(len < 0)
                return len;
            break;
        case MEM_TYPE_ROM:
            len = read_rom(memidx,addr,buf,blkcount*BLOCK_SIZE);
            if(len < 0)
                return len;
            break;
        default:
            return -1;
    }
    return blkcount;
}

int32_t write_block(uint8_t memtype,uint32_t memidx,uint32_t addr,uint8_t *buf,int32_t blkcount)
{
    int len; 
    //boot_notice("write block base:0x%x",addr);
    switch(memtype)
    {
        case MEM_TYPE_RAM:
            len = write_ram(memidx,addr,buf,blkcount*BLOCK_SIZE);
            if(len < 0)
                return len;
            break;
        case MEM_TYPE_ROM:
            len = write_rom(memidx,addr,buf,blkcount*BLOCK_SIZE);
            if(len < 0)
                return len;
            break;
        default:
            return -1;
    }
    return blkcount;
}

int32_t erase_block(uint8_t memtype,uint32_t memidx,uint32_t addr,int32_t blkcount)
{
    int len; 
    switch(memtype)
    {
        case MEM_TYPE_ROM:
            len = erase_rom(memidx,addr,blkcount*BLOCK_SIZE);
            if(len < 0)
                return len;
            break;
        default:
            return -1;
    }
	return blkcount;
}

#ifdef __cplusplus
}
#endif
