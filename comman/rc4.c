#include <stdio.h>
#include <string.h>
 
/*��ʼ������*/
void rc4_init(unsigned char*s, unsigned char*key, unsigned long Len)
{
    int i = 0, j = 0;
    char k[256] = { 0 };
    unsigned char tmp = 0;
    for (i = 0; i<256; i++)
    {
        s[i] = i;
        k[i] = key[i%Len];
    }
    for (i = 0; i<256; i++)
    {
        j = (j + s[i] + k[i]) % 256;
        tmp = s[i];
        s[i] = s[j];//����s[i]��s[j]
        s[j] = tmp;
    }
}
 
/*�ӽ���*/
void rc4_crypt(unsigned char*s, unsigned char*Data, unsigned long Len)
{
    int i = 0, j = 0, t = 0;
    unsigned long k = 0;
    unsigned char tmp;
    for (k = 0; k<Len; k++)
    {
        i = (i + 1) % 256;
        j = (j + s[i]) % 256;
        tmp = s[i];
        s[i] = s[j];//����s[x]��s[y]
        s[j] = tmp;
        t = (s[i] + s[j]) % 256;
        Data[k] ^= s[t];
    }
}
 
int rc4_test()
{
    unsigned char s[256] = { 0 }, s2[256] = { 0 };//S-box
    char key[256] = { "justfortest" };
    char pData[512] = "����һ���������ܵ�����Data";
    unsigned long len = strlen(pData);
    int i;
 
    printf("pData=%s\n", pData);
    printf("key=%s,length=%d\n\n", key, strlen(key));
    rc4_init(s, (unsigned char*)key, strlen(key));//�Ѿ�����˳�ʼ��
    printf("��ɶ�S[i]�ĳ�ʼ�������£�\n\n");
    for (i = 0; i<256; i++)
    {
        printf("%02X", s[i]);
        if (i && (i + 1) % 16 == 0)putchar('\n');
    }
    printf("\n\n");
    for (i = 0; i<256; i++)//��s2[i]��ʱ����������ʼ����s[i]������Ҫ�ģ�����
    {
        s2[i] = s[i];
    }
    printf("�Ѿ���ʼ�������ڼ���:\n\n");
    rc4_crypt(s, (unsigned char*)pData, len);//����
    printf("pData=%s\n\n", pData);
    printf("�Ѿ����ܣ����ڽ���:\n\n");
    //rc4_init(s,(unsignedchar*)key,strlen(key));//��ʼ����Կ
    rc4_crypt(s2, (unsigned char*)pData, len);//����
    printf("pData=%s\n\n", pData);
    return 0;
}