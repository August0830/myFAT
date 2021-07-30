#include "entry.h"
#include "stdlib.h"
#include <iostream>
#include <string.h>
using namespace std;
static unsigned char fat[BLOCKSIZE*9];//FAT table

/*char* strsep(char**strp,const char*delim)
{
    char* tok=*strp;
    if(tok==NULL)
        return NULL;
    for(char* s=tok;*s!='\0';++s)
    {
        for(const char* spdelim=delim;*spdelim!='\0';++spdelim)
        {
            if(*s==*spdelim)
            {
                *s='\0';
                *strp=s+1;//完成切割
                return tok;
            }
        }
    }
    *strp=NULL;
    return tok;//被分解出的token
}

char* strdup(const char*s)
{
    char *newstr = (char*)malloc((strlen(s)+1)*sizeof(char));
    return strcpy(newstr,s);
}*/

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

void parseEntCharStr(const char* ch,unsigned char* entStr,size_t base,size_t len)
{
    for(size_t offset=0;offset<len;++offset)
        entStr[base+offset]=ch[offset];
}

void parseEntNum(unsigned int entNum,unsigned char* entStr,size_t base,size_t len)
{
    for(size_t offset=0;offset<len;++offset)
    {
        entStr[base+offset]=entNum&0xff;
        entNum=entNum>>8;
    }      
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

void parseEnt(const Entry *entry,unsigned char* entStr)
{
    parseEntCharStr(entry->filename,entStr,0,11);
    parseEntNum(entry->attr,entStr,11,1);
    parseEntCharStr(entry->reserved,entStr,12,2);
    parseEntNum(entry->first_cluster,entStr,26,2);
    parseEntNum(entry->file_size,entStr,28,4);
}

void parseEnts(unsigned char *block,const Entry *entries,size_t ent_cnt)
{
    for(size_t i=0;i<ent_cnt;++i)
    {
        parseEnt(&entries[i],block+BYTEPERENT*i);
    }
    memset(block+BYTEPERENT*ent_cnt,0,(16-ent_cnt)*BYTEPERENT);
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

Entry setEnt(const char* entname,unsigned char attr,unsigned short firstclus,unsigned int filesize)
{
    Entry entry;    
    parseEntName(entname,entry.filename);
    entry.attr=attr;
    memset(entry.reserved,0,10);
    entry.first_cluster=firstclus;
    entry.file_size=filesize;
    return entry;
}

size_t getDirFreeEnt(size_t *block_idx,unsigned short dir_clus,unsigned char* ramdisk)
{
    if(dir_clus==0)
    {
        for(size_t base_sec=19,sec_ofset=0;sec_ofset<14;sec_ofset++)
        {
            unsigned char block[BLOCKSIZE];
            Read_rmdisk_block(ramdisk,base_sec+sec_ofset,block);
            size_t free_ent_idx=getFreeEntIdx(block);
            if(free_ent_idx!=(size_t)-1)
            {
                *block_idx=base_sec+sec_ofset;
                return free_ent_idx;
            }
        }
        return -1;
    }
    for(unsigned short clus = dir_clus;clus!=0xff;
    clus=getNextClus(clus))
    {
        unsigned char block[BLOCKSIZE];
        Read_rmdisk_block(ramdisk,31+clus,block);
        size_t free_ent_idx=getFreeEntIdx(block);
        if(free_ent_idx!=(size_t)-1)
        {
            *block_idx=(size_t)(31+clus);
            return free_ent_idx;
        }
    }
    //本block没有找到 申请新的块
    unsigned short new_next_clus = getFreeClus();
    if(new_next_clus==(unsigned short)-1)
        return -2;
    unsigned char block[BLOCKSIZE];
    Read_rmdisk_block(ramdisk,31+new_next_clus,block);
    memset(block,0,BLOCKSIZE);
    Write_rmdisk_block(ramdisk,31+new_next_clus,block);
    *block_idx = 31+new_next_clus;
    addEntClus(dir_clus,new_next_clus);
    return 0;
}

size_t getFreeEntIdx(const unsigned char *block)
{
    for(size_t i=0;i<16;++i)
    {
        unsigned char fst_byte = block[BYTEPERENT*i];
        if(fst_byte == 0xe5 || fst_byte==0)
            return i;
    }
    return -1;
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

Entry getEntryByClus(u16 ent_clus,u16 dir_clus,const unsigned char*ramdisk)
{
    if(dir_clus==0)//在根目录下读取的block的计算方式略有不同
    {
        for(size_t baseSec=19,secOfst=0;secOfst<14;secOfst++)
        {
            unsigned char block[BLOCKSIZE];
            Read_rmdisk_block(ramdisk,baseSec+secOfst,block);
            Entry entries[16];
            size_t entCnt=parseEntBlock(block,entries);
            //读出该块内所有entry
            for(size_t i=0;i<entCnt;++i)
                if(entries[i].first_cluster==ent_clus)
                    return entries[i];
        }
        Entry entry;
        entry.first_cluster=-1;
        return entry;
    }
    else
    {
        for(unsigned short clus = dir_clus;clus!=ENT_LASTCLU;
        clus=getNextClus(clus))
        {
            unsigned char block[BLOCKSIZE];
            Read_rmdisk_block(ramdisk,clus+31,block);
            Entry entries[16];
            size_t entCnt=parseEntBlock(block,entries);
            for(size_t i=0;i<entCnt;++i)
                if(entries[i].first_cluster==ent_clus)
                    return entries[i];
        }
    }
    Entry entry;
    entry.first_cluster=-1;
    return entry;
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

size_t getEntIdx(u16 ent_clus,const unsigned char*block)
{
    for(size_t i=0;i<16;++i)
    {
        Entry entry = parseEntStr(block+i*BYTEPERENT);
        if(entry.first_cluster==ent_clus)
            return i;
    }
    return -1;
}

size_t getDirEnt(size_t *block_idx,u16 ent_clus,u16 dir_clus,const unsigned char* ramdisk)
{
    if(dir_clus==0)
    {
        for(size_t baseSec=19,sec_offset=0;sec_offset<14;sec_offset++)
        {
            unsigned char block[BLOCKSIZE];
            Read_rmdisk_block(ramdisk,baseSec+sec_offset,block);
            size_t ent_idx = getEntIdx(ent_clus,block);
            if(ent_idx!=(size_t)-1)
            {
                *block_idx=baseSec+sec_offset;
                return ent_idx;
            }
        }
        return -1;
    }
    for(u16 clus = dir_clus;clus!=0xfff;clus=getNextClus(clus))
    {
        unsigned char block[BLOCKSIZE];
        Read_rmdisk_block(ramdisk,clus+31,block);
        size_t ent_idx = getEntIdx(ent_clus,block);
        if(ent_idx!=(size_t)-1)
        {
            *block_idx=31+clus;
            return ent_idx;
        }
    }
    return -1;
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

void addEntClus(unsigned short frst_clus,unsigned short new_clus)
{
    if(getNextClus(frst_clus)==0)
    {
        setFatClus(frst_clus,0xfff);
        return;
    }
    unsigned short end_clus = frst_clus;
    for(unsigned short next_clus=getNextClus(frst_clus);next_clus!=0xfff;
    end_clus=frst_clus,frst_clus=getNextClus(next_clus))
    {
        setFatClus(new_clus,0xfff);
        setFatClus(end_clus,new_clus);
    }
}

void setFatClus(unsigned short clus, unsigned short newClus)
{
    unsigned short offset = clus / 2 * 3;
    if (clus % 2 == 0) 
    {
        fat[offset + 1] = (fat[offset + 1] & 0xf0) | ((newClus & 0xf00) >> 8);
        fat[offset] = newClus & 0x0ff;
    } 
    else 
    {
        fat[offset + 1] = ((newClus & 0x00f) << 4) | (fat[offset + 1] & 0x00f);
        fat[offset + 2] = (newClus & 0xff0) >> 4;
    }
}

unsigned short getFreeClus()
{
    for(unsigned short clus=2;clus<BLOCKCNT-31;clus++)
    {
        if(getNextClus(clus)==0)
            return clus;
    }
    return -1;
}

