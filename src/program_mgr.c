/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
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
#include "wind_debug.h"
#include "boot_param.h"
#include "boot_framework.h"
#include "wind_debug.h"
#include "wind_crc32.h"
#include "share_param.h"
#include "program_mgr.h"
#include "boot_part.h"
#include "boot_hw_if.h"
#include "mem_driver.h"
#include "encrypt.h"
#include "phy_mem.h"
#include "wind_string.h"
#ifdef __cplusplus
extern "C" {
#endif


static w_uint32_t LE_TO_BE32(w_uint32_t x) 
{
    return (((x)&0xff)<<24) + (((x>>8)&0xff)<<16) + (((x>>16)&0xff)<<8) + (((x>>24)&0xff));
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


w_bool_t check_hardware_matched(img_head_s *head)
{
    if(0 != wind_memcmp(head->board_name,BOARD_NAME,wind_strlen(BOARD_NAME)))
    {
        wind_notice("dest board name:%s",BOARD_NAME);
        return B_FALSE;
    }
    if(0 != wind_memcmp(head->arch_name,ARCH_NAME,wind_strlen(ARCH_NAME)))
    {
        wind_notice("dest arch name:%s",ARCH_NAME);
        return B_FALSE;
    }
    if(0 != wind_memcmp(head->cpu_name,CPU_NAME,wind_strlen(CPU_NAME)))
    {
        wind_notice("dest cpu model:%s",CPU_NAME);
        return B_FALSE;
    }
    return B_TRUE;    
}

w_int32_t decrypt_img_data(part_s *img,part_s *bin)
{
    w_int32_t len;
    img_head_s *head;

    part_copy_info(img,bin);
	head = (img_head_s*)bin->addr;
    bin->addr += head->head_len;
    bin->size = img->size - head->head_len;
    bin->datalen = img->datalen - head->head_len;
    wind_notice("decrypt img file...");
    wind_debug("decrypt_data base:0x%x,lenth:%d",bin->addr,bin->datalen);
	len = decrypt_data(head->encry_type,(w_uint8_t *)bin->addr,bin->datalen);
    if(len < 0)
    {
        wind_warn("decrypt img file failed.");
        return -1;
    }
    bin->datalen = len;
    
    bin->crc = wind_crc32(0xffffffff,(w_uint8_t *)bin->addr,bin->datalen);
    feed_watchdog();
    wind_notice("decrypt img file OK.");
    return 0;
}


w_int32_t check_img_valid(part_s *img)
{
    w_uint32_t cal_crc,crc;
    img_head_s *head;

    feed_watchdog();
	head = (img_head_s*)img->addr;
    if(0 != head_endian_convert(head))
    {
        wind_warn("img file head endian convert ERROR.");
        return -1;
    }
    
    cal_crc = wind_crc32(0xffffffff,(w_uint8_t*)head,head->head_len - 4);
    crc = head->head_crc;
    
    wind_debug("img file head crc:0x%x,calc_crc:0x%x.",crc,cal_crc);
    if(cal_crc != crc)
    {
        wind_warn("img file head crc ERROR.");
        return -1;
    }
    print_img_head(head);
    if(!check_hardware_matched(head))
    {
        wind_warn("hardware is NOT matched.");
        return -1;
    }
    
    feed_watchdog();
	crc = wind_crc32(0xffffffff,(w_uint8_t*)(img->addr+head->head_len),head->img_len - head->head_len);
    cal_crc = head->bin_crc;
    
    wind_debug("bin file crc:0x%x,calc_crc:0x%x.",crc,cal_crc);
    if(cal_crc != crc)
    {
        wind_warn("bin file crc ERROR.");
        return -1;
    }
    
    feed_watchdog();
    wind_notice("img file verify OK.");
    return 0;
}






static w_bool_t region_equal(part_s *src,part_s *dest)
{
    if(src->memtype != dest->memtype)
        return B_FALSE;
    if(src->memidx != dest->memidx)
        return B_FALSE;
    if(src->addr != dest->addr)
        return B_FALSE;
    return B_TRUE;
}

w_int32_t roll_back_program(void)
{
    w_int32_t ret;
    part_s *src,*dest,*tmp1;
    part_s bin;
    w_bool_t run_in_program1;
    boot_param_s *bp = (boot_param_s*)boot_param_instance();
    
    //先将原来的程序拷贝到备份空间    
    dest = part_get_inst_name("img2");
    src = part_get_inst_name("img1");
    ret = part_copy_data(src,dest);
    if(0 != ret)
    {
        wind_warn("roll back program failed.");
        return -1;
    }
    
    dest = part_get_inst_name("romrun");
    src = part_get_inst_name("img1");
    run_in_program1 = region_equal(dest,src);
    if(run_in_program1)
    {
        tmp1 = part_get_inst_name("romrun");
        tmp1->status = MEM_NORMAL;
        tmp1->crc = src->crc;
        tmp1->datalen= src->datalen;
    }
    else
    {
        dest = part_get_inst_name("img1");
        src = part_get_inst_name("cache");
        part_copy_data(src,dest);
        decrypt_img_data(src,&bin);
        dest = part_get_inst_name("romrun");
        ret = part_copy_data(&bin,dest);
        if(0 != ret)
        {
            wind_error("flush program to running space failed.");
            return -1;
        }
    }
    boot_param_flush();
    return 0;
}

w_int32_t flush_img_to_ram(part_s *img)
{
    w_int32_t ret;
    part_s bin;
    part_s *dest;
    boot_param_s *bp = (boot_param_s*)boot_param_instance();
    decrypt_img_data(img,&bin);
    //这里需要修改，不能直接使用img，因为会修改程序缓存的起始地址，
    //另外需要重新考虑程序运行在RAM中间时，程序因该怎样存储
    
    wind_notice("begin to copy code to memory...");
    dest = part_get_inst_name("ramrun");
    ret = part_copy_data(&bin,dest);
    if(0 != ret)
    {
        wind_warn("copy img to running space failed.");
        return -1;
    }
    return 0;
}


//先备份原来的程序，再烧录新程序到sys_program1和运行区
w_int32_t flush_img_to_rom(part_s *img)
{
    w_int32_t ret;
    part_s *src,*dest;
    part_s bin;
    img_head_s *head;
    w_bool_t run_in_program1;
    boot_param_s *bp = (boot_param_s*)boot_param_instance();
    wind_notice("begin to flush code to rom space...");
    
    //先将原来的程序拷贝到备份空间    
    src = part_get_inst_name("img1");
    dest = part_get_inst_name("img2");
    
    ret = part_copy_data(src,dest);
    if(0 != ret)
    {
        wind_warn("backup old program failed.");
        return -1;
    }
    
    head = (img_head_s*)img->addr;
    src = part_get_inst_name("img1");
    dest = part_get_inst_name("romrun");
    run_in_program1 = region_equal(src,dest);
    
    //烧录到sys_program1，如果同时也是运行空间，则先解密，在烧录
    if(run_in_program1)
    {
        decrypt_img_data(img,&bin);
        dest = part_get_inst_name("img1");
        ret = part_copy_data(&bin,dest);
        if(0 != ret)
        {
            wind_warn("flush new program failed.");
            return -1;
        }
        dest = part_get_inst_name("romrun");
        dest->status = MEM_NORMAL;
        dest->crc = bin.crc;
        dest->datalen= bin.datalen;
        boot_param_flush();
        return 0;
    }
    else
    {
        dest = part_get_inst_name("img1");
        ret = part_copy_data(img,dest);
        if(0 != ret)
        {
            wind_warn("flush new program failed.");
            return -1;
        }
        decrypt_img_data(img,&bin);
        dest = part_get_inst_name("romrun");
        ret = part_copy_data(&bin,dest);
        if(0 != ret)
        {
            wind_error("write new program failed.");
            return -1;
        }
    }
    boot_param_flush();
    return 0;
}


w_int32_t flush_img_file(w_int16_t type,part_s *img)
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
            wind_error("unknown memory type:%d",type);
            ret = -1;
            break;
    }
    if(0 != ret)
    {
        wind_warn("flush img data failed.");
        (void)boot_param_from_rom();
        return ret;
    }
    if(MEM_TYPE_ROM == type)
        (void)boot_param_flush();
    return ret;
}

