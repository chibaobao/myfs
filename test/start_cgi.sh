spawn-fcgi -a 127.0.0.1 -p 8011 -f ./echo
spawn-fcgi -a 127.0.0.1 -p 8012 -f ./upload_file
spawn-fcgi -a 127.0.0.1 -p 8013 -f ./download_file
