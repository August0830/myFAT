myFAT:disk_operation.o main.cpp FAT_structure.h entry.o cmd.o
	g++ -g main.cpp disk_operation.o entry.o FAT_structure.h cmd.o -o myFAT

cmd.o:cmd.cpp cmd.h entry.h
	g++ -g -std=c++98 -c cmd.cpp cmd.h entry.h  

entry.o: entry.h entry.cpp disk_operation.h
	g++ -g -std=c++98 -c entry.h disk_operation.h entry.cpp 

disk_operation.o:disk_operation.h disk_operation.cpp
	g++ -g -std=c++98 -c disk_operation.cpp disk_operation.h

clean:
	rm -rf disk_operation.o 
	rm -rf entry.o
	rm -rf myFAT