make > make_message.txt

# Зададим полный путь к конфигурационному файлу
./prog -f/tmp/myfile.lck

a1=0;
a2=0;
a3=0;
a4=0;
a5=0;
a6=0;
a7=0;
a8=0;
a9=0;
a10=0;

( trap 'echo "caught signal"' SIGINT;  grep -rqa shazam /usr; exit) &
PID=$!

echo "" >stat.txt
exec 200>/tmp/myfile.lck  # open file and link file descriptor 200 to it
set -e            # die on errors

sleep 1 && while [ 1 ]
do
  flock -n 200      # acquire a lock
  echo lock
  a1=$((a1+1));
  echo Задача1_Ко-во_блокировок_$a1 >>stat.txt
  sleep 1
  echo Hello_from_1
  echo unlock
  flock -u -n 200   # release the lock
done &

sleep 1 &&  while [ 1 ]
do
  flock -n 200      # acquire a lock
  echo lock
  a2=$((a2+1));
  echo Задача2_Ко-во_блокировок_$a2 >>stat.txt
  sleep 1
  echo Hello_from_2
  echo unlock
  flock -u -n 200   # release the lock
done &


sleep 1 &&  while [ 1 ]
do
  flock -n 200      # acquire a lock
  echo lock
  a3=$((a3+1));
  echo Задача3_Ко-во_блокировок_$a3 >>stat.txt
  sleep 1
  echo Hello_from_3
  echo unlock
  flock -u -n 200   # release the lock
done &

sleep 1 &&  while [ 1 ]
do
  flock -n 200      # acquire a lock
  echo lock
  a4=$((a4+1));
  echo Задача4_Ко-во_блокировок_$a4 >>stat.txt
  sleep 1
  echo Hello_from_4
  echo unlock
  flock -u -n 200   # release the lock
done &

sleep 1 &&  while [ 1 ]
do
  flock -n 200      # acquire a lock
  echo lock
  a5=$((a5+1));
  echo Задача5_Ко-во_блокировок_$a5 >>stat.txt
  sleep 1
  echo Hello_from_5
  echo unlock
  flock -u -n 200   # release the lock
done &

sleep 1 &&  while [ 1 ]
do
  flock -n 200      # acquire a lock
  echo lock
  a6=$((a6+1));
  echo Задача6_Ко-во_блокировок_$a6 >>stat.txt
  sleep 1
  echo Hello_from_6
  echo unlock
  flock -u -n 200   # release the lock
done &

sleep 1 &&  while [ 1 ]
do
  flock -n 200      # acquire a lock
  echo lock
  a7=$((a7+1));
  echo Задача7_Ко-во_блокировок_$a7 >>stat.txt
  sleep 1
  echo Hello_from_7
  echo unlock
  flock -u -n 200   # release the lock
done &

sleep 1 &&  while [ 1 ]
do
  flock -n 200      # acquire a lock
  echo lock
  a8=$((a8+1));
  echo Задача8_Ко-во_блокировок_$a8 >>stat.txt
  sleep 1
  echo Hello_from_8
  echo unlock
  flock -u -n 200   # release the lock
done &

sleep 1 &&  while [ 1 ]
do
  flock -n 200      # acquire a lock
  echo lock
  a9=$((a9+1));
  echo Задача9_Ко-во_блокировок_$a9 >>stat.txt
  sleep 1
  echo Hello_from_9
  echo unlock
  flock -u -n 200   # release the lock
done &

sleep 1 &&  while [ 1 ]
do
  flock -n 200      # acquire a lock
  echo lock
  a10=$((a10+1));
  echo Задача10_Ко-во_блокировок_$a10 >>stat.txt
  sleep 1
  echo Hello_from_10
  echo unlock
  flock -u -n 200   # release the lock
done &

# Через пять минут завершить все десять задач сигналом sigint
sleep 300 && kill -s SIGINT &

wait

# в итоге статистика блокировок для всех процессов примерно одинакова
