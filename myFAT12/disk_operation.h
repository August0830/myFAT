#include <iostream>
#include <stdio.h>
#include "FAT_structure.h"
#include <string.h>

//将bootsector结构体中的数据写入缓存磁盘
void Write_rmdisk_bootsector(unsigned char* ramdisk,const BootSector* mbr);
//从缓存磁盘中读出数据到bootsector结构体
void Read_rmdisk_bootsector(const unsigned char*ramdisk,BootSector*mbr);
//从磁盘中指定块读出一个数据块
void Read_rmdisk_block(const unsigned char* ramdisk,unsigned int index,unsigned char* block);
//将一个数据块写入到指定块
void Write_rmdisk_block(unsigned char* ramdisk,unsigned int index,const unsigned char*block);
//将缓存磁盘的数据写入到磁盘文件中 成功返回0 否则-1
int Write_rmdisk(unsigned char* ramdisk,string diskname);
//从一个磁盘文件中读取数据到缓存磁盘中 返回成功从文件中读入的字节数
int Read_rmdisk(unsigned char* ramdisk,string diskname);
//打印bootsector的信息
void print_bootsector(const BootSector* mbr);


void mystrcpy(char* dst,size_t len,char* src);
//以delim为分隔符分割 返回token，
//同时原字符串指针后移到分隔符后
char* strsep(char**strp,const char*delim);
//strcpy同样功能
char* strdup(const char*s);

