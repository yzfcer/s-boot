/***************************************************************************************
** �ļ�����
** �ļ���Ϣ��
** ����ʱ�䣺2016.09.25
** �����ˣ���纣��
**�ļ��汾 1.0
** �޸ļ�¼��
***************************************************************************************/
#ifndef BOOT_INPUT_IF_H__
#define BOOT_INPUT_IF_H__
#ifdef __cplusplus
extern "C" {
#endif
void boot_delay(uint32_t ms);

int boot_printf(const char *fmt,...);

int32_t read_char_blocking(char *ch);
int32_t read_line_blockig(char *buff,int32_t len);
int32_t wait_for_key_input(int32_t to_sec,char *ch,int32_t print32_t_flag);

int32_t read_block(memtype_e memtype,uint32_t memidx,uint32_t addr,uint8_t *buf,int32_t blkcount);
int32_t write_block(memtype_e memtype,uint32_t memidx,uint32_t addr,uint8_t *buf,int32_t blkcount);
int32_t erase_block(memtype_e memtype,uint32_t memidx,uint32_t addr,int32_t blkcount);

#ifdef __cplusplus
}
#endif

#endif
