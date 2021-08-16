#include "cmd.h"
#include <stdlib.h>
using namespace std;

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

char* getPathEntname(const char* path,char*entname)
{
    char delim[]="/";
    char* pathcopy = strdup(path);
    const char *partpath = strsep(&pathcopy,delim);
    while(pathcopy)
        partpath=strsep(&pathcopy,delim);
    strcpy(entname,partpath);
    return entname;
}


int mkdirent(const char* entname,u16 dirClus,unsigned char*ramdisk)
{
    size_t block_idx,ent_idx;
    ent_idx=getDirFreeEnt(&block_idx,dirClus,ramdisk);
    if(ent_idx==(size_t)-1)
        return ROOT_IS_FULL;
    if(block_idx==(size_t)-2)
        return DISK_IS_FULL;
    unsigned short new_ent_clus = getFreeClus();
    if(new_ent_clus==(unsigned short)-1)
        return DISK_IS_FULL;
    
    //把文件夹的信息填写在父级目录的block entries中
    unsigned char block[BLOCKSIZE];
    Read_rmdisk_block(ramdisk,block_idx,block);
    Entry entry = setEnt(entname,DIR_ATTR,new_ent_clus,0);
    //文件夹的文件大小认为是0
    parseEnt(&entry,block+ent_idx*BLOCKSIZE);
    Write_rmdisk_block(ramdisk,block_idx,block);

    //建立自己的block entries
    Entry entries[2];
    entries[0] = setEnt(".",DIR_ATTR,new_ent_clus,0);
    entries[1]=setEnt("..",DIR_ATTR,dirClus,0);
    Read_rmdisk_block(ramdisk,31+new_ent_clus,block);
    parseEnts(block,entries,2);
    Write_rmdisk_block(ramdisk,31+new_ent_clus,block);
    addEntClus(new_ent_clus,new_ent_clus);//对FAT做更新
}


int mkdircmd(u16 clus,const char* path,unsigned char* ramdisk)
{
    if(!path)
        return MISS_OPERAND;
    u16 dirClus = clus;
    u16 entclus = parsePath(&dirClus,path,ramdisk);
    if(dirClus==(unsigned short)-1) 
    {
        cout<<"The upper directory does not exist\n";
        return NO_FILE_DIR;
    }
    if(entclus!=(unsigned short)-1)
    {
        cout<<"The directory has already existed\n";
        return FILE_DIR_XST;
    }
    char* entname = new char[12];//debug
    entname = getPathEntname(path,entname);
    int res = mkdirent(entname,dirClus,ramdisk);
    if(res==ROOT_IS_FULL)
        return ROOT_IS_FULL;
    if(res==DISK_IS_FULL)
        return DISK_IS_FULL;
    return 0;
}

int mkent(const char* entname,u16 dirClus,unsigned char*ramdisk)
{
    size_t block_idx,ent_idx;
    ent_idx = getDirFreeEnt(&block_idx,dirClus,ramdisk);
    if(ent_idx==(size_t)-1)
        return ROOT_IS_FULL;
    if(block_idx==(size_t)-2)
        return DISK_IS_FULL;
    unsigned short new_ent_clus = getFreeClus();
    if(new_ent_clus==(unsigned short)-1)
        return DISK_IS_FULL;

    unsigned char block[BLOCKSIZE];
    Read_rmdisk_block(ramdisk,block_idx,block);
    Entry entry = setEnt(entname,FILE_ATTR,new_ent_clus,0);
    //文件夹的文件大小认为是0
    parseEnt(&entry,block+ent_idx*BLOCKSIZE);
    Write_rmdisk_block(ramdisk,block_idx,block);
    addEntClus(new_ent_clus,new_ent_clus);
    return 0;
}

int touchcmd(u16 clus,const char*path,unsigned char*ramdisk)
{
    if(!path)
        return MISS_OPERAND;
    u16 dir_clus = clus;
    u16 ent_clus = parsePath(&dir_clus,path,ramdisk);
    if(dir_clus == (unsigned short)-1)
        return NO_FILE_DIR;
    if(ent_clus !=(unsigned short)-1)      
        return FILE_DIR_XST;
    char* entname = new char[12];
    entname = getPathEntname(path,entname);
    int res = mkent(entname,dir_clus,ramdisk);
    if(res == -1)
        return ROOT_IS_FULL;
    if(res == -2)
        return DISK_IS_FULL;
    return 0;
}

