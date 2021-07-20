#include <stdio.h>
#include <dirent.h>
#include <sys/types.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <stack>
#include <map>
#include <iostream>
#include <vector>
using namespace std;
int main()
{
    string dir_name;
    //getcwd(dir_name,300);
    //scanf("%s",dir_name);
    //printf("current directory: %s\n",dir_name);
    cin>>dir_name;
    
    stack<string> stk;
    stk.push(dir_name);
    multimap<ino_t,string> dict;
    struct dirent *stdinfo;
    struct stat state;
    while(!stk.empty())
    {
        cout<<"current directory: "<<dir_name<<endl;
        DIR *dir=NULL;
        dir=opendir(dir_name.c_str());
        if(!dir)
        {
            printf("failed to open directory\n");
            return 0;
        }
        while(1)
        {
            if((stdinfo=readdir(dir))==0)
            {
                closedir(dir);
                break;
            }   
            printf("ilink %lu ",state.st_nlink);
            printf("inode %lu ",state.st_ino);
            printf("name %s,type =%d\n",stdinfo->d_name,stdinfo->d_type);
                       
            if(stdinfo->d_type == 4)
                stk.push(stdinfo->d_name);
            else if(stdinfo->d_type == 8)
            {
                stat(stdinfo->d_name,&state);
                if(state.st_nlink>=2)
                {
                    printf("add %lu %s",state.st_ino,stdinfo->d_name);
                    dict.insert(pair<ino_t,string>(state.st_ino,stdinfo->d_name));
                }
            }
            
        }
        dir_name=stk.top();
        cout<<dir_name<<endl;
        stk.pop();
        while(dir_name!=".git" && !stk.empty())
        {
            dir_name=stk.top();
            stk.pop();
        }
        
    }
    multimap<ino_t,string>::iterator it = dict.begin();
    for(;it!=dict.end();++it)
    {
        cout<<it->first<<" "<<it->second<<endl;
    }
    
    return 0;
}