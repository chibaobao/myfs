
CC=gcc
CPPFLAGS= -I./include  -I /usr/include/mysql
CFLAGS=-Wall 
LIBS= -lhiredis -lpthread -lfcgi -lm  -lmysqlclient
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
echo = ./test/echo
upload_file = ./test/upload_file
download_file = ./test/download_file
reg_cgi = ./test/reg_cgi
login_cgi = ./test/login_cgi
dao_mysql_test = ./test/dao_mysql_test 
cJSON_test = ./test/cJSON_test

target=$(fdfs_upload_file)	$(redis_op_test) $(echo) $(upload_file) $(fdfs_file_info) $(cJSON_test) $(download_file) $(reg_cgi) \
		$(dao_mysql_test) $(login_cgi)

ALL:$(target)


#生成所有的.o文件 将src的.o生成到当前目录，将test中的.o生成到test中
$(obj):%.o:%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS) 
$(obj_test):%.o:%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS) 


#fdfs_upload_file程序
$(fdfs_upload_file):./test/fdfs_upload_file.o  $(LOG)
	$(CC) $^ -o $@ $(LIBS)

#fdfs_file_info程序
$(fdfs_file_info):./test/fdfs_file_info.o  $(LOG)
	$(CC) $^ -o $@ $(LIBS)

#redis_op_test程序
$(redis_op_test):./test/redis_op_test.o   ./src/redis_op.o $(LOG)
	$(CC) $^ -o $@ $(LIBS)

#upload_file程序
$(upload_file):./test/upload_file.o   ./src/util_cgi.o  $(LOG) ./src/fdfs_op.o ./src/redis_op.o
	$(CC) $^ -o $@ $(LIBS)

#download_file程序
$(download_file):./test/download_file.o   ./src/util_cgi.o  $(LOG)  ./src/redis_op.o ./src/cJSON.o
	$(CC) $^ -o $@ $(LIBS)

#reg_cgi程序
$(reg_cgi):./test/reg_cgi.o	$(LOG)	./src/cJSON.o	./src/util_cgi.o  $(LOG)  ./src/redis_op.o ./src/dao_mysql.o
	$(CC) $^ -o $@ $(LIBS)  

#login_cgi程序
$(login_cgi):./test/login_cgi.o	$(LOG)	./src/cJSON.o	./src/util_cgi.o  $(LOG)  ./src/redis_op.o ./src/dao_mysql.o
	$(CC) $^ -o $@ $(LIBS)  

#echo程序
$(echo):./test/echo.o   
	$(CC) $^ -o $@ $(LIBS)

#cJSON_test 程序
$(cJSON_test):./test/cJSON_test.o   ./src/cJSON.o  
	$(CC) $^ -o $@ $(LIBS)

# dao_mysql_test程序
$(dao_mysql_test):./test/dao_mysql_test.o   ./src/dao_mysql.o 
	$(CC) $^ -o $@ $(LIBS)


#clean指令

clean:
	-rm -rf $(obj) $(obj_test) $(target)

distclean:
	-rm -rf $(obj) $(obj_test)

#将clean目标 改成一个虚拟符号
.PHONY: clean ALL distclean
