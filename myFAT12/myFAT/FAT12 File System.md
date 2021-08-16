# FAT12 File System

https://zhuanlan.zhihu.com/p/122569192

https://blog.csdn.net/yxc135/article/details/8769086

https://reeeeeeeeeein.github.io/2020/04/15/fat12%E6%96%87%E4%BB%B6%E7%B3%BB%E7%BB%9F/

https://github.com/warpmatrix/fat12-file-system useful

https://blog.csdn.net/Laichilueng/article/details/54669370?utm_medium=distribute.pc_relevant_download.none-task-blog-2~default~searchFromBaidu~default-7.test_version_3&depth_1-utm_source=distribute.pc_relevant_download.none-task-blog-2~default~searchFromBaidu~default-7.test_version_



https://zhuanlan.zhihu.com/p/121807427 给出参数具体值

## DAY-3~1

> 出现负数是之前的时间一直在厘清思路，并没有很大的进展

大致框架：在main函数内初始化一个模拟磁盘的数据结构，作为FAT12文件系统，在支持输入和输出。目前给定的测试目标是存入文件并且读取。

由于使用的是内存的堆栈的空间，实际上在每次退出程序之后模拟文件系统中的内容都不会保存。阅读别人的代码以后，进行改进：把模拟磁盘的内容使用文件读写写进一个二进制文件（镜像），在程序运行的时候进行装载。

> 不要想着一口吃成胖子！可以先入手的是固定数据结构的实现

运行逻辑：main函数内接受指令，对模拟磁盘进行操作（模拟磁盘是一个大小与为1.44MB的char型数组，这就意味着以字节为单元），在程序结束时将该模拟磁盘数组的内容写入指定的二进制文件，在程序开始时反之装载二进制文件的内容到模拟磁盘数组。

对连续空间的操作：逐个字节搬运数据；在设计数据结构的时候使用union，使得结构同时具有字符数组的形式，通过对每个字节进行操作来完成读写盘。（ICS的pa中其实有类似的思路，文艺复兴！）

首先实现的读写操作如下：

```cc
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
```

进行简单的测试，头部变化较少，相对数据区更加简单，先装载头部进行测试：

![image-20210713152438835](C:\Users\ASUS\AppData\Roaming\Typora\typora-user-images\image-20210713152438835.png)

![image-20210713152455925](C:\Users\ASUS\AppData\Roaming\Typora\typora-user-images\image-20210713152455925.png)

在编写过程中首次独立编写简单的makefile，学习在脱离vs这类保姆式开发环境的情况下独立编译模块组合。

关于字符串的bug调了很久，const char*需要逐个赋值，后来被迫动手改造strcpy，注意strlen计算的是终止符之前的字符个数，所以未赋值的静态字符数组是无法通过strlen获得预定分配的空间的



优先实现具有代表性的个命令

+ mkdir: 创建文件夹 实际上包含再已有情况下新建和直接新建两种情况，对应着查找指定目录和新建目录两个任务
  + 分解路径，根据当前路径名找到对应的cluster，在当前cluster继续查找，直到无法找到为止（目标是支持建立不止一层新文件夹 即在只有./a的情况下mkdir ./a/b/c也支持）如果能找到说明已经存在
  + 在父文件夹cluster下补充信息，即新建新的entry，有必要就使用新的cluster（似乎应当交给函数封装实现）
  + 分配新的cluster给将要建立的子目录
+ touch 新建文件
  + 同上 分解路径 找到最近一级存在的父目录
  + 如果是需要建立目录，那么同上；最后一段被```/```分割的默认为文件名，无论是否有```.```做分割
+ edit 编辑文件
  + 根据文件路径 逐步拆分找到入口cluster并逐步遍历找到最后一个cluster
  + 从标准输入读入 存放在临时空间中，根据偏移量判断是否需要新的cluster来存放
+ cat 输出文件
  + 根据文件路径拆分找到入口cluster 一边遍历一边输出 直到计数器等于文件大小



实现过程：

0718

从mkdir开始实现 对位操作不太熟悉（之前ICS PA基础还不够扎实）对具体的模块也没有头绪 借鉴了代码 补齐了一些按照给定数据结构读写磁盘内容的基本操作，本质是把结构体内的成员根据字符串/数字逐个读取填写（即底层实现是两个parse数据结构的函数）

争取之后实现其他命令的时候把文字思路转换成伪代码，这样就能更好的确定各板块的共同需求 方便划分函数功能。

另外没有对新写的模块进行编译测试 磁盘FAT部分的初始化也没有完成 还有函数没有完成



0729 

花了很长时间写mkdir的指令，因为需要补充很多底层的指令。调试的时候发现段错误，于是用gdb调试；为了及时看到源码，对每一个模块都用了-g参数便于调试。

编译的时候因为typo， debug了很久，后来总是显示找不到新加入的cmd内的函数，检查发现makefile内编译主体程序的编译命令没有包含cmd.o

在认为有问题的函数打断点直接执行来缩小范围，后来怀疑是在parsePath的strcpy出现问题，因为一开始char entname[12] 编译没有通过改成了 char* entname，怀疑没有分配足够的空间导致栈溢出，于是改成用动态数组分配，问题解决

调试的时候遇到了输入第二个命令会 ```*** stack smashing detected ***: terminated Aborted (core dumped)``` 

0730

先把剩下的命令全部实现，其中在写edit命令的时候学到一个函数[**fget**](https://www.cplusplus.com/reference/cstdio/fgets/)的用法,支持逐个字节读入数据并写入磁盘

