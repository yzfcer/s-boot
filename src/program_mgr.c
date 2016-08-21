

/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2015.10.22
** 创建人：周江村
**文件版本1.0
** 修改记录：
***************************************************************************************/
#include "menu_list.h"
#include "port.h"
#include "boot_debug.h"
#include "boot_param.h"
#include "main.h"
#include "boot_debug.h"
#include "crc.h"
#include "share_param.h"
#include "program_mgr.h"
#include "mem_map.h"
#include "encrypt.h"
#include "boot_hw_if.h"
#include "mem_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

//通用缓存器，一般在拷贝数据时做缓存，或者接收命令字符
char commbuffer[BLOCK_SIZE];

static void print32_t_copy_percents(int32_t numerator, int32_t denominator,int32_t del)
{
    if(del)
        printk_rt("%c%c%c%c",8,8,8,8);
    printk_rt("%3d%%",numerator*100/denominator);
        feed_watchdog();
}

uint32_t get_ram_addr(uint32_t addr,memtype_e type)
{
	uint32_t base;
    if(IRAM == type)
       {
       	base = get_iram_base();
    	return base + addr;
       }
    else if(XRAM == type)
       {
       	base = get_xram_base();
    	return base + addr;
       }
    else
    {
        return INVALID_REAL_ADDR;
    }
}

uint32_t GET_INT_BY_STR(uint8_t *str,int32_t index)
{
    uint32_t i,tmp,va = 0;
    for(i = 0;i < 4;i ++)
    {
        tmp = str[index+i];
        va += (tmp << (i*8));
    }
    return va;
}

void FILL_BYTES_BY_INT(uint8_t *buf,int32_t index,uint32_t va)
{
    int32_t i;
    for(i = 0;i < 4;i ++)
    {
        buf[i + index] = (uint8_t)(va >> (i*8));
    }
}

int32_t check_and_decrypt_img(region_s *img,region_s *bin)
{
    int32_t ret;
    uint32_t real_addr,cal_crc,crc;
    img_head_s *imginfo;
    
    real_addr = get_ram_addr(img->base,img->type);
    if(INVALID_REAL_ADDR == real_addr)
    {
        boot_error("memory map error.");
        return -1;
    }
    
    //对接受的数据做CRC校验
    real_addr = get_ram_addr(img->base,img->type);
    if(INVALID_REAL_ADDR == real_addr)
    {
        boot_error("memory map error.");
        return -1;
    }
    feed_watchdog();
    crc = GET_INT_BY_STR((uint8_t*)real_addr,img->lenth - 4);
    cal_crc = calc_crc32((char*)real_addr,img->lenth - 4,0);
    boot_debug("crc:0x%x,calc_crc:0x%x.",crc,cal_crc);
    if(cal_crc != crc)
    {
        boot_warn("img file crc ERROR.");
        return -1;
    }
    feed_watchdog();
    boot_notice("img file verify OK.");
    
        
    //解密
    boot_notice("img file decrypt");
        //while(1);
    
    boot_debug("DecrypUpDData base:0x%x,lenth:%d",real_addr,img->lenth - 4);
    ret = DecrypUpDData(real_addr,img->lenth - 4,0);
    if(0 != ret)
    {
        boot_warn("img file Decrypt ERROR.");
        return -1;
    }
    feed_watchdog();
    boot_notice("img file decrypt OK.");

    imginfo = (img_head_s*)(real_addr+10);
    printk_rt("decrypt img file: \r\n");
    printk_rt("file name:%s\r\n",imginfo->file_name);
    printk_rt("lenth:%d\r\n",imginfo->file_len);

    //内层校验
    cal_crc = calc_crc32((char*)(real_addr+imginfo->head_len),imginfo->file_len,0);
    if(imginfo->file_crc != cal_crc)
    {
        boot_warn("bin file crc ERROR.");
        return -1;
    }
    feed_watchdog();
    boot_notice("bin file verify OK.");
    bin->regname = img->regname;
    bin->base = img->base + imginfo->head_len;
    bin->lenth = imginfo->file_len;
    bin->type = img->type;
    bin->crc = imginfo->file_crc;
      boot_debug("bin lenth:%d,crc:0x%x.",bin->lenth,cal_crc);
    return 0;
}