int dispFile(Entry* file_entry,const unsigned char* ramdisk)
{
    size_t byts_cnt;
    for(u16 clus = file_entry->first_cluster;clus!=0xfff;clus=getNextClus(clus))
    {
        unsigned char block[BLOCKSIZE];
        Read_rmdisk_block(ramdisk,31+clus,block);
        for(size_t offset=0;offset<BLOCKSIZE;offset++,byts_cnt++)
        {
            if(byts_cnt == file_entry->file_size)
            {
                if(getNextClus(clus)!=0xfff)
                    return -1;
                break;
            }
            printf("%c",block[offset]);
        }
    }
    return 0;
}

int catcmd(u16 clus,const char* path,unsigned char* ramdisk)
{
    if(!path)
        return MISS_OPERAND;
    u16 dir_clus=clus;
    u16 ent_clus = parsePath(&dir_clus,path,ramdisk);
    if(ent_clus == (unsigned short)-1)
        return NO_FILE_DIR;
    Entry entry = getEntryByClus(ent_clus,dir_clus,ramdisk);
    if(entry.attr==DIR_ATTR)
        return IS_DIR;
    int res = dispFile(&entry,ramdisk);
    if(res == -1)
        return FILE_ERROR;
    return 0;
}

int editFile(Entry* file_entry,u16 dir_clus, unsigned char* ramdisk)
{
    int res = dispFile(file_entry,ramdisk);
    if(res ==-1)
        return -1;
    u16 clus = file_entry->first_cluster;
    while(getNextClus(clus)!=0xfff)
        clus = getNextClus(clus);
    unsigned char* block = new unsigned char[BLOCKSIZE];
    Read_rmdisk_block(ramdisk,31+clus,block);
    int offset = file_entry->file_size%BLOCKSIZE;//零头

    const size_t LINELEN=16;
    char line[LINELEN];
    while(fgets(line,LINELEN,stdin)!=NULL)
    {
        size_t len = strlen(line);
        for(size_t i=0;i<len;++i)
        {
            block[offset+i]=line[i];
            if(i+offset == BLOCKSIZE-1)
            {
                Write_rmdisk_block(ramdisk,31+clus,block);
                u16 new_clus = getFreeClus();
                if(new_clus == (unsigned short)-1)
                    return -2;
                addEntClus(file_entry->first_cluster,new_clus);
                clus = new_clus;
                Read_rmdisk_block(ramdisk,31+clus,block);
                offset -=BLOCKSIZE;
            }
        }
        file_entry->file_size+=len;
        offset = file_entry->file_size%BLOCKSIZE;
    }
    Write_rmdisk_block(ramdisk,31+clus,block);

    size_t block_idx,ent_idx;
    ent_idx = getDirEnt(&block_idx,file_entry->first_cluster,dir_clus,ramdisk);
    if(ent_idx == (size_t)-1)
        return -1;
    Read_rmdisk_block(ramdisk,block_idx,block);
    parseEnt(file_entry,block+ent_idx*BYTEPERENT);
    Write_rmdisk_block(ramdisk,block_idx,block);
    return 0;
}

int editcmd(u16 clus,const char* path,unsigned char* ramdisk)
{
    if(!path)
        return MISS_OPERAND;
    u16 dir_clus = clus;
    u16 ent_clus = parsePath(&dir_clus,path,ramdisk);
    if(ent_clus == (unsigned short)-1)
        return NO_FILE_DIR;
    Entry entry = getEntryByClus(ent_clus,dir_clus,ramdisk);
    if(entry.attr == DIR_ATTR)
        return IS_DIR;

    int res = editFile(&entry,dir_clus,ramdisk);
    
    if(res == -1)
        return FILE_ERROR;
    if(res == -2)
        return DISK_IS_FULL;
    return 0;
}

void ResInfo(int res)
{
    if (res == ROOT_IS_FULL)
        cout<<"Fail. Root is full.\n";
    if(res == DISK_IS_FULL)
        cout<<"Fail. Disk is full.\n";
    if(res==0)
        cout<<"Suceed.\n";
    if(res == FILE_DIR_XST)
        cout<<"The target has already existed\n";
    if(res == NO_FILE_DIR)
        cout<<"The directory does not exist\n";
    if(res == IS_DIR)
        cout<<"This is directory\n";
    if(res == FILE_ERROR)
        cout<<"File error.\n";
}