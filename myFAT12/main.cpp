//#include "disk_operation.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include "cmd.h"
using namespace std;
int main()
{
    unsigned char ramdisk[SIZE];
    string cmd;
    cout<<"myFat:";
    //cin>>cmd;
    getline(cin,cmd);
    vector<string> cmdline;
    while(cmd!="q"&&cmd!="quit")
    {
        if(cmd=="mknwdisk")
        {
            cout<<"Name the new disk:";
            string diskname;
            cin>>diskname;
            FILE*fp=fopen(diskname.c_str(),"wb");
            if(!fp)
            {
                cout<<"Fail to create a new disk!\n";
                return 0;
            }    
            fclose(fp);
            BootSector* mbr = new BootSector;
            mbr->BS.byte_per_sector=512;
            mbr->BS.sec_per_cluster=(char)1;
            mbr->BS.num_reserved_sector=0x1;
            mbr->BS.num_FATs=(char)0x2;
            mbr->BS.max_num_dir_entries=224;
            mbr->BS.total_sec_cnt=19;
            mbr->BS.sec_per_FAT=9;
            mbr->BS.sec_per_track=0;
            mbr->BS.num_heads=0;
            mbr->BS.total_sec_cnt_FAT12=0;
            mbr->BS.boot_signature=0;
            mbr->BS.volume_id=0;
            mystrcpy(mbr->BS.file_system_type,8,"FAT12 ");
            //初始化ramDisk中的BOOTSECTOR 然后再写入该文件，
            //可以在完成一系列操作之后 在退出程序前写入 
            print_bootsector(mbr);
            Write_rmdisk_bootsector(ramdisk,mbr);
            int err=Write_rmdisk(ramdisk,diskname);      
            if(err==0)
                cout<<"Write the new disk successfully!\n";
        }
        else if(cmd=="rddisk")
        {
            cout<<"Pick the disk you wanna read:";
            string diskname;
            cin>>diskname;
            int err=Read_rmdisk(ramdisk,diskname);
            if(err==SIZE)
            {
                BootSector* check_mbr=new BootSector;
                Read_rmdisk_bootsector(ramdisk,check_mbr);
                print_bootsector(check_mbr);
            }
        }
        else
        {
            stringstream ssin(cmd);   
            string tmp;
            while(ssin>>tmp)
            {
                cmdline.push_back(tmp);
            }
            for(int i=0;i<cmdline.size();++i)
                cout<<cmdline[i]<<"*";
            if(cmdline.size()!=2)
            {
                cout<<"Wrong command!\n";
                cmdline.clear();
            } 
            else
            {
                if (cmdline[0]=="mkdir");
                {
                    int res = mkdircmd(31,cmdline[1].c_str(),ramdisk);
                    if (res == ROOT_IS_FULL)
                        cout<<"Fail. Root is full.\n";
                    else if(res == DISK_IS_FULL)
                        cout<<"Fail. Disk is full.\n";
                    if(res==0)
                        cout<<"Suceed.\n";
                }  
                if(cmdline[0]=="touch")
                {
                    int res = touchcmd(31,cmdline[1].c_str(),ramdisk);
                    if (res == ROOT_IS_FULL)
                        cout<<"Fail. Root is full.\n";
                    else if(res == DISK_IS_FULL)
                        cout<<"Fail. Disk is full.\n";
                    if(res==0)
                        cout<<"Suceed.\n";
                }  
            }   
        }
        //cout<<cmd<<endl;
        cmdline.clear();
        cout<<"myFat:";
        getline(cin,cmd);
    }
    
    return 0;
}