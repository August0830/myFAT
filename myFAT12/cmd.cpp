#include "cmd.h"
using namespace std;
char* getPathEntname(const char* path,char*entname)
{
    char delim[]="/";
    char* pathcopy = strdup(oath);
    const char *partpath = strsep(&pathcopy,delim);
    while(pathcopy)
        partpath=strsep(&pathcopy,delim);
    strcpy(entname,partpath);
    return entname;
}

size_t getDirFreeEnt()

unsigned short getFreeClus()

Entry getEnt(const char* entname,unsigned char attr,unsigned short firstclus,unsigned int filesize)


void parseEnt()

int mkdirent(const char* entname,u16 dirClus,unsigned char*ramdisk)
{
    size_t block_idx,ent_idx;
    ent_idx=getDirFreeEnt(&block_indx,dirClus,ramdisk);
    if(ent_idx==(size_t)-1)
        return ROOT_IS_FULL;
    if(block_idx==(size_t)-2)
        return DISK_IS_FULL;
    unsigned short new_ent_clus = getFreeClus();
    if(new_ent_clus==(unsigned short)-1)
        return DISK_IS_FULL;
    
    unsigned char block[BLOCKSIZE];
    Read_rmdisk_block(ramdisk,block_idx,block);
    Entry entry = getEnt(entname,DIR_ATTR,new_ent_clus,0);
    //文件夹的文件大小认为是0
    parseEnt(&)
}


int mkdircmd(u16 clus,const char* path,unsigned char* ramdisk)
{
    if(!path)
        return MISS_OPERAND;
    u16 dirClus = clus;
    u16 entclus = parsePath(&dirClus,path,ramdisk);
    if(dirClus==(unsigned short)-1) 
        return NO_FILE_DIR;
    if(entclus!=(unsigned short)-1)
        return FILE_DIR_XST;
    char entname[12];
    entname = getPathEntname(path,entname);
    int res = mkdirent(entname,dirClus,ramdisk);
    if(res==ROOT_IS_FULL)
        return ROOT_IS_FULL;
    if(res==DISK_IS_FULL)
        return DISK_IS_FULL;
    return 0;
}