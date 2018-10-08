
#include "boot_config.h"
#include "boot_hw_if.h"
#include "boot_part.h"
#include "boot_port.h"
#include "wind_debug.h"
#include "wind_crc32.h"
#include "wind_string.h"
#include "wind_debug.h"
#include "boot_param.h"
static w_part_s g_part[PART_COUNT];
static w_uint8_t commbuffer[COMMBUF_SIZE];

w_uint8_t *get_common_buffer(void)
{
    return commbuffer;
}

static w_part_s *get_null_part(void)
{
    w_int32_t i;
    for(i = 0;i < PART_COUNT;i ++)
    {
        if(!g_part[i].used)
            return &g_part[i];
    }
    return NULL;
}

w_err_t boot_part_init(void)
{
    w_err_t err;
    wind_memset(g_part,0,sizeof(g_part));
    err = boot_parts_create();
    return W_ERR_OK;
}

w_bool_t  boot_part_create(const char *name,w_media_s *md,w_uint32_t size)
{
    w_part_s *part;
    wind_notice("create part:%s",name);
    WIND_ASSERT_RETURN(name != W_NULL,B_FALSE);
    WIND_ASSERT_RETURN(wind_strlen(name) < PART_NAME_LEN,B_FALSE);
    WIND_ASSERT_RETURN(md != W_NULL,B_FALSE);
    WIND_ASSERT_RETURN(size < md->size,B_FALSE);
    WIND_ASSERT_RETURN(size + md->offset <= md->size,B_FALSE);
    part = get_null_part();
    WIND_ASSERT_RETURN(part != W_NULL,B_FALSE);
    wind_strcpy(part->name,name);
    wind_strcpy(part->media_name,md->name);
    part->mtype = md->mtype;
    part->used = 1;
    part->base = md->offset;
    part->size = size;
    part->blksize = md->blksize;
    part->status = MEM_NULL;
    part->datalen = 0;
    part->offset = 0;
    part->crc = 0;
    md->offset += size;
    return B_TRUE;
}


w_part_s *boot_part_get(const char *name)
{
    w_int32_t i;
    for(i = 0;i < PART_COUNT;i ++)
    {
        if(g_part[i].used && (wind_strcmp(name,g_part[i].name) == 0))
            return &g_part[i];
    }
    return NULL;
}

w_err_t boot_part_seek(w_part_s *part,w_int32_t offset)
{
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(offset >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(offset < part->size,W_ERR_INVALID);
    part->offset = offset;
    return W_ERR_OK;
}

w_err_t boot_part_calc_crc(w_part_s *part)
{
    w_int32_t blkcnt;
    w_int32_t size;    
    w_uint8_t *buff;
    w_uint32_t offset;
    w_uint32_t crc = 0xffffffff;
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_PTR_NULL);
    buff = get_common_buffer();
    blkcnt = COMMBUF_SIZE / part->blksize;
    WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_FAIL);
    size = blkcnt * part->blksize;
    offset = 0;
    part->offset = 0;
    while(part->offset < part->datalen)
    {
        size = boot_part_read(part,buff,COMMBUF_SIZE);
        WIND_ASSERT_RETURN(size > 0,W_ERR_FAIL);
        crc = wind_crc32(buff,size,crc);
        offset += size;
    }
    part->crc = crc;
    return W_ERR_OK;
}



w_int32_t boot_part_read(w_part_s *part,w_uint8_t *data,w_uint32_t datalen)
{
    w_uint32_t blkcnt;
    w_uint32_t size;
    w_media_s *media;
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(data != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(datalen >= part->blksize,W_ERR_INVALID);
    if(datalen > part->size)
        datalen = part->size;
    if(datalen + part->offset > part->size)
        datalen = part->size - part->offset;
    WIND_ASSERT_RETURN(datalen > 0,0);
    WIND_ASSERT_RETURN(datalen % part->blksize == 0,W_ERR_INVALID);
    media = boot_media_get(part->media_name);
    blkcnt = datalen / part->blksize;
    blkcnt = media->ops->read_blk(media,part->base + part->offset,data,blkcnt);
    WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_FAIL);
    size = blkcnt * part->blksize;
    part->offset += size;
    return size;
}

w_int32_t boot_part_write(w_part_s *part,w_uint8_t *data,w_uint32_t datalen)
{
    w_uint32_t blkcnt;
    w_uint32_t size;
    w_media_s *media;
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(data != W_NULL,W_ERR_PTR_NULL);
    if(datalen > part->size)
        datalen = part->size;
    if(datalen + part->offset > part->size)
        datalen = part->size - part->offset;
    WIND_ASSERT_RETURN(datalen > 0,0);
    if(datalen < part->blksize)
        datalen = part->blksize;
    media = boot_media_get(part->media_name);
    blkcnt = datalen / part->blksize;
    blkcnt = media->ops->write_blk(media,part->base + part->offset,data,blkcnt);
    WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_FAIL);
    size = blkcnt * part->blksize;
    part->offset += size;
    part->datalen = part->offset;
    return size;
}

