//从/etc/fdfs/client.conf读取配置执行以下操作

//执行 fdfs_upload_file命令获取返回的id（id由用户分配空间）
int fdfs_upload_file(char *file_name,char *id);


//执行 fdfs_file_info命令获取fileid的info信息，info由用户开辟空间
int fdfs_file_info(char *id,char *info);


//执行 fdfs_file_info命令获取fileid的info信息中的ip，ip由用户开辟空间
int fdfs_file_ip(char *id,char *ip);