//对解密状态的数据重新加密
int32_t encrypt_code(region_s *code_reg)
{
    uint32_t real_addr;
    int32_t ret;
    //img_head_s *head;
    //获取实际地址
    real_addr = get_ram_addr(code_reg->base,code_reg->type);
    if(INVALID_REAL_ADDR == real_addr)
    {
        boot_error("memory map error.");
        return -1;
    }

    //对数据重新加密，将加密后的数据烧录到SFLASH空间
    //head = (img_head_s*)(real_addr+10);
    
    boot_debug("DecrypUpDData base:0x%x,lenth:%d",real_addr,code_reg->lenth - 4);
    ret = DecrypUpDData(real_addr,code_reg->lenth -4,1);
    if(0 != ret)
    {
        boot_warn("img file encrypt ERROR.");
        return -1;
    }
    
    boot_notice("img file encrypt OK.");
    
    code_reg->crc = calc_crc32((char *)real_addr,code_reg->lenth - 4,0);
    //code_reg->lenth -= 4;
    FILL_BYTES_BY_INT((uint8_t *)real_addr,code_reg->lenth - 4,code_reg->crc);
    
    boot_debug("new file CRC:0x%x.",code_reg->crc);
    return 0;
}




int32_t copy_data_on_memory(region_s *src,region_s *dest)
{
    int32_t i,j,len,blocks,times;
    uint32_t base;

    if(0 >= src->lenth)
        return 0;
    if(dest->maxlen < src->lenth)
    {
        boot_warn("space is NOT enough.");
        return -1;
    }
    boot_notice("copy data from %s to %s lenth %d.",
                src->regname,dest->regname,src->lenth);
    boot_debug("source type %s,base 0x%x,lenth %d dest type,%s,base 0x%x,lenth %d.",
                sys_memtype[src->type],src->base,src->lenth,
                sys_memtype[dest->type],dest->base,dest->maxlen);
    
    blocks = (src->lenth + BLOCK_SIZE - 1) / BLOCK_SIZE;
    printk_rt("complete:");
    print32_t_copy_percents(0,1,0);
    for(i = 0;i < blocks;i ++)
    {    
        for(times = 0;times < 3;times ++)
        {
            base = src->base + i * sizeof(commbuffer);
            if(i >= blocks - 1)
            {
                for(j = 0;j < sizeof(commbuffer);j ++)
                    commbuffer[j] = 0;
            }
            len = read_block(src->type,base,commbuffer,sizeof(commbuffer)/BLOCK_SIZE);
            if(len > 0)
                break;
        }
        if(times >= 3)
        {
            boot_warn("read block 0x%x,lenth %d failed.",base,sizeof(commbuffer));
            dest->status = MEM_ERROR;
            return -1;
        }

        for(times = 0;times < 3;times ++)
        {
            base = dest->base + i * sizeof(commbuffer);
            len = write_block(dest->type,base,commbuffer,sizeof(commbuffer)/BLOCK_SIZE);
            if(len > 0)
                break;
        }
        if(times >= 3)
        {
            boot_warn("read block 0x%x,lenth %d failed.",base,sizeof(commbuffer));
            dest->status = MEM_ERROR;
            return -1;
        }
        print32_t_copy_percents(i,blocks,1);
        feed_watchdog();
    }
    print32_t_copy_percents(i,blocks,1);
    printk_rt("\r\n");

    dest->lenth = src->lenth;
    dest->crc = src->crc;
    dest->status = MEM_NORMAL;

    boot_debug("copy data OK."); 
    return 0;    
}



//去掉文件头部，且不用加密，烧录二进制文件
int32_t write_derect_space(region_s *code_reg,region_s *dest)
{
    int32_t ret;
    region_s bin_reg;
    img_head_s *head;
    uint32_t real_addr;
    boot_param_s *bp = (boot_param_s*)sys_boot_params();
    
    real_addr = get_ram_addr(code_reg->base,bp->mem_map.ram.probuf_region.type);
    if(INVALID_REAL_ADDR == real_addr)
    {
        boot_error("memory map error.");
        return -1;
    }

    head = (img_head_s*)(real_addr+10);
    bin_reg.base = code_reg->base + head->head_len;
    bin_reg.lenth = code_reg->lenth - head->head_len - 4;
    bin_reg.type = code_reg->type;
    bin_reg.crc = head->file_crc;
    bin_reg.lenth = head->file_len;
    
    ret = copy_data_on_memory(&bin_reg,dest);
    return ret;    
}


int32_t flush_code_to_ram(region_s *code_region)
{
    int32_t ret;
    boot_param_s *bp = (boot_param_s*)sys_boot_params();
    ret = copy_data_on_memory(code_region,&bp->mem_map.run.iflash);
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
    boot_param_s *bp = (boot_param_s*)sys_boot_params();

    boot_notice("begin to flush code to IFLASH space...");
    if(APP_NOEMAL == bp->app_type)
    {
        //先将原来的程序拷贝到备份空间    
        src = &bp->mem_map.rom.program1_region;
        ret = copy_data_on_memory(src,&bp->mem_map.rom.programbak_region);
        if(0 != ret)
        {
            boot_warn("backup old program failed.");
            return -1;
        }
    }

    //将新程序烧到运行空间
    ret = copy_data_on_memory(bin,&bp->mem_map.rom.program1_region);
    if(0 != ret)
    {
        boot_error("write new program failed.");
        return -1;
    }
    bp->mem_map.run.iflash.status = MEM_NORMAL;
    
    bp->mem_map.run.iflash.crc = bin->crc;
    bp->mem_map.run.iflash.lenth= bin->lenth;
    bp->app_type = APP_NOEMAL;
    
    return 0;
}



