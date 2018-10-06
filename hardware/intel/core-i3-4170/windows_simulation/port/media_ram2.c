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
#include "boot_media.h"
#include "wind_string.h"
#include "wind_debug.h"

#define RAM2_SIZE 0x100000
#define RAM2_BLKSIZE 512
w_uint8_t g_ram2[RAM2_SIZE];

static w_err_t ram2_init(w_media_s *media)
{
    wind_memset(g_ram2,0,RAM2_SIZE);
    return W_ERR_OK;
}
static w_int32_t ram2_read_blk(w_media_s *media,w_uint32_t addr,w_uint8_t *data,w_int32_t blkcnt)
{
    wind_memcpy(data,&g_ram2[addr],blkcnt * RAM2_BLKSIZE);
    return blkcnt;
}
static w_int32_t ram2_write_blk(w_media_s *media,w_uint32_t addr,w_uint8_t *data,w_int32_t blkcnt)
{
    wind_memcpy(&g_ram2[addr],data,blkcnt * RAM2_BLKSIZE);
    return blkcnt;
}

static w_int32_t ram2_erase_blk(w_media_s *media,w_uint32_t addr,w_int32_t blkcnt)
{
    wind_memset(&g_ram2[addr],0,blkcnt * RAM2_BLKSIZE);
    return blkcnt;
}
static w_err_t ram2_erase_all(w_media_s *media)
{
    wind_memset(g_ram2,0,RAM2_SIZE);
    return W_ERR_OK;
}

static w_media_ops_s ops = 
{
    ram2_init,
    ram2_read_blk,
    ram2_write_blk,
    ram2_erase_blk,
    ram2_erase_all
};

w_media_s media_ram2 = 
{
    "ram2",
    IRAM,
    0,
    RAM2_SIZE,
    RAM2_BLKSIZE,
    0,
    &ops
};

