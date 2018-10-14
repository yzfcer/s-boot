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
img_head_s img_head;

static w_uint32_t LE_TO_BE32(w_uint32_t x) 
{
    return (((x)&0xff)<<24) + (((x>>8)&0xff)<<16) + (((x>>16)&0xff)<<8) + (((x>>24)&0xff));
}

static w_uint32_t endian_test(void)
{
    w_uint8_t test[2] = {0x12,0x34};
    w_uint16_t *data = (w_uint16_t*)test;
}



static void print_img_head(img_head_s *head)
{
    static char *encty_type[4] = 
    {
        "no encrypt",
        "AES",
        "DES",
        "RC4"
    };
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

w_int32_t boot_img_decrypt(w_part_s *img,w_part_s *bin)
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

w_part_s * boot_img_get_old_part(void)
{
    w_part_s *part[2];
    part[0] = boot_part_get(PART_IMG1);
    part[1] = boot_part_get(PART_IMG2);
    if(!part[0] && !part[1])
        return W_NULL;
    if(!(part[0] && part[1]))
    {
        if(part[0] && (part[0]->status == MEM_NORMAL))
            return part[0];
        if(part[1] && (part[1]->status == MEM_NORMAL))
            return part[1];
        return W_NULL;
    }
    if((part[0]->status != MEM_NORMAL) && (part[1]->status == MEM_NORMAL))
    {
        part[0] = part[1] + 1;
        return part[0];
        
    }
    if((part[1]->status != MEM_NORMAL) && (part[0]->status == MEM_NORMAL))
    {
        part[1] = part[0] + 1;
        return part[1];
    }
    if(part[0]->time_mark <= part[1]->time_mark)
    {
        part[0]->time_mark = part[1]->time_mark + 1;
        return part[0];
    }
    else
    {
        part[1]->time_mark = part[0]->time_mark + 1;
        return part[1];
    }
}

w_part_s *boot_img_get_new_normal_img(void)
{
    w_part_s *part[2];
    part[0] = boot_part_get(PART_IMG1);
    part[1] = boot_part_get(PART_IMG2);
    if(!part[0] && !part[1])
        return W_NULL;
    if(!(part[0] && part[1]))
    {
        if(part[0] && (part[0]->status == MEM_NORMAL))
            return part[0];
        if(part[1] && (part[1]->status == MEM_NORMAL))
            return part[1];
        return W_NULL;
    }
    if((part[0]->status != MEM_NORMAL) && (part[1]->status == MEM_NORMAL))
        return part[1];
    if((part[1]->status != MEM_NORMAL) && (part[0]->status == MEM_NORMAL))
        return part[0];
    if(part[0]->time_mark <= part[1]->time_mark)
    {
        part[0]->time_mark = part[1]->time_mark + 1;
        return part[0];
    }
    else
    {
        part[1]->time_mark = part[0]->time_mark + 1;
        return part[1];
    }
   
}

w_err_t boot_img_flush_cache_to_part(w_part_s **part,w_int32_t count)
{
    w_int32_t i,ret;
    w_part_s *cache;
    cache = boot_part_get(PART_CACHE);
    for(i = 0;i < count;i ++)
    {
        ret = boot_part_copy_data(cache,part[i]);
        if(0 != ret)
        {
            wind_warn("flush data to %s failed.",cache->name);
            return W_ERR_FAIL;
        }
    }
    boot_param_flush();
    wind_notice("param flush OK.");
    return W_ERR_OK;
}

w_err_t boot_img_download(void)
{
    w_int32_t len;
    w_part_s *cache;
    boot_param_s *bp = (boot_param_s*)boot_param_get();

    if(bp->debug_mode)
    {
        wind_notice("device can NOT download in debug mode ,set it to normal mode first");
        return W_ERR_FAIL;
    }
    cache = boot_part_get(PART_CACHE);
    wind_notice("receive file data,please wait.\r\n");
    len = boot_receive_img(cache);
    if(len <= 0)
    {
        wind_error("receive cache data failed.");
        return W_ERR_FAIL;
    }
    cache->datalen = (w_uint32_t)len;
    boot_part_calc_crc(cache);
    wind_notice("cache file lenth:%d",cache->datalen);
    return W_ERR_OK;
}

w_err_t boot_img_flush(void)
{
    w_err_t err;
    w_part_s *part[2],*tmp;
    w_int32_t count;
    part[0] = boot_part_get(PART_SYSRUN);
    WIND_ASSERT_RETURN(part[0] != W_NULL,W_ERR_FAIL);
    part[1] = boot_img_get_old_part();
    if(part[1] != W_NULL)
    {
        tmp = part[0];
        part[0] = part[1];
        part[1] = tmp;
        count = 2;
    }
    else
    {
        count = 1;
    }

    err = boot_img_download();
    return boot_img_flush_cache_to_part(part,count);
}



w_int32_t boot_img_clear_all(void)
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
w_int32_t repair_part_space(w_part_s *src,w_part_s *dest)
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



static w_err_t repair_program(void)
{
    w_err_t err;
    w_part_s *src,*dest;
    dest = boot_part_get(PART_SYSRUN);
    if(MEM_ERROR != dest->status)
        return W_ERR_OK;
    src = boot_img_get_new_normal_img();
    WIND_ASSERT_RETURN(src != W_NULL,W_ERR_FAIL);
    err = boot_part_copy_data(src,dest);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    
    (void)boot_param_flush();
    return err;
}



w_int32_t boot_img_check(void)
{
    w_int32_t idx = 0;
    w_int32_t ret = 0;
    w_part_s *code[3];
    w_int32_t error_flag = 0,i;
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
            error_flag = 1;
            code[i]->status = MEM_ERROR;
        }
        wind_notice("part %s status:%s",code[i]->name,is_ok?"OK":"ERROR");
    }

    if(error_flag)
    {
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
