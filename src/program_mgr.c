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
#include "config.h"
#include "menu_list.h"
#include "port.h"
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

#ifdef __cplusplus
extern "C" {
#endif


static uint32_t LE_TO_BE32(uint32_t x) 
{
    return (((x)&0xff)<<24) + (((x>>8)&0xff)<<16) + (((x>>16)&0xff)<<8) + (((x>>24)&0xff));
}

static void print32_t_copy_percents(int32_t numerator, int32_t denominator,int32_t del)
{
    if(del)
        sys_printf("%c%c%c%c",8,8,8,8);
    sys_printf("%3d%%",numerator*100/denominator);
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
    sys_printf("img head info:\r\n");
	//sys_printf("board index:%d\r\n",(uint32_t)(&head->board_name)-(uint32_t)(&head->magic));
    sys_printf("board name     : %s\r\n",(char*)head->board_name);
    sys_printf("cpu arch       : %s\r\n",(char*)head->arch_name);
    sys_printf("CPU model      : %s\r\n",(char*)head->cpu_name);
    sys_printf("img file name  : %s\r\n",(char*)head->img_name);
    sys_printf("img file lenth : %d\r\n",head->img_len);
    sys_printf("soft version   : %s\r\n",head->bin_ver);
    if(head->encry_type < 4);
        sys_printf("encrypt type   : %s\r\n",encty_type[head->encry_type]);
    sys_printf("\r\n");
}

static int head_endian_convert(img_head_s *head)
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

static uint32_t calc_img_crc(region_s *img)
{
    img_head_s *head;
	head = (img_head_s*)img->addr;
    return calc_crc32((uint8_t *)(img->addr+head->head_len),
                        img->lenth-head->head_len,0xffffffff);
}


void convert_uint32_to_byte(uint8_t *buf,int32_t index,uint32_t va)
{
    int32_t i;
    for(i = 0;i < 4;i ++)
    {
        buf[i + index] = (uint8_t)(va >> (i*8));
    }
}
int memory_compare(uint8_t *dest,uint8_t *src,int len)
{
    int i;
    for(i = 0;i < len;i ++)
    {
        if(dest[i] > src[i])
            return 1;
        else if(dest[i] < src[i])
            return -1;
    }
    return 0;
}

int string_len(const char *str)
{
    int i = 0;
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

int decrypt_img_data(region_s *img)
{
    int len;
    
    img_head_s *head;

	head = (img_head_s*)img->addr;
    sys_notice("img file decrypt");
    sys_debug("decrypt_data base:0x%x,lenth:%d",img->addr,img->lenth);
	len = decrypt_data(head->encry_type,(uint8_t *)(img->addr+head->head_len),
                        img->lenth-head->head_len);
    if(len < 0)
    {
        sys_warn("img file decrypt ERROR.");
        return -1;
    }
    img->lenth = (uint32_t)len;
    feed_watchdog();
    sys_notice("img file decrypt OK.");
    return 0;
}

int encrypt_img_data(region_s *img)
{
    int len;
    img_head_s *head;
	head = (img_head_s*)img->addr;
    sys_notice("img file encrypt");
    sys_debug("encrypt_data base:0x%x,lenth:%d",img->addr,img->lenth);
	len = encrypt_data(head->encry_type,(uint8_t *)img->addr,img->lenth);
    if(len < 0)
    {
        sys_warn("img file encrypt ERROR.");
        return -1;
    }
    img->lenth = (uint32_t)len;
    feed_watchdog();
    sys_notice("img file encrypt OK.");
    return 0;
    return 0;
}



int32_t check_img_valid(region_s *img)
{
    int32_t len;
    uint32_t cal_crc,crc;
    img_head_s *head;

    feed_watchdog();
	head = (img_head_s*)img->addr;
    if(0 != head_endian_convert(head))
    {
        sys_warn("img file head endian convert ERROR.");
        return -1;
    }
    
    cal_crc = calc_crc32((uint8_t*)head,head->head_len - 4,0xffffffff);
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
	crc = calc_crc32((uint8_t*)(img->addr+head->head_len),head->img_len - head->head_len,0xffffffff);
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


int32_t encrypt_code_calc_crc(region_s *img)
{
    int32_t len;
    sys_debug("decrypt_data base:0x%x,lenth:%d",img->addr,img->lenth);
    len = encrypt_data((uint8_t *)img->addr,img->lenth);
    if(len < 0)
    {
        sys_warn("img file encrypt ERROR.");
        return -1;
    }
    
    sys_notice("img file encrypt OK.");
    
    img->crc = calc_crc32((char *)img->addr,img->lenth,0xffffffff);
    convert_uint32_to_byte((uint8_t *)img->addr,img->lenth - 4,img->crc);
    
    sys_debug("new file CRC:0x%x.",img->crc);
    return 0;
}




int32_t copy_region_data(region_s *src,region_s *dest)
{
    int32_t i,j,len,blocks,times;
    uint32_t addr;
    uint8_t *buff = get_block_buffer();

    if(0 >= src->lenth)
        return 0;
    if(dest->maxlen < src->lenth)
    {
        sys_warn("space is NOT enough.");
        return -1;
    }
    sys_notice("copy data from %s to %s lenth %d.",
                src->regname,dest->regname,src->lenth);
    sys_debug("source type %s,addr 0x%x,lenth %d dest type,%s,addr 0x%x,lenth %d.",
                memtype_name(src->type),src->addr,src->lenth,
                memtype_name(dest->type),dest->addr,dest->maxlen);
    
    blocks = (src->lenth + BLOCK_SIZE - 1) / BLOCK_SIZE;
    sys_printf("complete:");
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
    sys_printf("\r\n");

    dest->lenth = src->lenth;
    dest->crc = src->crc;
    dest->status = MEM_NORMAL;

    sys_debug("copy data OK."); 
    return 0;    
}


int32_t flush_code_to_ram(region_s *img)
{
    int32_t ret;
    region_s bin;
    img_head_s *head = (img_head_s*)img->addr;
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    
    copy_region_info(img,&bin);
    bin.addr = img->addr + head->head_len;
    bin.lenth = img->lenth - head->head_len;
    bin.crc = calc_crc32((uint8_t *)bin.addr,bin.lenth,0xffffffff);
    
    sys_notice("begin to copy code to memory...");
    ret = copy_region_data(&bin,&bp->mem_map.run.ram);
    if(0 != ret)
    {
        sys_warn("copy img to running space failed.");
        return -1;
    }
    return 0;
}


//�ȱ���ԭ���ĳ�������¼�³���������
int32_t flush_code_to_rom(region_s *bin)
{
    int32_t ret;
    region_s *src;
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    sys_notice("begin to flush code to rom space...");
    
    //�Ƚ�ԭ���ĳ��򿽱������ݿռ�    
    src = &bp->mem_map.rom.program1_region;
    ret = copy_region_data(src,&bp->mem_map.rom.program2_region);
    if(0 != ret)
    {
        sys_warn("backup old program failed.");
        return -1;
    }

    //���³����յ����пռ�
    ret = copy_region_data(bin,&bp->mem_map.rom.program1_region);
    if(0 != ret)
    {
        sys_error("write new program failed.");
        return -1;
    }
    bp->mem_map.run.flash.status = MEM_NORMAL;
    
    bp->mem_map.run.flash.crc = bin->crc;
    bp->mem_map.run.flash.lenth= bin->lenth;    
    return 0;
}


int32_t flush_code_data(memtype_e type,region_s *img)
{
    int32_t ret;   
    switch(type)
    {
        case MEM_TYPE_RAM:
            ret = flush_code_to_ram(img);
            break;
        case MEM_TYPE_ROM:
            ret = flush_code_to_rom(img);
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

int32_t download_img_file(memtype_e type)
{
    int32_t ret,len;
    region_s *img;
    boot_param_s *bp = (boot_param_s*)get_boot_params();

    if(bp->debug_mode)
    {
        sys_notice("device can NOT download in debug mode ,set it to normal mode first");
        return -1;
    }
    img = &bp->mem_map.ram.probuf_region;
    sys_printf("begin to receive file data,please wait.\r\n");
    len = boot_receive_img(img->addr,img->maxlen);
    if(len <= 0)
    {
        sys_error("receive img data failed.");
        return -1;
    }

    img->lenth = (uint32_t)len;
    ret = check_img_valid(img);
    if(ret != 0)
    {
        sys_error("check img file ERROR");
        return -1; 
    }
    decrypt_img_data(img);
    ret = flush_code_data(type,img);
    if(0 != ret)
    {
        sys_warn("flush data to %s failed.",memtype_name(type));
        return -1;
    }
    sys_notice("img flush OK.");
    return 0;
}


int32_t write_encrypt_code_to_run(region_s *src,region_s *run)
{
    int32_t ret;
    region_s img;
    boot_param_s *bp = (boot_param_s*)get_boot_params();

    ret = copy_region_data(src,&bp->mem_map.ram.probuf_region);
    if(0 != ret)
    {
        sys_warn("copy data error.");
        return -1;
    }
    
    copy_region_info(&bp->mem_map.ram.probuf_region,&img);
    img.lenth = src->lenth;

    ret = check_img_valid(&img);
    if(0 != ret)
    {
        sys_error("check img file ERROR.");
        return -1;
    }
    decrypt_img_data(&img);
    ret = copy_region_data(&img,run);
    if(0 != ret)
    {
        sys_warn("flush data to running space error.");
        return -1;
    }
    return 0;
}

int32_t clean_program(void)
{
    int idx = 0;
    uint32_t i,blocknum;
    region_s *code[5];
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    sys_printf("clearing program ...\r\n");
    code[idx++] = &bp->mem_map.rom.program1_region;
    code[idx++] = &bp->mem_map.rom.program2_region;
    code[idx++] = &bp->mem_map.run.flash;
    code[idx++] = &bp->mem_map.rom.param1_region;
    code[idx++] = &bp->mem_map.rom.param2_region;
    
    for(i = 0;i < 5;i ++)
    {   
        sys_notice("erase base 0x%x,lenth %d.",code[i]->addr,code[i]->lenth);
        blocknum = (code[i]->lenth + BLOCK_SIZE - 1) / BLOCK_SIZE;
        erase_block(code[i]->type,code[i]->index,code[i]->addr,blocknum);
    }
    sys_printf("clear program OK.\r\n");
    return 0;
}


#ifdef __cplusplus
}
#endif
