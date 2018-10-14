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
#include "wind_conv.h"
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



static void boot_img_print_head(img_head_s *head)
{
    static char *encty_type[4] = 
    {
        "no encrypt",
        "RC4"
        "AES",
        "DES",
    };
    wind_printf("img head info:\r\n");
    wind_printf("board name     : %s\r\n",(char*)head->board_name);
    wind_printf("cpu arch       : %s\r\n",(char*)head->arch_name);
    wind_printf("CPU model      : %s\r\n",(char*)head->cpu_name);
    wind_printf("img file name  : %s\r\n",(char*)head->img_name);
    wind_printf("img file lenth : %d\r\n",head->img_len);
    wind_printf("soft version   : %s\r\n",head->bin_ver);
    if(head->encrypt_type < 4);
        wind_printf("encrypt type   : %s\r\n",encty_type[head->encrypt_type]);
    wind_printf("\r\n");
}

static w_err_t boot_img_get_head(w_part_s *part)
{
    w_int32_t len;
    w_uint32_t crc;
    w_int32_t index = 0;
    img_head_s *head = &img_head;
    w_uint8_t *buff = get_common_buffer();
    len = boot_part_read(part,buff,COMMBUF_SIZE);
    WIND_ASSERT_RETURN(len > 0,W_ERR_FAIL);
    head->magic = 0;
    wind_to_uint32(&buff[index],&head->magic);
    WIND_ASSERT_RETURN(head->magic == IMG_MAGIC,W_ERR_FAIL);
    index += 4;
    wind_to_uint32(&buff[index],&head->img_len);
    index += 4;
    wind_to_uint32(&buff[index],&head->head_len);
    index += 4;
    wind_to_uint32(&buff[index],&head->head_ver);
    index += 4;
    wind_to_uint32(&buff[index],&head->bin_ver);
    index += 4;
    wind_to_uint32(&buff[index],&head->bin_crc);
    index += 4;
    wind_to_uint32(&buff[index],&head->encrypt_type);
    index += 4;
    wind_memset(head->img_name,0,sizeof(head->img_name));
    wind_strcpy(head->img_name,(const char*)&buff[index]);
    index += sizeof(head->img_name);

    wind_memset(head->board_name,0,sizeof(head->board_name));
    wind_strcpy(head->board_name,(const char*)&buff[index]);
    index += sizeof(head->board_name);
    
    wind_memset(head->arch_name,0,sizeof(head->arch_name));
    wind_strcpy(head->arch_name,(const char*)&buff[index]);
    index += sizeof(head->arch_name);

    wind_memset(head->cpu_name,0,sizeof(head->cpu_name));
    wind_strcpy(head->cpu_name,(const char*)&buff[index]);
    index += sizeof(head->cpu_name);

    wind_to_uint32(&buff[index],&head->head_crc);
    crc = wind_crc32(buff,index,0xffffffff);
    WIND_ASSERT_RETURN(crc == head->head_crc,W_ERR_INVALID);
    boot_img_print_head(head);
    return W_ERR_OK;

}

w_part_s * boot_img_get_old_part(void)
{
    w_part_s *part[2];
    part[0] = boot_part_get(PART_IMG1);
    part[1] = boot_part_get(PART_IMG2);
    if(!part[0] && !part[1])
        return (w_part_s *)W_NULL;
    if(!(part[0] && part[1]))
    {
        if(part[0] && (part[0]->status == MEM_NORMAL))
            return part[0];
        if(part[1] && (part[1]->status == MEM_NORMAL))
            return part[1];
        return (w_part_s *)W_NULL;
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
        return (w_part_s *)W_NULL;
    if(!(part[0] && part[1]))
    {
        if(part[0] && (part[0]->status == MEM_NORMAL))
            return part[0];
        if(part[1] && (part[1]->status == MEM_NORMAL))
            return part[1];
        return (w_part_s *)W_NULL;
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

w_err_t boot_img_decrypt(w_part_s *img)
{
    w_int32_t offset;
    w_int32_t len;
    img_head_s *head = &img_head;
    WIND_ASSERT_RETURN(head->magic == IMG_MAGIC,W_ERR_FAIL);
    offset = head->img_len;
    return 0;
}


w_err_t check_img_valid(w_part_s *img)
{
    w_uint32_t cal_crc,crc;
    img_head_s *head;

    feed_watchdog();

    
    cal_crc = wind_crc32((w_uint8_t*)head,head->head_len - 4,0xffffffff);
    crc = head->head_crc;
    
    wind_debug("img file head crc:0x%x,calc_crc:0x%x.",crc,cal_crc);
    if(cal_crc != crc)
    {
        wind_warn("img file head crc ERROR.");
        return W_ERR_FAIL ;
    }
    boot_img_print_head(head);
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


w_err_t boot_img_flush_cache_to_part(w_part_s **part,w_int32_t count)
{
    w_int32_t i;
    w_err_t err;
    w_part_s *cache;
    cache = boot_part_get(PART_CACHE);
    for(i = 0;i < count;i ++)
    {
        err = boot_part_copy_data(cache,part[i]);
        if(0 != err)
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



w_err_t boot_img_clear_all(void)
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
    return W_ERR_OK;
}


//-------------------------------------------------------------------------
static w_err_t repair_program(void)
{
    w_err_t err;
    w_part_s *src,*dest;
    dest = boot_part_get(PART_SYSRUN);
    if(MEM_ERROR != dest->status)
        return W_ERR_OK;
    src = boot_img_get_new_normal_img();
    WIND_ASSERT_RETURN(src != W_NULL,W_ERR_FAIL);
    wind_notice("repair part:%s from %s",dest->name,src->name);
    err = boot_part_copy_data(src,dest);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    
    (void)boot_param_flush();
    return err;
}

w_err_t boot_img_check(void)
{
    w_int32_t idx = 0;
    w_err_t err = 0;
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
        err = repair_program();
        
        if(0 != err)
        {
            wind_error("repairing program failed");
            return W_ERR_FAIL;
        }
        return err;
    }
    return err;
}

#ifdef __cplusplus
}
#endif
