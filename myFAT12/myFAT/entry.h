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
//填写或者分离entry中单位为char的内容
void parseEntCharStr(const char* ch,unsigned char* entStr,size_t base,size_t len);
//填写或者分离entry中单位为unsigned int的内容
void parseEntNum(unsigned int entNum,unsigned char* entStr,size_t base,size_t len);
//把一个block中的内容读取到entries数组中
size_t parseEntBlock(const unsigned char* block,Entry *entries);
//转换entry文件名
void parseEntName(const char*entName,char *entname);
//判断entry文件名是否相同
bool entnameEq(const char* input,const char* entName);
//根据名字找entry
Entry getEntryByName(const char* entname,u16 dirClus,const unsigned char*ramdisk);
//根据clus找entry
Entry getEntryByClus(u16 ent_clus,u16 dir_clus,const unsigned char*ramdisk);
//找到目录所在entry
size_t getDirEnt(size_t *block_idx,u16 ent_clus,u16 dir_clus,const unsigned char* ramdisk);
//获得entry所在block内的下标
size_t getEntIdx(u16 ent_clus,const unsigned char*block);

//从给定block中 从位置base开始分出长度为len的数字
unsigned int parseNum(const unsigned char *block,size_t base,size_t len);
//从给定block中 base开始处分理出长度为len的字符串
void parseStr(const unsigned char* block,size_t base,size_t len,char* str);
//提取entry内容
void parseEnt(const Entry *entry,unsigned char* entStr);
//提取多个entry的内容
void parseEnts(unsigned char *block,const Entry *entries,size_t ent_cnt);
//初始化一个entry
Entry setEnt(const char* entname,unsigned char attr,unsigned short firstclus,unsigned int filesize);

//获取新的entry 并返回所在block的idx
size_t getDirFreeEnt(size_t *block_idx,unsigned short dir_clus,unsigned char* ramdisk);
//获取新的entry在block中的idx
size_t getFreeEntIdx(const unsigned char *block);


//获取FAT中下一个cluster
unsigned short getNextClus(unsigned short clus);
//在FAT中加入新的文件的clus
void addEntClus(unsigned short frst_clus,unsigned short new_clus);
//在FAT中设置clus的内容
void setFatClus(unsigned short clus, unsigned short newClus);
//获得一个新的空的clus
unsigned short getFreeClus();