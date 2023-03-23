#!/bin/bash

FILE="stats"
BINARY="lock"
ERRORLOG="tmp/locker/errorlog"

# FILE.lck shouldn't be present after end of last test, but it's just insurance
rm -f "$FILE" && rm -f "$FILE.lck"

CMD="./$BINARY $FILE"

bash -c "$CMD > /dev/null 2>./$ERRORLOG" &

sleep 2

rm "$FILE.lck"

PIDS=$(ps aux | grep "$CMD" | grep -v grep | awk '{print $2}')
echo $PIDS

for pid in $PIDS; do
  kill INT $pid
done;

echo "stderr of locker:"
cat "./$ERRORLOG"