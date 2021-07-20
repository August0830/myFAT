myFAT:disk_operation.o main.cpp FAT_structure.h entry.o
	g++ main.cpp disk_operation.o entry.o FAT_structure.h -o myFAT

entry.o: entry.h entry.cpp disk_operation.h
	g++ -c entry.h disk_operation.h entry.cpp 

disk_operation.o:disk_operation.h disk_operation.cpp
	g++ -c disk_operation.cpp disk_operation.h

clean:
	rm -rf disk_operation.o 
	rm -rf entry.o
	rm -rf myFAT