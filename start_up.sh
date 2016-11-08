#启动tracker程序
sudo /usr/bin/fdfs_trackerd ./conf/tracker.conf restart
#启动storage
sudo /usr/bin/fdfs_storaged ./conf/storage.conf restart

#启动redis服务器
redis-server ./conf/redis.conf

#重新加载nginx
/usr/local/nginx/sbin/nginx -s reload
