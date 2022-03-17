CC = gcc
TARGET = libcJSON.so

cJSON 		= 	./%.c


cJSON_O 		=	cJSON.o 

#	-fPIC是创建与地址无关的编译程序，是为了能够在多个应用程序间共享
#	-shared是指定生成动态链接库
#	LD_LIBRARY_PATH=. 指定先在当前路径寻找链接的动态库.链接动态库时，可以加上一个"-Wl,--rpath=$(DIR)" 选项，可用于指定程序运行时，首先查找的动态库路径。	
	

all:$ $(cJSON_O) 
	$(CC) $^ -o $(TARGET)  -fPIC -shared


%.o :$(cJSON)
	$(CC) -c $<  

clean:
	rm $(TARGET) -rf;
	rm ./*.o -rf;
	
install:
	chmod 777 $(TARGET)
	rm $(TARGET) -rf
