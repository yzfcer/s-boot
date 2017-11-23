/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
  *FileName:  
  *Author:      Zhou Jiangcun
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
#include "sys_debug.h"
#include "crc.h"
#include "program_mgr.h"

int32_t repair_rom_space(region_s *src,region_s *dest)
{
    int32_t ret;
    
    if(MEM_NORMAL != src->status)
    {
        sys_error("can NOT find available source to repir program1.");
        src = NULL;
        return -1;
    }
    
    sys_notice("repair program from \"%s\" to \"%s\"",
                src->regname,dest->regname);
    ret = copy_region_data(src,dest);
    if(ret != 0)
    {
        sys_error("repir space %s base 0x%x,lenth %d failed.",
                    memtype_name(dest->type),dest->addr,dest->size);
        return -1;
    }
    return 0;
}

int32_t repair_running_space(void)
{
    int32_t ret;
    region_s *src,*dest;
    region_s bin;
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    dest = &bp->mem_map.run.flash;
    if(MEM_NORMAL == bp->mem_map.rom.sys_program1.status)
        src = &bp->mem_map.rom.sys_program1;
    else if(MEM_NORMAL == bp->mem_map.rom.sys_program2.status)
        src = &bp->mem_map.rom.sys_program2;
    else
        src = NULL;
    
    if(NULL == src)
    {
        sys_warn("can not find an available source for repairing.");
        ret = -1;
    }
    else
    {
        //如果不是同一块sys_program1与运行区不是同一块，则数据需要先解密
        sys_notice("repair program from \"%s\" to \"%s\"",src->regname,dest->regname);
        copy_region_data(src,&bp->mem_map.ram.load_buffer);
        src = &bp->mem_map.ram.load_buffer;
        if((bp->mem_map.rom.sys_program1.type != bp->mem_map.run.flash.type) ||
            (bp->mem_map.rom.sys_program1.index != bp->mem_map.run.flash.index) ||
            (bp->mem_map.rom.sys_program1.addr != bp->mem_map.run.flash.addr))
        {
            decrypt_img_data(src,&bin);
        }
        else
        {
            copy_region_info(src,&bin);
        }
        ret = repair_rom_space(&bin,dest);
    }
    return ret;
}



static int32_t repair_program(boot_param_s *bp)
{
    int32_t ret = 0;
    sys_notice("programs has errors,try to repair ...");
    if(MEM_ERROR == bp->mem_map.run.flash.status)
    {
        if(0 != repair_running_space())
            ret = -1;
    }
    if(MEM_ERROR == bp->mem_map.rom.sys_program1.status)
    {
        
        if((bp->mem_map.rom.sys_program1.type != bp->mem_map.run.flash.type) ||
            (bp->mem_map.rom.sys_program1.index != bp->mem_map.run.flash.index) ||
            (bp->mem_map.rom.sys_program1.addr != bp->mem_map.run.flash.addr))
        {
            if(0 != repair_rom_space(&bp->mem_map.rom.sys_program2,&bp->mem_map.rom.sys_program1))
                ret = -1;
        }
        else
        {
            bp->mem_map.rom.sys_program1.addr = bp->mem_map.run.flash.addr;
            bp->mem_map.rom.sys_program1.datalen = bp->mem_map.run.flash.datalen;
            bp->mem_map.rom.sys_program1.crc = bp->mem_map.run.flash.crc;
            bp->mem_map.rom.sys_program1.status = MEM_NORMAL;
        }
    }
    if(MEM_ERROR == bp->mem_map.rom.sys_program2.status)
    {
        if(0 != repair_rom_space(&bp->mem_map.rom.sys_program1,&bp->mem_map.rom.sys_program2))
            ret = -1;
    }
    (void)param_flush();
    return ret;
}



//检查程序块的CRC的值是否正确，正确返回1，错误返回0
int32_t check_rom_program(region_s *code)
{
    uint32_t cal_crc = 0;
    int32_t i,blocks,len;
    uint32_t base;
    region_s prog;
    img_head_s *head;
    uint8_t *buff = get_block_buffer();

    copy_region_info(code,&prog);
    if(prog.status == MEM_NULL)
    {
        sys_notice("region \"%s\" type %s base 0x%x lenth %d is empty.",
                    prog.regname,memtype_name(prog.type),prog.addr,prog.datalen);
        return 0;
    }
    
    if(prog.status != MEM_ERROR)
    {
        sys_debug("check program base 0x%x,lenth %d",prog.addr,prog.datalen);
        blocks = (prog.datalen + BLOCK_SIZE - 1) / BLOCK_SIZE;
        for(i = 0;i < blocks;i ++)
        {
            base = prog.addr + i * BLOCK_SIZE;
            len = read_block(prog.type,prog.index,base,buff,1);
            if(len <= 0)
            {
                sys_warn("read %s block base 0x%x,lenth %d failed.",
                            memtype_name(prog.type),base,BLOCK_SIZE);
                return -1;
            }
            len = (i == blocks - 1)?prog.datalen - i * BLOCK_SIZE:BLOCK_SIZE;
            if(i == 0)
            {
                head = (img_head_s*)buff;
				if(head->head_len > BLOCK_SIZE)
                {
                    cal_crc = calc_crc32(buff,len,0xffffffff);
                }            
                else if(head->head_crc == calc_crc32(buff,head->head_len-4,0xffffffff))
                    cal_crc = calc_crc32(&buff[head->head_len],BLOCK_SIZE-head->head_len,0xffffffff);
                else
                    cal_crc = calc_crc32(buff,len,cal_crc);
            }
            else
                cal_crc = calc_crc32(buff,len,cal_crc);
        }
    }
    
    if(MEM_ERROR == prog.status || cal_crc != prog.crc)
    {
        sys_warn("check program CRC in %s base 0x%x,lenth %d failed.",
                    memtype_name(prog.type),prog.addr,prog.datalen);
        sys_debug("cal_crc:0x%x,crc:0x%x",cal_crc,prog.crc);
        code->status = MEM_ERROR;
        return -1;
    }
    return 0;
}

int32_t check_rom_programs(void)
{
    int idx = 0;
    int32_t ret = 0;
    region_s *code[3];
    int32_t save_flag = 0,i;
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    
    code[idx++] = &bp->mem_map.rom.sys_program1;
    code[idx++] = &bp->mem_map.rom.sys_program2;
    code[idx++] = &bp->mem_map.run.flash;
    sys_notice("begin to check programs...");
    for(i = 0;i < sizeof(code)/sizeof(region_s*);i ++)
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
            
            sys_warn("check program CRC in %s base 0x%x,lenth %d failed.",
                        memtype_name(code[i]->type),code[i]->addr,code[i]->datalen);
            ret= 1;
        }
    }

    if(save_flag)
    {
        sys_error("program space ERROR.");
        (void)param_flush();
        ret = repair_program(bp);
        if(0 != ret)
        {
            sys_error("repairing program failed");
            return -1;
        }
        return ret;
    }
    sys_notice("check programs OK.");
    return ret;
}

