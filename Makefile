
CC=gcc
CPPFLAGS= -I./include  
CFLAGS=-Wall 
LIBS= -lhiredis -lpthread -lfcgi
LOG = ./src/make_log.o
#找到src和test目录下所有的.c文件
src = $(wildcard ./src/*.c)
test = $(wildcard ./test/*.c)

#将src中的.c生成.o到src目录
obj = $(patsubst %.c, %.o, $(src))
#将test中的.c生成.o到test目录
obj_test = $(patsubst %.c, %.o, $(test))

#生成test目标
fdfs_upload_file = ./test/fdfs_upload_file
fdfs_file_info = ./test/fdfs_file_info
redis_op_test = ./test/redis_op_test
myecho = ./test/myecho

target=$(fdfs_upload_file)	$(redis_op_test) $(myecho) $(fdfs_file_info)

ALL:$(target)


#生成所有的.o文件 将src的.o生成到当前目录，将test中的.o生成到test中
$(obj):%.o:%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS) 
	echo $(obj)
$(obj_test):%.o:%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS) 


#fdfs_upload_file程序
$(fdfs_upload_file):./test/fdfs_upload_file.o  $(LOG)
	$(CC) $^ -o $@ $(LIBS)

#fdfs_upload_file程序
$(fdfs_file_info):./test/fdfs_file_info.o  $(LOG)
	$(CC) $^ -o $@ $(LIBS)

#redis_op_test程序
$(redis_op_test):./test/redis_op_test.o   ./src/redis_op.o $(LOG)
	$(CC) $^ -o $@ $(LIBS)

#myecho程序
$(myecho):./test/myecho.o   ./src/util_cgi.o  $(LOG) ./src/fdfs_op.o ./src/redis_op.o
	$(CC) $^ -o $@ $(LIBS)


#clean指令

clean:
	-rm -rf $(obj) $(obj_test) $(target)

distclean:
	-rm -rf $(obj) $(obj_test)

#将clean目标 改成一个虚拟符号
.PHONY: clean ALL distclean
