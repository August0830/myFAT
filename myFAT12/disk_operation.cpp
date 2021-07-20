#include "disk_operation.h"
#include <string.h>
using namespace std;

//将bootsector结构体中的数据写入缓存磁盘
void Write_rmdisk_bootsector(unsigned char* ramdisk,const BootSector* mbr)
{
    size_t len=sizeof(BootSector);
    for(size_t offset=0;offset<len;++offset)
        ramdisk[offset]=mbr->blockform[offset];
}

//从缓存磁盘中读出数据到bootsector结构体
void Read_rmdisk_bootsector(const unsigned char*ramdisk,BootSector*mbr)
{
    size_t len=sizeof(BootSector);
    for(size_t offset=0;offset<len;++offset)
        mbr->blockform[offset]=ramdisk[offset];
}

//从磁盘中指定块读出一个数据块
void Read_rmdisk_block(const unsigned char* ramdisk,unsigned int index,unsigned char* block)
{
    size_t base = index*BLOCKSIZE;
    for(size_t offset=0;offset<BLOCKSIZE;++offset)
        block[offset]=ramdisk[base+offset];
}

//将一个数据块写入到指定块
void Write_rmdisk_block(unsigned char* ramdisk,unsigned int index,const unsigned char*block)
{
    size_t base = index*BLOCKSIZE;
    for(size_t offset=0;offset<BLOCKSIZE;++offset)
        ramdisk[base+offset]=block[offset];
}

//从一个磁盘文件中读取数据到缓存磁盘中 返回成功从文件中读入的字节数
int Read_rmdisk(unsigned char* ramdisk,string diskname)
{
    FILE* fp=fopen(diskname.c_str(),"rb");
    if(!fp)
        return -1;
    size_t cnt=0;
    while(fscanf(fp,"%c",&ramdisk[cnt])!=EOF)
        cnt++;
    fclose(fp);
    return cnt;
}

//将缓存磁盘的数据写入到磁盘文件中 成功返回0 否则-1
int Write_rmdisk(unsigned char* ramdisk,string diskname)
{
    FILE* fp=fopen(diskname.c_str(),"wb");
    if(!fp)
        return -1;
    size_t cnt=0;
    for(size_t i=0;i<SIZE;++i)
        fprintf(fp,"%c",ramdisk[i]);
    fclose(fp);
    return 0;
}

void print_bootsector(const BootSector* mbr)
{
    cout<<"Info in boot sector\n";
    cout<<"bytes per sector: "<<mbr->BS.byte_per_sector<<endl;
    cout<<"sector per cluster:"<<mbr->BS.sec_per_cluster<<endl;
    cout<<"number of reserved_sector: "<<mbr->BS.num_reserved_sector<<endl;
    cout<<"number of FAT: "<<mbr->BS.num_FATs<<endl;
    cout<<"max number of directory entries: "<<mbr->BS.max_num_dir_entries<<endl;
    cout<<"total count of sector: "<<mbr->BS.total_sec_cnt<<endl;
    cout<<"sector per FAT: "<<mbr->BS.sec_per_FAT<<endl;
    cout<<"sector per track: "<<mbr->BS.sec_per_track<<endl;
    cout<<"number of heads: "<<mbr->BS.num_heads<<endl;
    cout<<"total sector count of FAT12: "<<mbr->BS.total_sec_cnt_FAT12<<endl;
    cout<<"boot signature: "<<mbr->BS.boot_signature<<endl;
    cout<<"volume id: "<<mbr->BS.volume_id<<endl;
    cout<<"volume label:"<<mbr->BS.volume_label<<endl;
    cout<<"file system: "<<mbr->BS.file_system_type<<endl;
}

void mystrcpy(char* dst,size_t len,char* src)
{
    //cout<<"dst "<<dst<<endl;
   // cout<<"src "<<src<<endl;
    if(strlen(src)+1>len)
    {
        //cout<<"no\n";
        return;
    }    
    int i=0;
    while(*src!='\0')
    {
        dst[i]=*src;
        src++;
        i++;
    }
    dst[i]='\0';
    //cout<<"new dst "<<dst<<endl;
}


char* strsep(char**strp,const char*delim)
{
    char* tok=*str;
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
}