w_err_t boot_part_erase(w_part_s *part)
{
    w_uint32_t blkcnt;
    w_media_s *media;
    WIND_ASSERT_RETURN(part != NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(part->size > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(part->blksize > 0,W_ERR_INVALID);
    media = boot_media_get(part->media_name);
    blkcnt = part->size / part->blksize;
    blkcnt = media->ops->erase_blk(media,part->base,blkcnt);
    WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_FAIL);
    return W_ERR_OK;
}

void boot_part_print(void)
{
    w_int32_t i;
    w_int32_t count = boot_part_get_count();
    w_part_s *part = boot_part_get_list();
    wind_printf("system part infomation:\r\n");
    wind_print_space(9);
    wind_printf("%-15s%-8s%-12s%-11s%-11s%-9s%-8s\r\n","part","media","addr","size","datalen","type","usage");
    wind_print_space(9);
    for(i = 0;i < count;i ++)
    {
        wind_printf("%-15s%-8s0x%-10x0x%-9x0x%-9x%-9s%4d%%\r\n",part[i].name,part[i].media_name,part[i].base,part[i].size,\
                part[i].datalen,part[i].name,part[i].size?(part[i].datalen*100)/part[i].size:0);
    }
    wind_print_space(9);
}



w_int32_t boot_part_get_count(void)
{
    w_int32_t i;
    for(i = 0;i < PART_COUNT;i ++)
    {
        if(!g_part[i].used)
            return i;
    }
    return 0;
}

 
w_part_s *boot_part_get_list(void)
{
    return g_part;
}

void boot_part_copy_info(w_part_s *src,w_part_s *dest)
{
    wind_strcpy(dest->name,src->name);
    wind_strcpy(dest->media_name,src->media_name);
    dest->mtype = src->mtype;
    dest->used = src->used;
    dest->status = src->status;
    dest->base = src->base;
    dest->size = src->size;
    dest->blksize = src->blksize;
    dest->datalen = src->datalen;
    dest->offset = src->offset;
    dest->crc = src->crc;
}

static void print_copy_percents(w_int32_t numerator, w_int32_t denominator,w_int32_t del)
{
    if(del)
        wind_printf("%c%c%c%c",8,8,8,8);
    wind_printf("%3d%%",numerator*100/denominator);
    feed_watchdog();
}

w_int32_t boot_part_copy_data(w_part_s *src,w_part_s *dest)
{
    w_int32_t i,len,blocks,times;
    w_uint8_t *buff = get_common_buffer();

    if(0 >= src->datalen)
        return 0;
    if(dest->size < src->datalen)
    {
        wind_warn("space is NOT enough.");
        return -1;
    }
    wind_notice("copy data from \"%s\" to \"%s\" lenth %d.",
                src->name,dest->name,src->datalen);
    wind_debug("source part %s,addr 0x%x,lenth %d dest part,%s,addr 0x%x,lenth %d.",
                src->name,src->base,src->datalen,
                src->name,dest->base,dest->size);
    src->offset = 0;
    dest->offset = 0;
    blocks = (src->datalen + COMMBUF_SIZE - 1) / COMMBUF_SIZE;
    wind_printf("complete:");
    print_copy_percents(0,1,0);
    for(i = 0;i < blocks;i ++)
    {    
        for(times = 0;times < 3;times ++)
        {
            wind_memset(buff,0,COMMBUF_SIZE);
            len = boot_part_read(src,buff,COMMBUF_SIZE);
            if(len > 0)
                break;            
        }
        if(times >= 3)
        {
            wind_warn("read block offset 0x%x,lenth %d failed.",src->offset,COMMBUF_SIZE);
            dest->status = MEM_ERROR;
            return -1;
        }

        for(times = 0;times < 3;times ++)
        {
            len = boot_part_write(dest,buff,COMMBUF_SIZE);
            if(len > 0)
                break;
        }
        if(times >= 3)
        {
            wind_warn("read block offset 0x%x,lenth %d failed.",dest->offset,COMMBUF_SIZE);
            dest->status = MEM_ERROR;
            return -1;
        }
        print_copy_percents(i,blocks,1);
        feed_watchdog();
    }
    print_copy_percents(i,blocks,1);
    wind_printf("\r\n");

    dest->datalen = src->datalen;
    dest->crc = src->crc;
    dest->status = MEM_NORMAL;

    wind_debug("copy data OK."); 
    return 0;    
}

w_bool_t boot_part_equal(w_part_s *src,w_part_s *dest)
{
    if(src->mtype != dest->mtype)
        return B_FALSE;
    if(wind_strcmp(src->media_name,dest->media_name))
        return B_FALSE;
    if(src->base != dest->base)
        return B_FALSE;
    if(src->size != dest->size)
        return B_FALSE;
    return B_TRUE;
}

w_bool_t boot_part_check(w_part_s *part)
{
    w_uint32_t crc;
    WIND_ASSERT_RETURN(part != W_NULL,B_FALSE);
    if(part->status == MEM_NULL)
        return B_TRUE;
    if(part->status == MEM_ERROR)
        return B_FALSE;
    crc = boot_part_calc_crc(part);
    if(part->crc != crc)
        return B_FALSE;
    return B_TRUE;
}

