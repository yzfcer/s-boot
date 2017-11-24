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
#include "boot_port.h"
#include "sys_debug.h"
#include "boot_hw_if.h"
#include "boot_param.h"
#include "mem_driver.h"
#include <stdarg.h>
#include "boot_config.h"
#ifdef __cplusplus
extern "C" {
#endif

//通用缓存器，一般在拷贝数据时做缓存，或者接收命令字符
w_uint8_t commbuffer[BLOCK_SIZE];

void boot_delay(w_uint32_t ms)
{
    w_uint32_t tick = boot_get_sys_ms();
    while(boot_get_sys_ms() - tick < ms);
}

w_int32_t wait_for_key_input(w_int32_t to_sec,char *ch,w_int32_t print_flag)
{
    w_int32_t second;
    w_uint32_t tick;
    w_int32_t ret = -1;
    if(to_sec <= 0)
        return -1;
    second = to_sec > 99?99:to_sec;
    tick = boot_get_sys_ms();
    if(print_flag)
        wind_printf("\r\nwaiting:%ds",second);
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
                wind_printf("%c%c%c",8,8,8);
                wind_printf("%2ds",second);
            }
        }
    }
    wind_printf("\r\n");
    return ret;
}


w_int32_t wind_printf(const char *fmt,...)
{
    w_int32_t cnt;
    va_list argptr;
    static char outbuf[256];
    
    va_start(argptr, fmt);
    cnt = vsprintf(outbuf, fmt, argptr);
    va_end(argptr);
    boot_output(outbuf,cnt);
    return cnt;
}


w_int32_t read_char_blocking(char *ch)
{
    if(0 == wait_for_key_input(60,ch,0))
        return 0;
    wind_printf("you have NOT input any key in a 60 seconds,boot exit the menu list.\r\n");
    if(0 == wait_for_key_input(30,ch,1))
        return 0;
    return -1;
}

w_int32_t read_line_blockig(char *buff,w_int32_t len)
{
    w_int32_t idx = 0;
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
    w_uint32_t stat;
    w_uint32_t idx;
    w_uint32_t mstick;
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
	w_int32_t ret;
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

w_int32_t boot_receive_img(w_uint32_t addr,w_uint32_t maxlen)
{
    w_int32_t ret;
    w_int32_t i,end;
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
                    sys_notice("wait for file data time out.");
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
                        sys_warn("img file is too long.");
                                              wait_file_send_compete();
                                              return -1;
                    }
                    g_recvstat.mstick = boot_get_sys_ms();
                    if((g_recvstat.idx & 0x1ff) == 0)
                    {
                        wind_printf("#");
                    }
                    if((g_recvstat.idx & 0x7fff) == 0)
                    {
                        wind_printf("\r\n");
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
                wind_printf("\r\n");
                end = (g_recvstat.idx + BLOCK_SIZE - 1)/BLOCK_SIZE*BLOCK_SIZE;
                for(i = g_recvstat.idx;i < end; i ++)
                    buf[i] = 0;
                return g_recvstat.idx;
        }
    }
}
#endif

static w_int32_t read_ram(w_uint32_t memidx,w_uint32_t addr,w_uint8_t *buf,w_int32_t lenth)
{
    w_int32_t i;
    w_uint8_t *src;
    w_uint32_t base;
    w_uint32_t size;

    src = (w_uint8_t*)addr;
    base = get_ram_base(memidx);
    size = get_ram_lenth(memidx);
    if(addr - base + lenth >= size)
        return -1;
    for(i = 0;i < lenth;i ++)
    {
        buf[i] = src[i];
    }
    return lenth;
}

static w_int32_t write_ram(w_uint32_t memidx,w_uint32_t addr,w_uint8_t *buf,w_int32_t lenth)
{
    w_int32_t i;
    w_uint8_t *dest;
    w_uint32_t base;
    w_uint32_t size;

    dest = (w_uint8_t*)addr;
    base = get_ram_base(memidx);
    size = get_ram_lenth(memidx);
    if(addr - base + lenth >= size)
        return -1;

    for(i = 0;i < lenth;i ++)
    {
        dest[i] = buf[i];
    }
    return lenth;
}

w_uint8_t *get_block_buffer(void)
{
    return commbuffer;
}
w_int32_t read_block(w_uint8_t memtype,w_uint32_t memidx,w_uint32_t addr,w_uint8_t *buf,w_int32_t blkcount)
{
    w_int32_t len;
    //sys_notice("read block base:0x%x",addr);
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

w_int32_t write_block(w_uint8_t memtype,w_uint32_t memidx,w_uint32_t addr,w_uint8_t *buf,w_int32_t blkcount)
{
    w_int32_t len; 
    //sys_notice("write block base:0x%x",addr);
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

w_int32_t erase_block(w_uint8_t memtype,w_uint32_t memidx,w_uint32_t addr,w_int32_t blkcount)
{
    w_int32_t len; 
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
