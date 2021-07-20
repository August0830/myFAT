#include "disk_operation.h"
#define DIR_ATTR 0X10
#define FILE_ATTR 0x20

#define ENT_UNUSED 0X0
#define ENT_RESERVED 0xff0
#define ENT_LASTCLU 0xfff

//分解路径字符串 返回对应文件/文件夹的entry cluster
u16 parsePath(unsigned short *dirClus,const char* path,const unsigned char* ramdisk);
//把数据块内容填写进入entry结构
Entry parseEntStr(const unsigned char* entryStr);
//把一个block中的内容读取到entries数组中
size_t parseEntBlock(const unsigned char* block,Entry *entries);
//转换entry文件名
void parseEntName(const char*entName,char *entname);
//判断entry文件名是否相同
bool entnameEq(const char* input,const char* entName);
//根据名字找entry
Entry getEntryByName(const char* entname,u16 dirClus,const unsigned char*ramdisk);
//获取FAT中下一个cluster
unsigned short getNextClus(unsigned short clus);
//从给定block中 从位置base开始分出长度为len的数字
unsigned int parseNum(const unsigned char *block,size_t base,size_t len);
//从给定block中 base开始处分理出长度为len的字符串
void parseStr(const unsigned char* block,size_t base,size_t len,char* str);