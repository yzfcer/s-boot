#include "port.h"
#include "boot_debug.h"
#include "boot_input_if.h"
#ifdef __cplusplus
extern "C" {
#endif
static void memory_copy(uint8_t *dest,uint8_t *dest,int32_t len)
{
    int i;
    for(i = 0;i < len;i ++)
    {
        dest[i] = src[i];
    }
}

static void memory_set(uint8_t *mem,uint8_t value,int32_t len)
{
    int i;
    for(i = 0;i < len;i ++)
    {
        mem[i] = value;
    }
}
int32_t wait_for_key_input(int32_t timeout_sec,char *ch,int32_t print_flag)
{
    int32_t second;
    uint32_t tick;
    int32_t ret = -1;
    if(timeout_sec <= 0)
        return -1;
    second = timeout_sec > 99?99:timeout_sec;
    tick = boot_get_sys_ms();
    if(print_flag)
        boot_printf("\r\nwaiting:%ds",second);
    while(1)
    {
        feed_watchdog();
        ret = read_char_noblocking(ch);
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

#ifdef MCU_DEVICE
int32_t read_char_blocking(char *ch)
{
    if(0 == wait_for_key_input(60,ch,0))
        return 0;
    boot_printf("you have NOT input any key in a 60 seconds,boot exit the menu list.\r\n");
    if(0 == wait_for_key_input(30,ch,1))
        return 0;
    return -1;
}


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
    	ret = read_char_noblocking(&ch);
    	if(0 == ret)
        {
        	g_recvstat.stat = boot_get_sys_ms();
        }
    	if(boot_get_sys_ms() - g_recvstat.stat >= 2000)
    	return;
    }
}

int32_t receive_img_data(uint32_t addr,uint32_t maxlen)
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
                ret = read_char_noblocking(&buf[g_recvstat.idx]);
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


int32_t init_block(void)
{
    return 0;
}

int32_t write_block(uint8_t memtype,uint32_t addr,uint8_t *buf,int32_t blkcount)
{
    uint8_t *base;
    switch(memtype)
    {
        case MEM_TYPE_RAM:
            base = get_iram_base();
            memory_copy((uint8_t*)(base+addr*BLOCK_SIZE),buf,blkcount);
            break;
        case XRAM:
            base = get_xram_base();
            memory_copy((uint8_t*)(base+addr*BLOCK_SIZE),buf,blkcount);
            break;
        case MEM_TYPE_ROM:
            break;
        case XFLASH:
            break;
        default:
            boot_warn("unkown memory type:%d.",memtype);
            return -1;
    }
    return blkcount;
}


int32_t read_block(uint8_t memtype,uint32_t addr,uint8_t *buf,int32_t blkcount)
{
    uint8_t *base;
    switch(memtype)
    {
        case MEM_TYPE_RAM:
            base = get_iram_base();
            memory_copy(buf,(uint8_t*)(base+addr*BLOCK_SIZE),blkcount);
            break;
        case XRAM:
            base = get_xram_base();
            memory_copy(buf,(uint8_t*)(base+addr*BLOCK_SIZE),blkcount);
            break;
        case MEM_TYPE_ROM:
            break;
        case XFLASH:
            break;
        default:
            boot_warn("unkown memory type:%d.",memtype);
            return -1;
    }
    return blkcount;
}


int32_t erase_block(uint8_t memtype,uint32_t addr,int32_t blkcount)
{
    uint32_t base;
    switch(memtype)
    {
        case MEM_TYPE_RAM:
            base = get_iram_base();
            memory_set((uint8_t*)(base+addr*BLOCK_SIZE),0,blkcount);
            break;
        case XRAM:
            base = get_iram_base();
            memory_set((uint8_t*)(base+addr*BLOCK_SIZE),0,blkcount);
            break;
        case MEM_TYPE_ROM:
            break;
        case XFLASH:
            break;
        default:
            boot_warn("unkown memory type:%d.",memtype);
            return -1;
    }
    return blkcount;
}


#ifdef __cplusplus
}
#endif
