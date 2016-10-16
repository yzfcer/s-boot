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
#include "menu_list.h"
#include "port.h"
#include "boot_debug.h"
#include "boot_param.h"
#include "boot_framework.h"
#include "boot_debug.h"
#include "crc.h"
#include "share_param.h"
#include "program_mgr.h"
#include "mem_map.h"
#include "boot_hw_if.h"
#include "mem_driver.h"
#define LE_TO_BE32(x) (((x)&0xff)<<24) + (((x>>8)&0xff)<<16) + (((x>>16)&0xff)<<8) + (((x>>24)&0xff))
#ifdef __cplusplus
extern "C" {
#endif

//通用缓存器，一般在拷贝数据时做缓存，或者接收命令字符
uint8_t commbuffer[BLOCK_SIZE];

static void print32_t_copy_percents(int32_t numerator, int32_t denominator,int32_t del)
{
    if(del)
        boot_printf("%c%c%c%c",8,8,8,8);
    boot_printf("%3d%%",numerator*100/denominator);
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
    boot_printf("img head info:\r\n");
	//boot_printf("board index:%d\r\n",(uint32_t)(&head->board_name)-(uint32_t)(&head->magic));
    boot_printf("board name     : %s\r\n",(char*)head->board_name);
    boot_printf("cpu arch       : %s\r\n",(char*)head->arch_name);
    boot_printf("CPU model      : %s\r\n",(char*)head->cpu_name);
    boot_printf("img file name  : %s\r\n",(char*)head->img_name);
    boot_printf("img file lenth : %d\r\n",head->img_len);
    boot_printf("soft version   : %s\r\n",head->bin_ver);
    if(head->encry_type < 4);
        boot_printf("encrypt type   : %s\r\n",encty_type[head->encry_type]);
    boot_printf("\r\n");
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


uint32_t convert_byte_to_uint32(uint8_t *str,int32_t index)
{
    uint32_t i,tmp,va = 0;
    for(i = 0;i < 4;i ++)
    {
        tmp = str[index+i];
        va += (tmp << (i*8));
    }
    return va;
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
        boot_notice("dest board name:%s",BOARD_NAME);
        return B_FALSE;
    }
    if(0 != memory_compare(head->arch_name,ARCH_NAME,string_len(ARCH_NAME)))
    {
        boot_notice("dest arch name:%s",ARCH_NAME);
        return B_FALSE;
    }
    if(0 != memory_compare(head->cpu_name,CPU_NAME,string_len(CPU_NAME)))
    {
        boot_notice("dest cpu model:%s",CPU_NAME);
        return B_FALSE;
    }
    return B_TRUE;    
}

int32_t check_and_decrypt_img(region_s *img)
{
    int32_t len;
    uint32_t cal_crc,crc;
    img_head_s *head;
    //对接受的数据做CRC校验
    feed_watchdog();
	head = (img_head_s*)img->addr;
    if(0 != head_endian_convert(head))
    {
        boot_warn("img file head endian convert ERROR.");
        return -1;
    }
    
    cal_crc = calc_crc32((uint8_t*)head,head->head_len - 4,0xffffffff);
    crc = head->head_crc;
    
    boot_debug("img file head crc:0x%x,calc_crc:0x%x.",crc,cal_crc);
    if(cal_crc != crc)
    {
        boot_warn("img file head crc ERROR.");
        return -1;
    }
    print_img_head(head);
    if(!check_hardware_matched(head))
    {
        boot_warn("hardware is NOT matched.");
        return -1;
    }
    
    feed_watchdog();
	crc = calc_crc32((uint8_t*)(img->addr+head->head_len),head->img_len - head->head_len,0xffffffff);
    cal_crc = head->bin_crc;
    
    boot_debug("bin file crc:0x%x,calc_crc:0x%x.",crc,cal_crc);
    if(cal_crc != crc)
    {
        boot_warn("bin file crc ERROR.");
        return -1;
    }
    
    feed_watchdog();
    boot_notice("img file verify OK.");
    
    //解密
    boot_notice("img file decrypt");
    boot_debug("decrypt_data base:0x%x,lenth:%d",img->addr,img->lenth - 4);
    len = decrypt_data((uint8_t *)img->addr,img->lenth - 4);
    if(len < 0)
    {
        boot_warn("img file Decrypt ERROR.");
        return -1;
    }
    img->lenth = (uint32_t)len;
    feed_watchdog();
    boot_notice("img file decrypt OK.");
    return 0;
}


int32_t encrypt_code_calc_crc(region_s *code_reg)
{
    int32_t len;
    

    boot_debug("decrypt_data base:0x%x,lenth:%d",code_reg->addr,code_reg->lenth - 4);
    len = encrypt_data((uint8_t *)code_reg->addr,code_reg->lenth -4);
    if(len < 0)
    {
        boot_warn("img file encrypt ERROR.");
        return -1;
    }
    
    boot_notice("img file encrypt OK.");
    
    code_reg->crc = calc_crc32((char *)code_reg->addr,code_reg->lenth,0);
    convert_uint32_to_byte((uint8_t *)code_reg->addr,code_reg->lenth - 4,code_reg->crc);
    
    boot_debug("new file CRC:0x%x.",code_reg->crc);
    return 0;
}




int32_t copy_region_data(region_s *src,region_s *dest)
{
    int32_t i,j,len,blocks,times;
    uint32_t addr;

    if(0 >= src->lenth)
        return 0;
    if(dest->maxlen < src->lenth)
    {
        boot_warn("space is NOT enough.");
        return -1;
    }
    boot_notice("copy data from %s to %s lenth %d.",
                src->regname,dest->regname,src->lenth);
    boot_debug("source type %s,addr 0x%x,lenth %d dest type,%s,addr 0x%x,lenth %d.",
                memtype_name(src->type),src->addr,src->lenth,
                memtype_name(dest->type),dest->addr,dest->maxlen);
    
    blocks = (src->lenth + BLOCK_SIZE - 1) / BLOCK_SIZE;
    boot_printf("complete:");
    print32_t_copy_percents(0,1,0);
    for(i = 0;i < blocks;i ++)
    {    
        for(times = 0;times < 3;times ++)
        {
            addr = src->addr + i * sizeof(commbuffer);
            if(i >= blocks - 1)
            {
                for(j = 0;j < sizeof(commbuffer);j ++)
                    commbuffer[j] = 0;
            }
            len = read_block(src->type,src->index,addr,commbuffer,sizeof(commbuffer)/BLOCK_SIZE);
            if(len > 0)
                break;
        }
        if(times >= 3)
        {
            boot_warn("read block 0x%x,lenth %d failed.",addr,sizeof(commbuffer));
            dest->status = MEM_ERROR;
            return -1;
        }

        for(times = 0;times < 3;times ++)
        {
            addr = dest->addr + i * sizeof(commbuffer);
            len = write_block(dest->type,dest->index,addr,commbuffer,sizeof(commbuffer)/BLOCK_SIZE);
            if(len > 0)
                break;
        }
        if(times >= 3)
        {
            boot_warn("read block 0x%x,lenth %d failed.",addr,sizeof(commbuffer));
            dest->status = MEM_ERROR;
            return -1;
        }
        print32_t_copy_percents(i,blocks,1);
        feed_watchdog();
    }
    print32_t_copy_percents(i,blocks,1);
    boot_printf("\r\n");

    dest->lenth = src->lenth;
    dest->crc = src->crc;
    dest->status = MEM_NORMAL;

    boot_debug("copy data OK."); 
    return 0;    
}


int32_t flush_code_to_ram(region_s *code_region)
{
    int32_t ret;
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    boot_notice("begin to copy code to memory...");
    ret = copy_region_data(code_region,&bp->mem_map.run.flash);
    if(0 != ret)
    {
        boot_warn("copy img to running space failed.");
        return -1;
    }
    return 0;
}


//先备份原来的程序，再烧录新程序到运行区
int32_t flush_code_to_iflash(region_s *bin)
{
    int32_t ret;
    region_s *src;
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    boot_notice("begin to flush code to rom space...");
    
    //先将原来的程序拷贝到备份空间    
    src = &bp->mem_map.rom.program1_region;
    ret = copy_region_data(src,&bp->mem_map.rom.program2_region);
    if(0 != ret)
    {
        boot_warn("backup old program failed.");
        return -1;
    }

    //将新程序烧到运行空间
    ret = copy_region_data(bin,&bp->mem_map.rom.program1_region);
    if(0 != ret)
    {
        boot_error("write new program failed.");
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
            ret = flush_code_to_iflash(img);
            break;
        default:
            boot_error("unknown memory type:%d",type);
            ret = -1;
            break;
    }
    if(0 != ret)
    {
        boot_warn("flush img data failed.");
        (void)get_boot_params_from_ROM();
        return ret;
    }
    if(MEM_TYPE_ROM == type)
        (void)write_param();
    return ret;
}

int32_t download_img_file(memtype_e type)
{
    int32_t ret,len;
    region_s *img;
    boot_param_s *bp = (boot_param_s*)get_boot_params();

    if(bp->debug_mode)
    {
        boot_notice("device can NOT download in debug mode ,set it to normal mode first");
        return -1;
    }
    img = &bp->mem_map.ram.probuf_region;
    boot_printf("begin to receive file data,please wait.\r\n");
    len = boot_receive_img(img->addr,img->maxlen);
    if(len <= 0)
    {
        boot_error("receive img data failed.");
        return -1;
    }

    img->lenth = (uint32_t)len;
    ret = check_and_decrypt_img(img);
    if(ret != 0)
    {
        boot_error("check img file ERROR");
        return -1; 
    }
    ret = flush_code_data(type,img);
    if(0 != ret)
    {
        boot_warn("flush data to %s failed.",memtype_name(type));
        return -1;
    }
    boot_notice("img flush OK.");
    return 0;
}

void clean_program(void)
{
    int idx = 0;
    uint32_t i,blocknum;
    region_s *code[5];
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    boot_printf("clearing program ...\r\n");
    code[idx++] = &bp->mem_map.rom.program1_region;
    code[idx++] = &bp->mem_map.rom.program2_region;
    code[idx++] = &bp->mem_map.run.flash;
    code[idx++] = &bp->mem_map.rom.param1_region;
    code[idx++] = &bp->mem_map.rom.param2_region;
    
    for(i = 0;i < 5;i ++)
    {   
        boot_notice("erase base 0x%x,lenth %d.",code[i]->addr,code[i]->lenth);
        blocknum = (code[i]->lenth + BLOCK_SIZE - 1) / BLOCK_SIZE;
        erase_block(code[i]->type,code[i]->index,code[i]->addr,blocknum);
    }
    boot_printf("clear program OK.\r\n");
}

int32_t write_encrypt_code_to_run(region_s *src,region_s *run)
{
    int32_t ret;
    region_s img;
    boot_param_s *bp = (boot_param_s*)get_boot_params();

    ret = copy_region_data(src,&bp->mem_map.ram.probuf_region);
    if(0 != ret)
    {
        boot_warn("copy data error.");
        return -1;
    }
    
    copy_region_info(&bp->mem_map.ram.probuf_region,&img);
    img.lenth = src->lenth;

    ret = check_and_decrypt_img(&img);
    if(0 != ret)
    {
        boot_error("check img file ERROR.");
        return -1;
    }
    
    ret = copy_region_data(&img,run);
    if(0 != ret)
    {
        boot_warn("flush data to running space error.");
        return -1;
    }
    return 0;
    
}
int32_t change_boot_app(int32_t index)
{
    int32_t ret;
    region_s src;
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    boot_notice("begin to change boot App:%d.",index);
    switch(index)
    {
        case APP_IDX_PRO1:
            copy_region_info(&bp->mem_map.rom.program2_region,&src);
            break;
        case APP_IDX_PROBAK:
            copy_region_info(&bp->mem_map.rom.program2_region,&src);
            break;
        default:
            boot_warn("undefined index:%d.",index);
            return -1;
    }
    
    if(src.addr == bp->mem_map.run.flash.addr)
    {
        boot_notice("need NOT to write program.");
        return 0;        
    }
    
    if(src.lenth <= 0)
    {
        boot_warn("program is NOT existing.");
        return -1;
    }
    
    if(MEM_TYPE_ROM == src.type)
    {
        ret = copy_region_data(&src,&bp->mem_map.run.flash);
        if(MEM_TYPE_ROM == bp->mem_map.rom.program1_region.type)
        {
            if(0 == ret)
            {
                bp->mem_map.rom.program1_region.lenth = src.lenth;
                bp->mem_map.rom.program1_region.crc = src.crc;
                bp->mem_map.rom.program1_region.status = MEM_NORMAL;
            }
            else
            {
                bp->mem_map.rom.program1_region.status = MEM_ERROR;
            }
        }
    }
    else if(MEM_TYPE_ROM == src.type)
    {
        ret = write_encrypt_code_to_run(&src,&bp->mem_map.run.flash);
    }
    else
    {
        boot_warn("invalid memory type:%d.",src.type);
        ret = -1;
    }
    
    if(0 != ret)
    {
        boot_error("change boot program failed.");
    }
    (void)write_param();
    return ret;    
}



//检查程序块的CRC的值是否正确，正确返回1，错误返回0
int32_t check_rom_program(region_s *code)
{
    uint32_t cal_crc = 0;
    int32_t i,blocks,len;
    uint32_t base;
    region_s prog;

    copy_region_info(code,&prog);
    prog.regname = code->regname;
    if(prog.status == MEM_NULL)
    {
        boot_notice("region %s type %s base 0x%x lenth %d is empty.",
                    prog.regname,memtype_name(prog.type),prog.addr,prog.lenth);
        return 0;
    }
    if(prog.status != MEM_ERROR)
    {
        boot_debug("check program base 0x%x,lenth %d",prog.addr,prog.lenth);
        blocks = (prog.lenth + BLOCK_SIZE - 1) / BLOCK_SIZE;
        for(i = 0;i < blocks;i ++)
        {
            base = prog.addr + i * sizeof(commbuffer);
            len = read_block(prog.type,prog.index,base,commbuffer,sizeof(commbuffer)/BLOCK_SIZE);
            if(len <= 0)
            {
                boot_warn("read %s block base 0x%x,lenth %d failed.",
                            memtype_name(prog.type),base,sizeof(commbuffer));
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
            cal_crc = calc_crc32(commbuffer,len,cal_crc);
        }
    }
    
    if(MEM_ERROR == prog.status || cal_crc != prog.crc)
    {
        boot_warn("check program CRC in %s base 0x%x,lenth %d failed.",
                    memtype_name(prog.type),prog.addr,prog.lenth);
        boot_debug("cal_crc:0x%x,crc:0x%x",cal_crc,prog.crc);
        code->status = MEM_ERROR;
        return -1;
    }
    return 0;
}

int32_t check_programs(void)
{
	int idx = 0;
    int32_t ret = 0;
    region_s *code[3];
    int32_t save_flag = 0,i;
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    
    code[idx++] = &bp->mem_map.rom.program1_region;
    code[idx++] = &bp->mem_map.rom.program2_region;
    code[idx++] = &bp->mem_map.run.flash;
    boot_notice("begin to check programs...");
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
            
            boot_warn("check program CRC in %s base 0x%x,lenth %d failed.",
                        memtype_name(code[i]->type),code[i]->addr,code[i]->lenth);
            ret= 1;
        }
    }

    if(save_flag)
    {
        boot_error("program space ERROR.");
        (void)write_param();
        return -1;
    }
    boot_notice("check programs OK.");
    return ret;
    
}

#ifdef __cplusplus
}
#endif
