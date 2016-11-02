/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
  *FileName:  
  *Author:      Zhou Jiangcun
  *Version:     1.0
  *Date:        2016/11/2
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#include "config.h"
#include "boot_check.h"
#include "boot_param.h"
#include "sys_debug.h"

int32_t repair_running_space(boot_param_s *bp)
{
    int32_t ret;
    region_s *src,*dest;
    
    dest = &bp->mem_map.run.flash;
    if(MEM_NORMAL == bp->mem_map.rom.program1_region.status)
        src = &bp->mem_map.rom.program1_region;
    else if(MEM_NORMAL == bp->mem_map.rom.program2_region.status)
        src = &bp->mem_map.rom.program2_region;
    else
        src = NULL;
    
    if(NULL == src)
    {
        sys_warn("can not find an available source for repairing.");
        ret = -1;
    }
    else
    {
        sys_notice("repair program from %s to %s",src->regname,dest->regname);
        ret = repair_rom_space(src,dest);
    }
    return ret;
}



int32_t repair_rom_space(region_s *src,region_s *dest)
{
    int32_t ret;
    
    if(MEM_NORMAL != src->status)
    {
        sys_error("can NOT find available source to repir program1.");
        src = NULL;
        return -1;
    }
    
    sys_notice("repair program from %s to %s",src->regname,dest->regname);
    if(dest->type == src->type)
        ret = copy_region_data(src,dest);
    else if(MEM_TYPE_ROM == src->type || MEM_TYPE_ROM == dest->type)
        ret = write_encrypt_code_to_run(src,dest);
    else
    {
        sys_warn("can not repair space.");
        ret = -1;
    }
    
    if(0 != ret)
    {
        sys_error("repir space %s base 0x%x,lenth %d failed.",
                    memtype_name(dest->type),dest->addr,dest->maxlen);
        return -1;
    }
    return 0;
}

static int32_t repair_program(boot_param_s *bp)
{
    int32_t ret = 0;
    sys_notice("programs has errors,try to repair ...");
    if(MEM_ERROR == bp->mem_map.run.flash.status)
    {
        if(0 != repair_running_space(bp))
            ret = -1;
    }
    if(MEM_ERROR == bp->mem_map.rom.program1_region.status)
    {
        if(0 != repair_rom_space(&bp->mem_map.rom.program2_region,&bp->mem_map.rom.program1_region))
            ret = -1;
    }
    if(MEM_ERROR == bp->mem_map.rom.program2_region.status)
    {
        if(0 != repair_rom_space(&bp->mem_map.rom.program1_region,&bp->mem_map.rom.program2_region))
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
    uint8_t *buff = get_block_buffer();

    copy_region_info(code,&prog);
    prog.regname = code->regname;
    if(prog.status == MEM_NULL)
    {
        sys_notice("region %s type %s base 0x%x lenth %d is empty.",
                    prog.regname,memtype_name(prog.type),prog.addr,prog.lenth);
        return 0;
    }
    if(prog.status != MEM_ERROR)
    {
        sys_debug("check program base 0x%x,lenth %d",prog.addr,prog.lenth);
        blocks = (prog.lenth + BLOCK_SIZE - 1) / BLOCK_SIZE;
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
            
            if(i == blocks - 1)
            {
                len = prog.lenth - i * BLOCK_SIZE;
            }
            else
            {
                len = BLOCK_SIZE;
            }
            cal_crc = calc_crc32(buff,len,cal_crc);
        }
    }
    
    if(MEM_ERROR == prog.status || cal_crc != prog.crc)
    {
        sys_warn("check program CRC in %s base 0x%x,lenth %d failed.",
                    memtype_name(prog.type),prog.addr,prog.lenth);
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
    
    code[idx++] = &bp->mem_map.rom.program1_region;
    code[idx++] = &bp->mem_map.rom.program2_region;
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
                        memtype_name(code[i]->type),code[i]->addr,code[i]->lenth);
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

