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
#include "boot_config.h"
#include "menu_list.h"
#include "boot_port.h"
#include "sys_debug.h"
#include "boot_param.h"
#include "boot_framework.h"
#include "sys_debug.h"
#include "crc.h"
#include "share_param.h"
#include "program_mgr.h"
#include "mem_map.h"
#include "boot_hw_if.h"
#include "mem_driver.h"
#include "encrypt.h"

#ifdef __cplusplus
extern "C" {
#endif


static w_uint32_t LE_TO_BE32(w_uint32_t x) 
{
    return (((x)&0xff)<<24) + (((x>>8)&0xff)<<16) + (((x>>16)&0xff)<<8) + (((x>>24)&0xff));
}

static void print32_t_copy_percents(w_int32_t numerator, w_int32_t denominator,w_int32_t del)
{
    if(del)
        wind_printf("%c%c%c%c",8,8,8,8);
    wind_printf("%3d%%",numerator*100/denominator);
        feed_watchdog();
}
static char *encty_type[] = 
{
    "no encrypt",
    "AES",
    "DES",
    "RC4"
};

static void print_img_head(img_head_s *head)
{
    wind_printf("img head info:\r\n");
	//wind_printf("board index:%d\r\n",(w_uint32_t)(&head->board_name)-(w_uint32_t)(&head->magic));
    wind_printf("board name     : %s\r\n",(char*)head->board_name);
    wind_printf("cpu arch       : %s\r\n",(char*)head->arch_name);
    wind_printf("CPU model      : %s\r\n",(char*)head->cpu_name);
    wind_printf("img file name  : %s\r\n",(char*)head->img_name);
    wind_printf("img file lenth : %d\r\n",head->img_len);
    wind_printf("soft version   : %s\r\n",head->bin_ver);
    if(head->encry_type < 4);
        wind_printf("encrypt type   : %s\r\n",encty_type[head->encry_type]);
    wind_printf("\r\n");
}

static w_int32_t head_endian_convert(img_head_s *head)
{
    if(head->endian_test = 0x12345678)
        return 0;
    else if(head->endian_test = 0x78563412)
    {
        LE_TO_BE32(head->img_len);
        LE_TO_BE32(head->head_len);
        LE_TO_BE32(head->head_ver);
        LE_TO_BE32(head->bin_crc);
        LE_TO_BE32(head->encry_type);
        LE_TO_BE32(head->endian_test);
        LE_TO_BE32(head->head_crc);
        return 0;        
    }
    return -1;        
}

w_int32_t memory_compare(w_uint8_t *dest,w_uint8_t *src,w_int32_t len)
{
    w_int32_t i;
    for(i = 0;i < len;i ++)
    {
        if(dest[i] > src[i])
            return 1;
        else if(dest[i] < src[i])
            return -1;
    }
    return 0;
}

w_int32_t string_len(const char *str)
{
    w_int32_t i = 0;
    if(str == NULL)
        return 0;
    while(str[i])
        i ++;
    return i;
}

bool_t check_hardware_matched(img_head_s *head)
{
    if(0 != memory_compare(head->board_name,BOARD_NAME,string_len(BOARD_NAME)))
    {
        sys_notice("dest board name:%s",BOARD_NAME);
        return B_FALSE;
    }
    if(0 != memory_compare(head->arch_name,ARCH_NAME,string_len(ARCH_NAME)))
    {
        sys_notice("dest arch name:%s",ARCH_NAME);
        return B_FALSE;
    }
    if(0 != memory_compare(head->cpu_name,CPU_NAME,string_len(CPU_NAME)))
    {
        sys_notice("dest cpu model:%s",CPU_NAME);
        return B_FALSE;
    }
    return B_TRUE;    
}

w_int32_t decrypt_img_data(region_s *img,region_s *bin)
{
    w_int32_t len;
    img_head_s *head;

    copy_region_info(img,bin);
	head = (img_head_s*)bin->addr;
    bin->addr += head->head_len;
    bin->size = img->size - head->head_len;
    bin->datalen = img->datalen - head->head_len;
    sys_notice("decrypt img file...");
    sys_debug("decrypt_data base:0x%x,lenth:%d",bin->addr,bin->datalen);
	len = decrypt_data(head->encry_type,(w_uint8_t *)bin->addr,bin->datalen);
    if(len < 0)
    {
        sys_warn("decrypt img file failed.");
        return -1;
    }
    bin->datalen = len;
    
    bin->crc = calc_crc32((w_uint8_t *)bin->addr,bin->datalen,0xffffffff);
    feed_watchdog();
    sys_notice("decrypt img file OK.");
    return 0;
}


w_int32_t check_img_valid(region_s *img)
{
    w_uint32_t cal_crc,crc;
    img_head_s *head;

    feed_watchdog();
	head = (img_head_s*)img->addr;
    if(0 != head_endian_convert(head))
    {
        sys_warn("img file head endian convert ERROR.");
        return -1;
    }
    
    cal_crc = calc_crc32((w_uint8_t*)head,head->head_len - 4,0xffffffff);
    crc = head->head_crc;
    
    sys_debug("img file head crc:0x%x,calc_crc:0x%x.",crc,cal_crc);
    if(cal_crc != crc)
    {
        sys_warn("img file head crc ERROR.");
        return -1;
    }
    print_img_head(head);
    if(!check_hardware_matched(head))
    {
        sys_warn("hardware is NOT matched.");
        return -1;
    }
    
    feed_watchdog();
	crc = calc_crc32((w_uint8_t*)(img->addr+head->head_len),head->img_len - head->head_len,0xffffffff);
    cal_crc = head->bin_crc;
    
    sys_debug("bin file crc:0x%x,calc_crc:0x%x.",crc,cal_crc);
    if(cal_crc != crc)
    {
        sys_warn("bin file crc ERROR.");
        return -1;
    }
    
    feed_watchdog();
    sys_notice("img file verify OK.");
    return 0;
}


w_int32_t copy_region_data(region_s *src,region_s *dest)
{
    w_int32_t i,j,len,blocks,times;
    w_uint32_t addr;
    w_uint8_t *buff = get_block_buffer();

    if(0 >= src->datalen)
        return 0;
    if(dest->size < src->datalen)
    {
        sys_warn("space is NOT enough.");
        return -1;
    }
    sys_notice("copy data from \"%s\" to \"%s\" lenth %d.",
                src->regname,dest->regname,src->datalen);
    sys_debug("source type %s,addr 0x%x,lenth %d dest type,%s,addr 0x%x,lenth %d.",
                memtype_name(src->type),src->addr,src->datalen,
                memtype_name(dest->type),dest->addr,dest->size);
    
    blocks = (src->datalen + BLOCK_SIZE - 1) / BLOCK_SIZE;
    wind_printf("complete:");
    print32_t_copy_percents(0,1,0);
    for(i = 0;i < blocks;i ++)
    {    
        for(times = 0;times < 3;times ++)
        {
            addr = src->addr + i * BLOCK_SIZE;
            if(i >= blocks - 1)
            {
                for(j = 0;j < BLOCK_SIZE;j ++)
                    buff[j] = 0;
            }
            len = read_block(src->type,src->index,addr,buff,1);
            if(len > 0)
                break;
        }
        if(times >= 3)
        {
            sys_warn("read block 0x%x,lenth %d failed.",addr,BLOCK_SIZE);
            dest->status = MEM_ERROR;
            return -1;
        }

        for(times = 0;times < 3;times ++)
        {
            addr = dest->addr + i * BLOCK_SIZE;
            len = write_block(dest->type,dest->index,addr,buff,1);
            if(len > 0)
                break;
        }
        if(times >= 3)
        {
            sys_warn("read block 0x%x,lenth %d failed.",addr,BLOCK_SIZE);
            dest->status = MEM_ERROR;
            return -1;
        }
        print32_t_copy_percents(i,blocks,1);
        feed_watchdog();
    }
    print32_t_copy_percents(i,blocks,1);
    wind_printf("\r\n");

    dest->datalen = src->datalen;
    dest->crc = src->crc;
    dest->status = MEM_NORMAL;

    sys_debug("copy data OK."); 
    return 0;    
}



static bool_t region_equal(region_s *src,region_s *dest)
{
    if(src->type != dest->type)
        return B_FALSE;
    if(src->index != dest->index)
        return B_FALSE;
    if(src->addr != dest->addr)
        return B_FALSE;
    return B_TRUE;
}

w_int32_t roll_back_program(void)
{
    w_int32_t ret;
    region_s *src;
    region_s bin;
    bool_t run_in_program1;
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    
    //先将原来的程序拷贝到备份空间    
    src = &bp->mem_map.rom.sys_program2;
    ret = copy_region_data(src,&bp->mem_map.rom.sys_program1);
    if(0 != ret)
    {
        sys_warn("roll back program failed.");
        return -1;
    }
    
    run_in_program1 = region_equal(&bp->mem_map.rom.sys_program1,&bp->mem_map.run.flash);
    if(run_in_program1)
    {
        bp->mem_map.run.flash.status = MEM_NORMAL;
        bp->mem_map.run.flash.crc = src->crc;
        bp->mem_map.run.flash.datalen= src->datalen;
    }
    else
    {
        copy_region_data(src,&bp->mem_map.ram.load_buffer);
        src = &bp->mem_map.ram.load_buffer;
        decrypt_img_data(src,&bin);
        ret = copy_region_data(&bin,&bp->mem_map.run.flash);
        if(0 != ret)
        {
            sys_error("flush program to running space failed.");
            return -1;
        }
    }
    param_flush();
    return 0;
}

w_int32_t flush_img_to_ram(region_s *img)
{
    w_int32_t ret;
    region_s bin;
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    decrypt_img_data(img,&bin);
    //这里需要修改，不能直接使用img，因为会修改程序缓存的起始地址，
    //另外需要重新考虑程序运行在RAM中间时，程序因该怎样存储
    
    sys_notice("begin to copy code to memory...");
    ret = copy_region_data(&bin,&bp->mem_map.run.ram);
    if(0 != ret)
    {
        sys_warn("copy img to running space failed.");
        return -1;
    }
    return 0;
}


//先备份原来的程序，再烧录新程序到sys_program1和运行区
w_int32_t flush_img_to_rom(region_s *img)
{
    w_int32_t ret;
    region_s *src,*dest;
    region_s bin;
    img_head_s *head;
    bool_t run_in_program1;
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    sys_notice("begin to flush code to rom space...");
    
    //先将原来的程序拷贝到备份空间    
    src = &bp->mem_map.rom.sys_program1;
    ret = copy_region_data(src,&bp->mem_map.rom.sys_program2);
    if(0 != ret)
    {
        sys_warn("backup old program failed.");
        return -1;
    }
    
    head = (img_head_s*)img->addr;
    src = &bp->mem_map.rom.sys_program1;
    dest = &bp->mem_map.run.flash;
    run_in_program1 = region_equal(src,dest);
    
    //烧录到sys_program1，如果同时也是运行空间，则先解密，在烧录
    if(run_in_program1)
    {
        decrypt_img_data(img,&bin);
        ret = copy_region_data(&bin,&bp->mem_map.rom.sys_program1);
        if(0 != ret)
        {
            sys_warn("flush new program failed.");
            return -1;
        }
        bp->mem_map.run.flash.status = MEM_NORMAL;
        bp->mem_map.run.flash.crc = bin.crc;
        bp->mem_map.run.flash.datalen= bin.datalen;
        param_flush();
        return 0;
    }
    else
    {
        ret = copy_region_data(img,&bp->mem_map.rom.sys_program1);
        if(0 != ret)
        {
            sys_warn("flush new program failed.");
            return -1;
        }
        decrypt_img_data(img,&bin);
        ret = copy_region_data(&bin,&bp->mem_map.run.flash);
        if(0 != ret)
        {
            sys_error("write new program failed.");
            return -1;
        }
    }
    param_flush();
    return 0;
}


w_int32_t flush_img_file(memtype_e type,region_s *img)
{
    w_int32_t ret;   
    switch(type)
    {
        case MEM_TYPE_RAM:
            ret = flush_img_to_ram(img);
            break;
        case MEM_TYPE_ROM:
            ret = flush_img_to_rom(img);
            break;
        default:
            sys_error("unknown memory type:%d",type);
            ret = -1;
            break;
    }
    if(0 != ret)
    {
        sys_warn("flush img data failed.");
        (void)get_boot_params_from_ROM();
        return ret;
    }
    if(MEM_TYPE_ROM == type)
        (void)param_flush();
    return ret;
}

w_int32_t download_img_file(memtype_e type)
{
    w_int32_t ret,len;
    region_s *img;
    boot_param_s *bp = (boot_param_s*)get_boot_params();

    if(bp->debug_mode)
    {
        sys_notice("device can NOT download in debug mode ,set it to normal mode first");
        return -1;
    }
    img = &bp->mem_map.ram.load_buffer;
    wind_printf("begin to receive file data,please wait.\r\n");
    len = boot_receive_img(img->addr,img->size);
    if(len <= 0)
    {
        sys_error("receive img data failed.");
        return -1;
    }

    img->datalen = (w_uint32_t)len;
    sys_notice("img file lenth:%d",img->datalen);
    ret = check_img_valid(img);
    if(ret != 0)
    {
        sys_error("check img file ERROR");
        return -1; 
    }
    ret = flush_img_file(type,img);
    if(0 != ret)
    {
        sys_warn("flush data to %s failed.",memtype_name(type));
        return -1;
    }
    sys_notice("img flush OK.");
    return 0;
}



w_int32_t clean_program(void)
{
    w_int32_t idx = 0;
    w_uint32_t i,blocknum;
    region_s *code[5];
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    wind_printf("clearing program ...\r\n");
    code[idx++] = &bp->mem_map.rom.sys_program1;
    code[idx++] = &bp->mem_map.rom.sys_program2;
    code[idx++] = &bp->mem_map.run.flash;
    code[idx++] = &bp->mem_map.rom.boot_param1;
    code[idx++] = &bp->mem_map.rom.boot_param2;
    
    for(i = 0;i < 5;i ++)
    {
        sys_notice("erase base 0x%x,lenth %d.",code[i]->addr,code[i]->datalen);
        blocknum = (code[i]->datalen + BLOCK_SIZE - 1) / BLOCK_SIZE;
        erase_block(code[i]->type,code[i]->index,code[i]->addr,blocknum);
    }
    wind_printf("clear program OK.\r\n");
    return 0;
}


#ifdef __cplusplus
}
#endif