int32_t flush_code_to_sflash(region_s *img,region_s *bin)
{
    int32_t ret;
    boot_param_s *bp = (boot_param_s*)sys_boot_params();
    region_s *old_code;

    boot_notice("begin to flush code to XFLASH space...");
    if(APP_NOEMAL == bp->app_type)
    {
        //先将原来的SFLASH程序数据备份
        old_code = &bp->mem_map.rom.program1_region;
        ret = copy_data_on_memory(old_code,&bp->mem_map.rom.programbak_region);
        if(0 != ret)
        {
            boot_warn("backup old program failed.");
            return -1;
        }
    }

    //如果是在IFLASH运行，在加密前先烧录运行程序空间
    if(IFLASH == bp->mem_map.run.iflash.type)
    {
        ret = copy_data_on_memory(bin,&bp->mem_map.run.iflash);
        if(0 != ret)
        {
            boot_error("write new program to running space failed.");
            return -1;
        }
    }
    bp->app_type = APP_NOEMAL;
    
    //将数据重新加密
    ret = encrypt_code(img);
    if(0 != ret)
    {
        boot_error("encrypt code error");
        return -1;
    }
    
    ret = copy_data_on_memory(img,&bp->mem_map.rom.program1_region);
    if(0 != ret)
    {
        boot_error("write new program failed.");
        return -1;
    }
    return 0;
}


int32_t flush_code_to_product(region_s *img,region_s *bin)
{
    int32_t ret;
    region_s *src;
    boot_param_s *bp = (boot_param_s*)sys_boot_params();

    boot_notice("begin to flush code to producting space...");
    //如果是在IFLASH运行，先烧录运行程序
    if(IFLASH == bp->mem_map.run.iflash.type)
    {
        ret = copy_data_on_memory(bin,&bp->mem_map.run.iflash);
        if(0 != ret)
        {
            boot_error("write new program to running space failed.");
            return -1;
        }
        if(IFLASH == bp->mem_map.rom.program1_region.type)
        {
            bp->mem_map.rom.program1_region.lenth = bin->lenth;
            bp->mem_map.rom.program1_region.crc = bin->crc;
        }
    }
    bp->app_type = APP_PRODUCT;


    if(XFLASH == bp->mem_map.rom.product_region.type)
    {
        //将数据重新加密
        ret = encrypt_code(img);
        if(0 != ret)
        {
            boot_error("encrypt code error");
            return -1;
        }
        src = img;
    }
    else
    {
        src = bin;
    }
    
    ret = copy_data_on_memory(src,&bp->mem_map.rom.product_region);
    if(0 != ret)
    {
        boot_error("write product program failed.");
        return -1;
    }
    
    return 0;
}