w_int32_t download_img_file(w_int16_t type)
{
    w_int32_t ret,len;
    part_s *img;
    boot_param_s *bp = (boot_param_s*)boot_param_instance();

    if(bp->debug_mode)
    {
        wind_notice("device can NOT download in debug mode ,set it to normal mode first");
        return -1;
    }
    img = part_get_inst_name("cache");
    wind_printf("begin to receive file data,please wait.\r\n");
    len = boot_receive_img(img->addr,img->size);
    if(len <= 0)
    {
        wind_error("receive img data failed.");
        return -1;
    }

    img->datalen = (w_uint32_t)len;
    wind_notice("img file lenth:%d",img->datalen);
    ret = check_img_valid(img);
    if(ret != 0)
    {
        wind_error("check img file ERROR");
        return -1; 
    }
    ret = flush_img_file(type,img);
    if(0 != ret)
    {
        wind_warn("flush data to %s failed.",phymem_type(type));
        return -1;
    }
    wind_notice("img flush OK.");
    return 0;
}



w_int32_t clean_program(void)
{
    w_int32_t idx = 0;
    w_uint32_t i,blocknum;
    part_s *code[5];
    boot_param_s *bp = (boot_param_s*)boot_param_instance();
    wind_printf("clearing program ...\r\n");
    code[idx++] = part_get_inst_name("img1");
    code[idx++] = part_get_inst_name("img2");
    code[idx++] = part_get_inst_name("romrun");
    code[idx++] = part_get_inst_name("btpara1");
    code[idx++] = part_get_inst_name("btpara2");
    
    for(i = 0;i < 5;i ++)
    {
        wind_notice("erase base 0x%x,lenth %d.",code[i]->addr,code[i]->datalen);
        blocknum = (code[i]->datalen + BLOCK_SIZE - 1) / BLOCK_SIZE;
        erase_block(code[i]->memtype,code[i]->memidx,code[i]->addr,blocknum);
    }
    wind_printf("clear program OK.\r\n");
    return 0;
}


#ifdef __cplusplus
}
#endif
