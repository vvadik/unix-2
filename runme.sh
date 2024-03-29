#!/bin/bash

FILE="stats"
BINARY="lock"
ERRORLOG="tmp/locker/errorlog"

mkdir -p "./tmp/locker"

# FILE.lck shouldn't be present after end of last test, but it's just insurance
rm -f "$FILE" && rm -f "$FILE.lck"

CMD="./$BINARY $FILE"

for i in {1..10}; do
  bash -c "$CMD > /dev/null 2>${ERRORLOG}_${i}" &
done;

sleep 300

PIDS=$(ps aux | grep "$CMD" | grep -v grep | awk '{print $2}')

for pid in $PIDS; do
  kill -INT $pid
done;

# wait for processes got signal
sleep 1

for i in {1..10}; do
  echo "stderr of locker_${i}:"
  cat "${ERRORLOG}_${i}"
done;

echo "stats:"
cat "./$FILE"