/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2016/11/04
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#include "boot_config.h"
#include "boot_check.h"
#include "boot_param.h"
#include "wind_debug.h"
#include "wind_crc32.h"
#include "wind_string.h"
#include "program_mgr.h"
#include "boot_hw_if.h"
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
    dest = boot_part_get(PART_ROMRUN);
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
        tmp1 = boot_part_get(PART_ROMRUN);
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
    tmp2 = boot_part_get(PART_ROMRUN);
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
            tmp2 = boot_part_get(PART_ROMRUN);
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
        tmp2 = boot_part_get(PART_ROMRUN);
        if(0 != repair_rom_space(tmp1,tmp2))
            ret = -1;
    }
    (void)boot_param_flush();
    return ret;
}


w_int32_t check_rom_programs(void)
{
    w_int32_t idx = 0;
    w_int32_t ret = 0;
    w_part_s *code[3];
    w_int32_t save_flag = 0,i;
    w_bool_t is_ok;
    
    code[idx++] = boot_part_get(PART_IMG1);
    code[idx++] = boot_part_get(PART_IMG2);
    code[idx++] = boot_part_get(PART_ROMRUN);
    wind_notice("begin to check programs...");
    for(i = 0;i < sizeof(code)/sizeof(w_part_s*);i ++)
    {
        is_ok = boot_part_check(code[i]);
        if(!is_ok && (MEM_ERROR != code[i]->status))
        {
            save_flag = 1;
            code[i]->status = MEM_ERROR;
        }
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
    wind_notice("check programs OK.");
    return ret;
}

