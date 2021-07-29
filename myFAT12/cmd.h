#include "entry.h"
#include <iostream>

#define MISS_OPERAND 1
#define NO_FILE_DIR 2
#define FILE_DIR_XST 3
#define ROOT_IS_FULL 4
#define DISK_IS_FULL 5
#define NOT_RM_PRES_DIR 6
#define INVALID_ARGUMET 7
#define IS_DIRECTORY 8
#define DIR_NOT_EMPTY 9
#define FILE_ERROR 10
#define OPER_NOT_PERM 11
#define MISS_DEST_FILE 12

#define CMD_NOT_FOUND -1
#define DIR_HAS_DOT -2
#define SOURCE_IS_DIR -3

//分解路径名找到希望建立的文件夹的名字（在最后）
char* getPathEntname(const char* path,char*entname);
//建立文件夹 会修改FAT内容
int mkdirent(const char* entname,u16 dirClus,unsigned char*ramdisk);
//实现mkdir的命令
int mkdircmd(u16 clus,const char* path,unsigned char* ramdisk);
//建立文件 修改FAT中内容
int mkent(const char* entname,u16 dirClus,unsigned char*ramdisk);
//实现touch指令
int touchcmd(u16 clus,const char*path,unsigned char*ramdisk);