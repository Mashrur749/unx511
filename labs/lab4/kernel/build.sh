echo "unload"
sudo ./unload.sh
echo "make"
make clean all
echo "load"
sudo ./load.sh
tail -f /var/log/syslog
