#!/bin/bash 
#30 6 * * * /usr/local/sbin/anti-hacking.sh > /dev/null 2>&1
check_path="/home/wwwroot"
hacking_log="/data/shell"
maillist="ppabc@qq.com"
time=`date +%Y/%d/%m/%H:%M:%S`
find ${check_path} -name "*.php" -type f -print0|xargs -0 egrep "(phpspy|c99sh|milw0rm|eval\(base64_decode|spider_bc)"|awk -F: '{print $1}'|sort|uniq > ${hacking_log}/hacking.log
status=$(grep php /data/shell/hacking.log > /dev/null 2>&1)
if [ $? -eq 0 ]; then
     echo "Hacking stauts: ${status} time:$time" | mutt -s "Anti-Hacking wwwroot status check"  -a "${hacking_log}/hacking.log" ${maillist} < "${hacking_log}/hacking.log"
     exit 0
else
     echo not exist
     exit 1
fi

