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
#include "program_mgr.h"
#include "boot_hw_if.h"
w_int32_t repair_rom_space(part_s *src,part_s *dest)
{
    w_int32_t ret;
    
    if(MEM_NORMAL != src->status)
    {
        wind_error("can NOT find available source to repir program1.");
        src = NULL;
        return -1;
    }
    
    wind_notice("repair program from \"%s\" to \"%s\"",
                src->name,dest->name);
    ret = part_copy_data(src,dest);
    if(ret != 0)
    {
        wind_error("repir space %s base 0x%x,lenth %d failed.",
                    dest->name,dest->addr,dest->size);
        return -1;
    }
    return 0;
}

w_int32_t repair_running_space(void)
{
    w_int32_t ret;
    part_s *src,*dest,*tmp,*tmp1;
    part_s bin;
    boot_param_s *bp = (boot_param_s *)boot_param_instance();
    dest = part_get_inst_name("romrun");
    do
    {
        src = part_get_inst_name("img1");
        if(MEM_NORMAL == src->status)
            break;
        src = part_get_inst_name("img2");
        if(MEM_NORMAL == src->status)
            break;
        src = NULL;  
    }while(0);
    
    if(NULL == src)
    {
        wind_warn("can not find an available source for repairing.");
        ret = -1;
    }
    else
    {
        //如果不是同一块sys_program1与运行区不是同一块，则数据需要先解密
        wind_notice("repair program from \"%s\" to \"%s\"",src->name,dest->name);
        tmp = part_get_inst_name("cache");
        part_copy_data(src,tmp);
        src = part_get_inst_name("cache");
        tmp = part_get_inst_name("img1");
        tmp1 = part_get_inst_name("romrun");
        if((tmp->memtype != tmp1->memtype) ||
            (tmp->memidx != tmp1->memidx) ||
            (tmp->addr != tmp1->addr))
        {
            decrypt_img_data(src,&bin);
        }
        else
        {
            part_copy_info(src,&bin);
        }
        ret = repair_rom_space(&bin,dest);
    }
    return ret;
}



static w_int32_t repair_program(boot_param_s *bp)
{
    w_int32_t ret = 0;
    part_s *tmp1,*tmp2;
    wind_notice("programs has errors,try to repair ...");
    tmp2 = part_get_inst_name("romrun");
    if(MEM_ERROR == tmp2->status)
    {
        if(0 != repair_running_space())
            ret = -1;
    }
    tmp1 = part_get_inst_name("img1");
    if(MEM_ERROR == tmp1->status)
    {
        
        if((tmp2->memtype != tmp1->memtype) ||
            (tmp2->memidx != tmp1->memidx) ||
            (tmp2->addr != tmp1->addr))
        {
            tmp2 = part_get_inst_name("img2");
            if(0 != repair_rom_space(tmp2,tmp1))
                ret = -1;
        }
        else
        {
            tmp1 = part_get_inst_name("img1");
            tmp2 = part_get_inst_name("romrun");
            tmp1->addr = tmp2->addr;
            tmp1->datalen = tmp2->datalen;
            tmp1->crc = tmp2->crc;
            tmp1->status = MEM_NORMAL;
        }
    }
    tmp1 = part_get_inst_name("img2");
    if(MEM_ERROR == tmp1->status)
    {
        tmp1 = part_get_inst_name("img1");
        tmp2 = part_get_inst_name("romrun");
        if(0 != repair_rom_space(tmp1,tmp2))
            ret = -1;
    }
    (void)boot_param_flush();
    return ret;
}



//检查程序块的CRC的值是否正确，正确返回1，错误返回0
w_int32_t check_rom_program(part_s *code)
{
    w_uint32_t cal_crc = 0;
    w_int32_t i,blocks,len;
    w_uint32_t base;
    part_s prog;
    img_head_s *head;
    w_uint8_t *buff = get_block_buffer();

    part_copy_info(code,&prog);
    if(prog.status == MEM_NULL)
    {
        wind_notice("part \"%s\" type %s base 0x%x lenth %d is empty.",
                    prog.name,phymem_type(prog.memtype),prog.addr,prog.datalen);
        return 0;
    }
    
    if(prog.status != MEM_ERROR)
    {
        wind_debug("check program base 0x%x,lenth %d",prog.addr,prog.datalen);
        blocks = (prog.datalen + BLOCK_SIZE - 1) / BLOCK_SIZE;
        for(i = 0;i < blocks;i ++)
        {
            base = prog.addr + i * BLOCK_SIZE;
            len = read_block(prog.memtype,prog.memidx,base,buff,1);
            if(len <= 0)
            {
                wind_warn("read %s block base 0x%x,lenth %d failed.",
                            phymem_type(prog.memtype),base,BLOCK_SIZE);
                return -1;
            }
            len = (i == blocks - 1)?prog.datalen - i * BLOCK_SIZE:BLOCK_SIZE;
            if(i == 0)
            {
                head = (img_head_s*)buff;
				if(head->head_len > BLOCK_SIZE)
                {
                    cal_crc = wind_crc32(0xffffffff,buff,len);
                }            
                else if(head->head_crc == wind_crc32(0xffffffff,buff,head->head_len-4))
                    cal_crc = wind_crc32(0xffffffff,&buff[head->head_len],BLOCK_SIZE-head->head_len);
                else
                    cal_crc = wind_crc32(cal_crc,buff,len);
            }
            else
                cal_crc = wind_crc32(cal_crc,buff,len);
        }
    }
    
    if(MEM_ERROR == prog.status || cal_crc != prog.crc)
    {
        wind_warn("check program CRC in %s base 0x%x,lenth %d failed.",
                    phymem_type(prog.memtype),prog.addr,prog.datalen);
        wind_debug("cal_crc:0x%x,crc:0x%x",cal_crc,prog.crc);
        code->status = MEM_ERROR;
        return -1;
    }
    return 0;
}

w_int32_t check_rom_programs(void)
{
    w_int32_t idx = 0;
    w_int32_t ret = 0;
    part_s *code[3];
    w_int32_t save_flag = 0,i;
    boot_param_s *bp = (boot_param_s *)boot_param_instance();
    
    code[idx++] = part_get_inst_name("img1");;
    code[idx++] = part_get_inst_name("img2");
    code[idx++] = part_get_inst_name("romrun");
    wind_notice("begin to check programs...");
    for(i = 0;i < sizeof(code)/sizeof(part_s*);i ++)
    {
        if(MEM_ERROR != code[i]->status)
        {
            if(check_rom_program(code[i]))
            {
                save_flag = 1;
            }
        }
        else
        {
            
            wind_warn("check program CRC in %s base 0x%x,lenth %d failed.",
                        phymem_type(code[i]->memtype),code[i]->addr,code[i]->datalen);
            ret= 1;
        }
    }

    if(save_flag)
    {
        wind_error("program space ERROR.");
        (void)boot_param_flush();
        ret = repair_program(bp);
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

