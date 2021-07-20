#include "entry.h"
#include <iostream>
#include <string.h>
using namespace std;
static unsigned char fat[BLOCKSIZE*9];//FAAT table

unsigned int parseNum(const unsigned char* block,size_t base,size_t len)
{
    unsigned int num;
    for(size_t offset=0;offset<len;++offset)
        num|=block[base+offset]<<(offset*8);
        //位操作 为了保证对应位在位置商 右移offset个字节 8*offset个比特
    return num;
}

void parseStr(const unsigned char* block,size_t base,size_t len,char* str)
{
    for(size_t offset=0;offset<len;++offset)
    {
        str[offset]=block[base+offset];
    }
}


u16 parsePath(unsigned short *dirClus,const char* path,const unsigned char* ramdisk)
{
    const char delim[] = "/";
    char *pathCopy = strdup(path);
    Entry entry;
    if(path[0]=='/')
    {
        entry.first_cluster=*dirClus=0;
        strsep(&pathCopy,delim);
    }
    char* entname = strsep(&pathCopy,delim);
    while(entname!=NULL)
    {
        if(!pathCopy && strlen(entname)==0)
            break;
        entry = getEntryByName(entname,*dirClus,ramdisk);//根据文件夹名字查找对应entry
        if(pathCopy && strlen(pathCopy))
            *dirClus = entry.first_cluster;//dirClus转移 到该文件夹下继续查找
        if(entry.first_cluster==(unsigned  short)-1)// 没找到
            break;
        entname = strsep(&pathCopy,delim);   
    }
    return entry.first_cluster;
}

Entry parseEntStr(const unsigned char* entryStr)
{
    Entry entry;
    parseStr(entryStr,0,11,entry.filename);
    entry.attr = parseNum(entryStr,11,1);
    parseStr(entryStr,12,2,entry.reserved);
    entry.first_cluster=parseNum(entryStr,26,2);
    entry.file_size=parseNum(entryStr,28,4);
    return entry;
}

size_t parseEntBlock(const unsigned char* block,Entry *entries)
{
    size_t entCnt=0;
    for(size_t entIdx=0;entIdx<BLOCKSIZE/BYTEPERENT;entIdx++)
    {
        if(block[entIdx*BYTEPERENT]==ENT_UNUSED)//以字节为单位进行判断 因为数uchar
            continue;
        //if(block[entIdx*BYTEPERENT]==0xe5)
            //continue;
        entries[entCnt++]=parseEntStr(block+entIdx*BYTEPERENT);
        //指针偏移 对block内被划分成16个entry的可用结构进行检查
    }
    return entCnt;
}

void parseEntName(const char*entName,char *entname)
{
    if(!strcmp(".           ",entName))
    {
        strcpy(entname,".");
        return;
    }
    if(!strcmp("..          ",entName))
    {
        strcpy(entname,"..");
        return;
    }
    size_t len=0;
    for(size_t i=0;i<8;++i)
    {
        if(entName[i]==' ')
            break;
        entname[len++]=entName[i];
    }
    if(entName[8]==' ')
    {
        entname[len]='\0';
        return ;
    }
    entname[len++]='.';
    for(size_t i=8;i<11;++i)
    {
        if(entName[i]==' ')
            break;
        entname[len++]=entName[i];
    }
    entname[len]='\0';
}

bool entnameEq(const char* input,const char* entName)
{
    char entname[12];
    parseEntName(entName,entname);
    return !strcmp(input,entname);
}



Entry getEntryByName(const char* entname,u16 dirClus,const unsigned char*ramdisk)
{
    if(dirClus==0)//在根目录下读取的block的计算方式略有不同
    {
        for(size_t baseSec=19,secOfst=0;secOfst<14;secOfst++)
        {
            unsigned char block[BLOCKSIZE];
            Read_rmdisk_block(ramdisk,baseSec+secOfst,block);
            Entry entries[16];
            size_t entCnt=parseEntBlock(block,entries);
            //读出该块内所有entry
            for(size_t i=0;i<entCnt;++i)
                if(entnameEq(entname,entries[i].filename))
                    return entries[i];
        }
        Entry entry;
        entry.first_cluster=-1;
        return entry;
    }
    else
    {
        for(unsigned short clus = dirClus;clus!=ENT_LASTCLU;
        clus=getNextClus(clus))
        {
            unsigned char block[BLOCKSIZE];
            Read_rmdisk_block(ramdisk,clus+31,block);
            Entry entries[16];
            size_t entCnt=parseEntBlock(block,entries);
            for(size_t i=0;i<entCnt;++i)
                if(entnameEq(entname,entries[i].filename))
                    return entries[i];
        }
    }
    Entry entry;
    entry.first_cluster=-1;
    return entry;
}

unsigned short getNextClus(unsigned short clus)
{
    size_t offset = clus/2*3;
    unsigned short nextClus=0;
    if (clus % 2 == 0)
        nextClus = ((fat[offset + 1] & 0x0f) << 8) | fat[offset];
    else
        nextClus = (fat[offset + 2] << 4) | ((fat[offset + 1] >> 4) & 0x0f);
    return nextClus;
}
