# 分布式存储系统

## 文件结构

- src基础组件和基础模块目录
- include基础组件的头文件，和其他需要的头文件
- test测试目录，该文件夹下.c都可以被makfile编译执行，是src相关组件的测试程序和部分服务器主程序
- conf存放fdfs和redis的配置文件
- lib部分动态库
- bin存放部分这执行的服务器程序

## 各文件功能

- 主目录
 - start_up.sh启动redis以及fdfs的tracker和storage
- src，include和test
 - make_log.c test_log.c日志程序和测试文件（会向当前文件log文件夹打印日志）
 - redis_op.c redis_op.h对redis接口的浅封装 redis_op_test.c是相应测试程序部分测试
 - util_cgi.c util_cgi.h 字符串处理基础组件
 - fdfs_upload_file.c  fdfs_upload_file.h实现fdfs_upload_file命令相同功能的函数封装test目录下的是测试程序
 - myecho.c处理http请求的函数，集成了获取文件数据流，解析文件数据流，并存储文件数据流
 - upload_file模块处理上传页面的各种请求
 - download_file处理下载页面（cloud_file.html）各种请求
 - reg_cgi处理注册页面的请求
 - login_cgi处理登录页面的各种请求

## 环境配置
- fastdfs
- redis
- negix
- fastdfs-nginx-module
    - 编译nginx时，./congure之后，修改nginx-1.10.1/objs/Makefile中的- I找到所缺.h目录添加就可以

## 未解决问题
- 下载页面刷新后无法重新获取数据，将导致前端按钮实现
- 分享页面允许重复分享