int32_t flush_code_data(downtype_e type,region_s *img,region_s *bin)
{
    int32_t ret;   
    switch(type)
    {
        case DOWN_IRAM:
        case DOWN_XRAM:
            ret = flush_code_to_ram(bin);
            break;
        case DOWN_IFLASH:
            ret = flush_code_to_iflash(bin);
            break;
        case DOWN_SFLASH:
            ret = flush_code_to_sflash(img,bin);
            break;
        case DOWN_PRODUCT:
            ret = flush_code_to_product(img,bin);
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
    if(DOWN_IRAM != type && DOWN_XRAM != type)
        (void)write_param();
    return ret;
}

int32_t download_img_file(downtype_e type)
{
    int32_t ret,len;
    region_s *img,bin;
    boot_param_s *bp = (boot_param_s*)sys_boot_params();

    if(bp->debug_mode)
    {
        boot_notice("device can NOT download in debug mode ,set it to normal mode first");
        return -1;
    }
    img = &bp->mem_map.ram.probuf_region;
    printk_rt("begin to receive file data,please wait.\r\n");
    len = receive_img_data(img->base,img->maxlen);
    if(len <= 0)
    {
        boot_error("receive img data failed.");
        return -1;
    }

    img->lenth = (uint32_t)len;
    ret = check_and_decrypt_img(img,&bin);
    if(0 != ret)
    {
        boot_error("check img file ERROR");
        return -1; 
    }
        
    ret = flush_code_data(type,img,&bin);
    if(0 != ret)
    {
        boot_warn("flush data to %s failed.",sys_memtype[type]);
        return -1;
    }
    boot_notice("img flush OK.");
    return 0;
}

void clean_program(void)
{
    
    uint32_t i,blocknum;
    region_s *code[6];
    boot_param_s *bp = (boot_param_s*)sys_boot_params();
    printk_rt("clearing program ...\r\n");
    code[0] = &bp->mem_map.rom.program1_region;
    code[1] = &bp->mem_map.rom.programbak_region;
    code[2] = &bp->mem_map.rom.product_region;
    code[3] = &bp->mem_map.run.iflash;
    code[4] = &bp->mem_map.rom.param1_region;
    code[5] = &bp->mem_map.rom.param2_region;
    
    for(i = 0;i < 6;i ++)
    {   
        boot_notice("erase base 0x%x,lenth %d.",code[i]->base,code[i]->lenth);
        blocknum = (code[i]->lenth + BLOCK_SIZE - 1) / BLOCK_SIZE;
        erase_block(code[i]->type,code[i]->base,blocknum);
    }
    printk_rt("clear program OK.\r\n");
}

int32_t write_encrypt_code_to_run(region_s *src,region_s *run)
{
    int32_t ret;
    region_s img,bin;
    boot_param_s *bp = (boot_param_s*)sys_boot_params();

    ret = copy_data_on_memory(src,&bp->mem_map.ram.probuf_region);
    if(0 != ret)
    {
        boot_warn("copy data error.");
        return -1;
    }
    
    copy_region_info(&bp->mem_map.ram.probuf_region,&img);
    img.lenth = src->lenth;

    ret = check_and_decrypt_img(&img,&bin);
    if(0 != ret)
    {
        boot_error("check img file ERROR.");
        return -1;
    }
    
    ret = copy_data_on_memory(&bin,run);
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
    boot_param_s *bp = (boot_param_s *)sys_boot_params();
    boot_notice("begin to change boot App:%d.",index);
    switch(index)
    {
        case APP_IDX_PRO1:
            copy_region_info(&bp->mem_map.rom.programbak_region,&src);
            bp->app_type = APP_NOEMAL;
            break;
        case APP_IDX_PROBAK:
            copy_region_info(&bp->mem_map.rom.programbak_region,&src);
            bp->app_type = APP_NOEMAL;
            break;
        case APP_IDX_PRODUCT:
            copy_region_info(&bp->mem_map.rom.product_region,&src);
            bp->app_type = APP_PRODUCT;
            break;
        default:
            boot_warn("undefined index:%d.",index);
            return -1;
    }
    
    if(src.base == bp->mem_map.run.iflash.base)
    {
        boot_notice("need NOT to write program.");
        return 0;        
    }
    
    if(src.lenth <= 0)
    {
        boot_warn("program is NOT existing.");
        return -1;
    }
    
    if(IFLASH == src.type)
    {
        ret = copy_data_on_memory(&src,&bp->mem_map.run.iflash);
        if(IFLASH == bp->mem_map.rom.program1_region.type)
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
    else if(XFLASH == src.type)
    {
        ret = write_encrypt_code_to_run(&src,&bp->mem_map.run.iflash);
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
    
    if(prog.status == MEM_NULL)
    {
        boot_notice("  ***  space %s type %s base 0x%x lenth %d is empty.",
                    prog.regname,sys_memtype[prog.type],prog.base,prog.lenth);
        return 0;
    }
    if(prog.status != MEM_ERROR)
    {
        if(XFLASH == prog.type)
        {
            prog.lenth -= 4;
        }
        boot_debug("check program base 0x%x,lenth %d",prog.base,prog.lenth);
        blocks = (prog.lenth + BLOCK_SIZE - 1) / BLOCK_SIZE;
        for(i = 0;i < blocks;i ++)
        {
            base = prog.base + i * sizeof(commbuffer);
            len = read_block(prog.type,base,commbuffer,sizeof(commbuffer)/BLOCK_SIZE);
            if(len <= 0)
            {
                boot_warn("read %s block base 0x%x,lenth %d failed.",
                            sys_memtype[prog.type],base,sizeof(commbuffer));
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
                    sys_memtype[prog.type],prog.base,prog.lenth);
        boot_debug("cal_crc:0x%x,crc:0x%x",cal_crc,prog.crc);
        code->status = MEM_ERROR;
        return -1;
    }
    return 0;
}

int32_t check_programs(void)
{
    int32_t ret = 0;
    region_s *code[4];
    int32_t save_flag = 0,i;
    boot_param_s *bp = (boot_param_s *)sys_boot_params();
    
    code[0] = &bp->mem_map.rom.program1_region;
    code[1] = &bp->mem_map.rom.programbak_region;
    code[2] = &bp->mem_map.rom.product_region;
    code[3] = &bp->mem_map.run.iflash;
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
                        sys_memtype[code[i]->type],code[i]->base,code[i]->lenth);
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
