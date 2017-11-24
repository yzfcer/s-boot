/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_debug.h
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: ������Ϣ�������ʽͷ�ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_DEBUG_H__
#define WIND_DEBUG_H__

#include "wind_config.h"
#include "wind_os_hwif.h"

#ifdef __cplusplus
extern "C" {
#endif
//ϵͳ������붨��
#define ERR_OK                 0 //�������ؽ��
#define ERR_COMMAN            -1 //һ�����
#define ERR_NULL_POINTER      -2 //ָ��Ϊ��
#define ERR_PARAM_OVERFLOW    -3 //����Ĳ��������˷�Χ
#define ERR_MEM               -4 //�洢�ռ��ܵ�����
#define ERR_INVALID_PARAM     -6//������Ч
#define ERR_TIMEOUT           -7//������ʱ������ʧ��


//---------------------------------------------------------------------
//ϵͳ������Ϣ��ӡ����
#define WIND_ERROR_OUT_EN 1 //�������������Ϣ
#define WIND_INFO_OUT_EN  1 //���������ͨ����Ϣ��Ϣ
#define WIND_WARN_OUT_EN  1 //�������������Ϣ
#define WIND_DEBUG_OUT_EN 0 //���������Ϣ���

w_int32_t wind_printf(const char *fmt, ...);
//WIND_DEBUG
#if WIND_DEBUG_OUT_EN
#define WIND_DEBUG(fmt,...) do{wind_printf("debug:"fmt" [%s,%d]\r\n",##__VA_ARGS__,__FILE__,__LINE__);wind_printf("[%s,%d]\r\n",__FILE__,__LINE__);}while(0)
#else 
#define WIND_DEBUG(fmt,...)
#endif

//WIND_INFO
#if WIND_INFO_OUT_EN
#define WIND_INFO(fmt,...) do{wind_printf(fmt,##__VA_ARGS__);}while(0)
#else 
#define WIND_INFO(fmt,...) 
#endif

//WIND_WARN
#if WIND_WARN_OUT_EN
#define WIND_WARN(fmt,...) do{wind_printf(fmt,##__VA_ARGS__);wind_printf("[%s,%d]\r\n",__FILE__,__LINE__);}while(0)
#else 
#define WIND_WARN(fmt,...)
#endif

//WIND_ERROR
#if WIND_ERROR_OUT_EN
#define WIND_ERROR(fmt,...) do{wind_printf(fmt,##__VA_ARGS__);wind_printf("[%s,%d]\r\n",__FILE__,__LINE__);}while(0)
#else 
#define WIND_ERROR(fmt,...)
#endif



//---------------------------------------------------------------------
//�ж���������
#define WIND_ASSERT_RETURN(cond,res) do{if(!(cond)) {WIND_ERROR("ASSERT(%s)",#cond);return res;}}while(0)
#define WIND_ASSERT_TODO(cond,todo,res) do{\
    if(!(cond)) {todo;return res;}\
        }while(0)

#ifdef __cplusplus
}
#endif

#endif  //WIND_DEBUG_H__

