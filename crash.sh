# скрипт, который будет запускать программу блокировки
# а потом удалять/портить lck файл.

./prog -f/tmp/myfile.lck
#sleep 10
rm /tmp/myfile.lck
