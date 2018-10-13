/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2017/04/08
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#include "boot_config.h"
#include "boot_menu.h"
#include "boot_port.h"
#include "wind_debug.h"
#include "boot_param.h"
#include "boot_framework.h"
#include "wind_debug.h"
#include "wind_crc32.h"
#include "boot_share_param.h"
#include "boot_img.h"
#include "boot_part.h"
#include "boot_hw_if.h"
#include "wind_encrypt.h"
#include "wind_string.h"
#ifdef __cplusplus
extern "C" {
#endif


static w_uint32_t LE_TO_BE32(w_uint32_t x) 
{
    return (((x)&0xff)<<24) + (((x>>8)&0xff)<<16) + (((x>>16)&0xff)<<8) + (((x>>24)&0xff));
}

static w_uint32_t endian_test(void)
{
    w_uint8_t test[2] = {0x12,0x34};
    w_uint16_t *data = (w_uint16_t*)test;
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

w_int32_t decrypt_img_data(w_part_s *img,w_part_s *bin)
{
    w_int32_t len;
    img_head_s *head;

    boot_part_copy_info(img,bin);
	head = (img_head_s*)bin->base;
    bin->base += head->head_len;
    bin->size = img->size - head->head_len;
    bin->datalen = img->datalen - head->head_len;
    wind_notice("decrypt img file...");
    wind_debug("decrypt_data base:0x%x,lenth:%d",bin->base,bin->datalen);
	len = wind_decrypt((w_uint8_t *)bin->base,bin->datalen);
    if(len < 0)
    {
        wind_warn("decrypt img file failed.");
        return -1;
    }
    bin->datalen = len;
    
    bin->crc = wind_crc32((w_uint8_t *)bin->base,bin->datalen,0xffffffff);
    feed_watchdog();
    wind_notice("decrypt img file OK.");
    return 0;
}


w_int32_t check_img_valid(w_part_s *img)
{
    w_uint32_t cal_crc,crc;
    img_head_s *head;

    feed_watchdog();
	head = (img_head_s*)img->base;
    if(0 != head_endian_convert(head))
    {
        wind_warn("img file head endian convert ERROR.");
        return -1;
    }
    
    cal_crc = wind_crc32((w_uint8_t*)head,head->head_len - 4,0xffffffff);
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
	crc = wind_crc32((w_uint8_t*)(img->base+head->head_len),head->img_len - head->head_len,0xffffffff);
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



//先备份原来的程序，再烧录新程序到sys_program1和运行区
w_int32_t flush_img_to_rom(w_part_s *img)
{
    w_int32_t ret;
    w_part_s *src,*dest;
    w_part_s bin;
    img_head_s *head;
    w_bool_t run_in_program1;
    wind_notice("begin to flush code to rom space...");
    
    //先将原来的程序拷贝到备份空间    
    src = boot_part_get(PART_IMG1);
    dest = boot_part_get(PART_IMG2);
    
    ret = boot_part_copy_data(src,dest);
    if(0 != ret)
    {
        wind_warn("backup old program failed.");
        return -1;
    }
    
    head = (img_head_s*)img->base;
    src = boot_part_get(PART_IMG1);
    dest = boot_part_get(PART_SYSRUN);
    run_in_program1 = boot_part_equal(src,dest);
    
    //烧录到sys_program1，如果同时也是运行空间，则先解密，在烧录
    if(run_in_program1)
    {
        decrypt_img_data(img,&bin);
        dest = boot_part_get(PART_IMG1);
        ret = boot_part_copy_data(&bin,dest);
        if(0 != ret)
        {
            wind_warn("flush new program failed.");
            return -1;
        }
        dest = boot_part_get(PART_SYSRUN);
        dest->status = MEM_NORMAL;
        dest->crc = bin.crc;
        dest->datalen= bin.datalen;
        boot_param_flush();
        return 0;
    }
    else
    {
        dest = boot_part_get(PART_IMG1);
        ret = boot_part_copy_data(img,dest);
        if(0 != ret)
        {
            wind_warn("flush new program failed.");
            return -1;
        }
        decrypt_img_data(img,&bin);
        dest = boot_part_get(PART_SYSRUN);
        ret = boot_part_copy_data(&bin,dest);
        if(0 != ret)
        {
            wind_error("write new program failed.");
            return -1;
        }
    }
    boot_param_flush();
    return 0;
}



w_err_t download_img_file(w_part_s **part,w_int32_t count)
{
    w_int32_t ret,len,i;
    w_part_s *cache;
    boot_param_s *bp = (boot_param_s*)boot_param_get();

    if(bp->debug_mode)
    {
        wind_notice("device can NOT download in debug mode ,set it to normal mode first");
        return -1;
    }
    cache = boot_part_get(PART_CACHE);
    wind_printf("begin to receive file data,please wait.\r\n");
    len = boot_receive_img(cache);
    if(len <= 0)
    {
        wind_error("receive cache data failed.");
        return -1;
    }

    cache->datalen = (w_uint32_t)len;
    boot_part_calc_crc(cache);
    
    wind_notice("cache file lenth:%d",cache->datalen);

    for(i = 0;i < count;i ++)
    {
        ret = boot_part_copy_data(cache,part[i]);
        if(0 != ret)
        {
            wind_warn("flush data to %s failed.",cache->name);
            return -1;
        }
    }
    boot_param_flush();
    wind_notice("param flush OK.");
    return 0;
}



w_int32_t clean_program(void)
{
    w_int32_t idx = 0;
    w_uint32_t i;
    w_part_s *code[5];
    wind_printf("clearing program ...\r\n");
    code[idx++] = boot_part_get(PART_IMG1);
    code[idx++] = boot_part_get(PART_IMG2);
    code[idx++] = boot_part_get(PART_SYSRUN);
    code[idx++] = boot_part_get(PART_PARAM1);
    code[idx++] = boot_part_get(PART_PARAM2);
    
    for(i = 0;i < 5;i ++)
    {
        if(!code[i])
            continue;
        wind_notice("erase base 0x%x,lenth %d.",code[i]->base,code[i]->datalen);
        boot_part_erase(code[i]);
    }
    wind_printf("clear program OK.\r\n");
    return 0;
}


//-------------------------------------------------------------------------
w_int32_t repair_rom_space(w_part_s *src,w_part_s *dest)
{
    w_int32_t ret;
    
    if(MEM_NORMAL != src->status)
    {
        wind_error("can NOT find available source to repir program1.");
        src = W_NULL;
        return -1;
    }
    
    wind_notice("repair program from \"%s\" to \"%s\"",
                src->name,dest->name);
    ret = boot_part_copy_data(src,dest);
    if(ret != 0)
    {
        wind_error("repir space %s base 0x%x,lenth %d failed.",
                    dest->name,dest->base,dest->size);
        return -1;
    }
    return 0;
}

w_int32_t repair_running_space(void)
{
    w_int32_t ret;
    w_part_s *src,*dest,*tmp,*tmp1;
    w_part_s bin;
    dest = boot_part_get(PART_SYSRUN);
    do
    {
        src = boot_part_get(PART_IMG1);
        if(MEM_NORMAL == src->status)
            break;
        src = boot_part_get(PART_IMG2);
        if(MEM_NORMAL == src->status)
            break;
        src = W_NULL;  
    }while(0);
    
    if(W_NULL == src)
    {
        wind_warn("can not find an available source for repairing.");
        ret = -1;
    }
    else
    {
        //如果不是同一块sys_program1与运行区不是同一块，则数据需要先解密
        wind_notice("repair program from \"%s\" to \"%s\"",src->name,dest->name);
        tmp = boot_part_get(PART_CACHE);
        boot_part_copy_data(src,tmp);
        src = boot_part_get(PART_CACHE);
        tmp = boot_part_get(PART_IMG1);
        tmp1 = boot_part_get(PART_SYSRUN);
        if((tmp->mtype != tmp1->mtype) ||
            wind_strcmp(tmp->media_name,tmp1->media_name) ||
            (tmp->base != tmp1->base))
        {
            decrypt_img_data(src,&bin);
        }
        else
        {
            boot_part_copy_info(src,&bin);
        }
        ret = repair_rom_space(&bin,dest);
    }
    return ret;
}



static w_int32_t repair_program(void)
{
    w_int32_t ret = 0;
    w_part_s *tmp1,*tmp2;
    wind_notice("programs has errors,try to repair ...");
    tmp2 = boot_part_get(PART_SYSRUN);
    if(MEM_ERROR == tmp2->status)
    {
        if(0 != repair_running_space())
            ret = -1;
    }
    tmp1 = boot_part_get(PART_IMG1);
    if(MEM_ERROR == tmp1->status)
    {
        
        if((tmp2->mtype != tmp1->mtype) ||
            wind_strcmp(tmp2->media_name,tmp1->media_name) ||
            (tmp2->base != tmp1->base))
        {
            tmp2 = boot_part_get(PART_IMG2);
            if(0 != repair_rom_space(tmp2,tmp1))
                ret = -1;
        }
        else
        {
            tmp1 = boot_part_get(PART_IMG1);
            tmp2 = boot_part_get(PART_SYSRUN);
            tmp1->base = tmp2->base;
            tmp1->datalen = tmp2->datalen;
            tmp1->crc = tmp2->crc;
            tmp1->status = MEM_NORMAL;
        }
    }
    tmp1 = boot_part_get(PART_IMG2);
    if(MEM_ERROR == tmp1->status)
    {
        tmp1 = boot_part_get(PART_IMG1);
        tmp2 = boot_part_get(PART_SYSRUN);
        if(0 != repair_rom_space(tmp1,tmp2))
            ret = -1;
    }
    (void)boot_param_flush();
    return ret;
}



w_int32_t boot_img_check(void)
{
    w_int32_t idx = 0;
    w_int32_t ret = 0;
    w_part_s *code[3];
    w_int32_t save_flag = 0,i;
    w_bool_t is_ok;
    
    code[idx++] = boot_part_get(PART_IMG1);
    code[idx++] = boot_part_get(PART_IMG2);
    code[idx++] = boot_part_get(PART_SYSRUN);
    for(i = 0;i < sizeof(code)/sizeof(w_part_s*);i ++)
    {
        if(!code[i])
            continue;
        is_ok = boot_part_check(code[i]);
        if(!is_ok)
        {
            save_flag = 1;
            code[i]->status = MEM_ERROR;
        }
        wind_notice("part %s status:%s",code[i]->name,is_ok?"OK":"ERROR");
    }

    if(save_flag)
    {
        wind_error("program space ERROR.");
        (void)boot_param_flush();
        ret = repair_program();
        if(0 != ret)
        {
            wind_error("repairing program failed");
            return -1;
        }
        return ret;
    }
    return ret;
}

#ifdef __cplusplus
}
#endif
