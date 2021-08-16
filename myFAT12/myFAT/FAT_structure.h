#include <iostream>

using namespace std;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
const unsigned int BLOCKSIZE = 512;//in bytes
const unsigned int BLOCKCNT=2880;
const unsigned int SIZE=BLOCKSIZE*BLOCKCNT;
const unsigned int BYTEPERENT=32;
typedef struct BS_real
{
    u8 ignore[11];
    u16 byte_per_sector;
    u8 sec_per_cluster;
    u16 num_reserved_sector;
    u8 num_FATs;
    u16 max_num_dir_entries;
    u16 total_sec_cnt;
    u16 sec_per_FAT;
    u16 sec_per_track;
    u16 num_heads;
    u32 total_sec_cnt_FAT12;
    u8 boot_signature;
    u32 volume_id;
    char volume_label[11];
    char file_system_type[8];
}BS_real;

typedef union BootSector
{
    BS_real BS;
    unsigned char blockform[512];
}BootSector;

typedef struct Entry
{
    char filename[12];
    u8 attr;
    char reserved[2];
    u16 creation_time;//time类暂时都不需要实现
    u16 creation_date;
    u16 last_access_time;
    u8 ignore;
    u16 last_write_time;
    u16 last_write_date;
    u16 first_cluster;
    u32 file_size;//in bytes
}Entry;