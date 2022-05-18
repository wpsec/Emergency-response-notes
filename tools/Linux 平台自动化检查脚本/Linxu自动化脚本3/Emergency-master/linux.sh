#!/bin/bash
echo "

	
                         ____ _  _ ____ ____ ____ ____ _  _ ____ _   _ 
                           |___ |\/| |___ |__/ | __ |___ |\ | |     \_/  
                           |___ |  | |___ |  \ |__] |___ | \| |___   |  

                                        Powered by 1997CN                                           

"
read -p "Press any key to continue." var

echo -e "\033[34m[-]主机信息：\033[0m"
# 当前用户
echo -e "USER:\t\t" $(whoami) 2>/dev/null
# 版本信息
echo -e "OS Version:\t" ${PRETTY_NAME}
# 主机名
echo -e "Hostname: \t" $(hostname -s)
# uptime
echo -e "uptime: \t" $(uptime | awk -F ',' '{print $1}')
printf "\n"

# CPU占用率
echo -e "\033[34m[-]CPU使用率：\033[0m"
awk '$0 ~/cpu[0-9]/' /proc/stat 2>/dev/null | while read line; do
	echo "$line" | awk '{total=$2+$3+$4+$5+$6+$7+$8;free=$5;\
        print$1" Free "free/total*100"%",\
        "Used " (total-free)/total*100"%"}'
done
printf "\n"

# CPU占用TOP 10
cpu=$(ps aux | grep -v ^'USER' | sort -rn -k3 | head -10) 2>/dev/null
echo -e "\033[34m[-]CPU TOP 10：\033[0m\n${cpu}\n"

# 内存占用
echo -e "\033[34m[-]内存占用：\033[0m"
free -m
printf "\n"

# 内存占用TOP 10
cpu=$(ps aux | grep -v ^'USER' | sort -rn -k3 | head -10) 2>/dev/null
#echo -e "\e[00;31m[*]内存占用 TOP10:  \e[00m\n${cpu}\n"
echo -e "\033[34m[-]内存占用 TOP 10：\033[0m\n${cpu}\n"

# 查看passwd下的用户，是否存在可疑用户
echo -e "\033[34m[-]/etc/passwd:\033[0m" #blue
cat /etc/passwd |grep -nvi "nologin"

# 显示可远程登录的用户
echo -e "\033[34m[-]可远程登录用户：\033[0m"
awk -F: '/\$1|\$6/{print $1}' /etc/shadow

# 查看有无特权用户，并将其删除
echo -e "\033[34m[-]特权用户：\033[0m"
admin=`awk -F: '$3==0{print $1}' /etc/passwd`
echo -e "$admin"

# 删除可疑特权用户，将/home下的用户一块删掉
for b in {1..10}
do 
	admin=`awk -F: '$3==0{print $1}' /etc/passwd`
	echo "请输入要删除的用户名:"
	read user
	if [ $user == "0" ]; then
		break
	else
		#del=`userdel -r $user`
		echo "Del $user succefssful!!"
	fi
done

# 显示处于连接状态的端口号和响应的进程
port=`netstat -antlp` # port=`netstat -antlp | grep -ni "ESTABLISHED"`
echo -e "\033[34m[-]连接端口状态:：\033[0m"
echo -e "$port"

# for循环用于获取各个pid对应的进程
for a in {1..10}
do 
	echo "please input PID $a [if PID==0, break!]:"
	read PID
	if [ $PID -eq 0 ];
	then
		break
	else
		exe=`ls -l /proc/$PID/exe`
		ps=`ps aux | grep $PID` 
		echo -e "$PID\033[34m 对应进程文件路径 ->  $exe\033[0m" #blue
		echo -e "$PID\033[34m 对应进程为 ->  $ps\033[0m" #blue	
	fi
	echo "是否要杀掉此进程?[1/0]"
	read result
	if [ $result -eq 1 ]; then
		`kill $PID`
		echo "$PID 删除成功！"
	else
		echo "$PID 删除失败！"
	fi
done
printf "\n"

# 查看历史命令，并显示异常操作
shell=`cat ~/.bash_history | grep -e "chmod" -e "rm" -e "wget" -e "ssh" -e "tar" -e "zip" -e "scp"`
echo -e "\033[34m[-]历史命令:：\033[0m"
echo -e "$shell"
printf "\n"

# 增强型历史命令记录
echo -e "\033[34m[-]是否添加增强型历史命令记录？[1/0]\033[0m"
read r
if [ $r -eq 0 ]; then
	echo "未添加增强型命令记录！"
else
	
	cat>>/tmp/666.txt<<EOF
USER_IP=\`who -u am i 2>/dev/null | awk '{print $NF}' | sed -e 's/[()]//g'\` 
if [ "\$USER_IP" = "" ]
then
USER_IP=\`hostname\`
fi
export HISTTIMEFORMAT="%F %T $USER_IP \`whoami\` "
shopt -s histappend
export PROMPT_COMMAND="history -a"
EOF
echo "/etc/profile写入成功！"
printf "\n"

# 使配置生效
#source /etc/profile
echo "/etc/profile已生效！"
printf "\n"
fi

# 用户自定义启动项
echo -e "\033[34m[-]用户自定义启动项：\033[0m"
chkconfig=$(chkconfig --list | grep -E ":on|启用" | awk '{print $1}')
if [ -n "$chkconfig" ];then
	(echo "[*]用户自定义启动项:" && echo "$chkconfig")
else
	echo "[!]未发现用户自定义启动项"
fi
printf "\n"

# 可能存在的危险启动项
echo -e "\033[34m[-]危险启动项：\033[0m"
dangerstarup=$(chkconfig --list | grep -E ":on|启用" | awk '{print $1}' | grep -E "\.(sh|per|py)$")
if [ -n "$dangerstarup" ];then
	echo "[!]发现危险启动项:" && echo "$dangerstarup"
else
	echo "[*]未发现危险启动项"
fi
printf "\n"

# 系统定时任务
echo -e "\033[34m[-]系统定时任务：\033[0m"
syscrontab=$(more /etc/crontab | grep -v "# run-parts" | grep run-parts)
if [ -n "$syscrontab" ];then
	echo "[!]发现存在系统定时任务:" && more /etc/crontab 
else
	echo "[*]未发现系统定时任务"
fi
printf "\n"

# 可疑定时任务
echo -e "\033[34m[-]可疑定时任务：\033[0m"
dangersyscron=$(egrep "((chmod|useradd|groupadd|chattr)|((wget|curl)*\.(sh|pl|py)$))"  /etc/cron*/* /var/spool/cron/*)
if [ $? -eq 0 ];then
	(echo "[!]发现可疑定时任务：" && echo "$dangersyscron")
else
	echo "[*]未发现可疑系统定时任务"
fi
printf "\n"

# 查看日志配置
echo -e "\033[34m[-]日志配置：\033[0m"
logconf=$(more /etc/rsyslog.conf | egrep -v "#|^$")
if [ -n "$logconf" ];then
	(echo "[*]日志配置如下:" && echo "$logconf")
else
	echo "[!]未发现日志配置文件"
fi
printf "\n"

# 检查日志是否被清除
echo -e "\033[34m[-]查看日志是否被清除：\033[0m"
logs=$(ls -l /var/log/)
if [ -n "$logs" ];then
	echo "[*]日志文件存在"
else
	echo "[!]日志文件不存在,可能被清除！"
fi
printf "\n"

# secure日志分析
echo -e "\033[34m[-]secure日志分析-登录成功情况：\033[0m"
loginsuccess=$(more /var/log/secure* | grep "Accepted password" | awk '{print $1,$2,$3,$9,$11}')
if [ -n "$loginsuccess" ];then
	(echo "[*]日志中分析到以下用户成功登录:" && echo "$loginsuccess") 
	(echo "[*]登录成功的IP及次数如下：" && grep "Accepted " /var/log/secure* | awk '{print $11}' | sort -nr | uniq -c ) 
	(echo "[*]登录成功的用户及次数如下:" && grep "Accepted" /var/log/secure* | awk '{print $9}' | sort -nr | uniq -c ) 
else
	echo "[*]日志中未发现成功登录的情况"
fi
printf "\n"

echo -e "\033[34m[-]secure日志分析-登录失败情况：\033[0m"
loginfailed=$(more /var/log/secure* | grep "Failed password" | awk '{print $1,$2,$3,$9,$11}')
if [ -n "$loginfailed" ];then
	(echo "[!]日志中发现以下登录失败的情况:" && echo "$loginfailed") |  tee -a $danger_file 
	(echo "[!]登录失败的IP及次数如下:" && grep "Failed password" /var/log/secure* | awk '{print $11}' | sort -nr | uniq -c) 
	(echo "[!]登录失败的用户及次数如下:" && grep "Failed password" /var/log/secure* | awk '{print $9}' | sort -nr | uniq -c) 
else
	echo "[*]日志中未发现登录失败的情况"
fi
printf "\n"

echo -e "\033[34m[-]secure日志分析-本机登录情况：\033[0m"
systemlogin=$(more /var/log/secure* | grep -E "sshd:session.*session opened" | awk '{print $1,$2,$3,$11}')
if [ -n "$systemlogin" ];then
	(echo "[*]本机登录情况:" && echo "$systemlogin")
	(echo "[*]本机登录账号及次数如下:" && more /var/log/secure* | grep -E "sshd:session.*session opened" | awk '{print $11}' | sort -nr | uniq -c)
else
	echo "[!]未发现在本机登录退出情况！"
fi
printf "\n"

echo -e "\033[34m[-]secure日志分析-新增用户情况：\033[0m"
newusers=$(more /var/log/secure* | grep "new user"  | awk -F '[=,]' '{print $1,$2}' | awk '{print $1,$2,$3,$9}')
if [ -n "$newusers" ];then
	(echo "[!]日志中发现新增用户:" && echo "$newusers")
	(echo "[*]新增用户账号及次数如下:" && more /var/log/secure* | grep "new user" | awk '{print $8}' | awk -F '[=,]' '{print $2}' | sort | uniq -c)
else
	echo "[*]日志中未发现新增加用户"
fi
printf "\n"

# message日志分析
echo -e "\033[34m[-]message日志分析-文件传输情况：\033[0m"
zmodem=$(more /var/log/message* | grep "ZMODEM:.*BPS")
if [ -n "$zmodem" ];then
	(echo "[!]传输文件情况:" && echo "$zmodem")
else
	echo "[*]日志中未发现传输文件"
fi
printf "\n"

echo -e "\033[34m[-]cron日志分析-定时下载：\033[0m"
cron_download=$(more /var/log/cron* | grep "wget|curl")
if [ -n "$cron_download" ];then
	(echo "[!]定时下载情况:" && echo "$cron_download")
else
	echo "[*]未发现定时下载情况"
fi
printf "\n"

echo -e "\033[34m[-]cron日志分析-定时执行脚本：\033[0m"
cron_shell=$(more /var/log/cron* | grep -E "\.py$|\.sh$|\.pl$") 
if [ -n "$cron_shell" ];then
	(echo "[!]发现定时执行脚本:" && echo "$cron_download")
else
	echo "[*]未发现定时下载脚本"
fi
printf "\n"

# btmp日志分析
echo -e "\033[34m[-]btmp日志分析-错误登录日志分析：\033[0m"
lastb=$(lastb)
if [ -n "$lastb" ];then
	(echo "[！]错误登录日志如下:" && echo "$lastb")
else
	echo "[*]未发现错误登录日志"
fi
printf "\n"

# lastlog日志分析
echo -e "\033[34m[-]lastlog日志分析-最后一次登录：\033[0m"
lastlog=$(lastlog)
if [ -n "$lastlog" ];then
	(echo "[！]所有用户最后一次登录日志如下:" && echo "$lastlog")
else
	echo "[*]未发现所有用户最后一次登录日志"
fi
printf "\n"

# wtmp日志分析
echo -e "\033[34m[-]wtmp日志分析-用户登录分析：\033[0m"
lasts=$(last | grep pts | grep -vw :0)
if [ -n "$lasts" ];then
	(echo "[！]历史上登录到本机的用户如下:" && echo "$lasts")
else
	echo "[*]未发现历史上登录到本机的用户信息"
fi
printf "\n"


#文件排查
echo -e "\033[34m[-]文件排查：\033[0m"
#敏感文件
echo -e "\033[34m[-]敏感文件列表：\033[0m"
find / ! -path "/lib/modules*" ! -path "/usr/src*" ! -path "/snap*" ! -path "/usr/include/*" -regextype posix-extended -regex '.*sqlmap|.*msfconsole|.*\bncat|.*\bnmap|.*nikto|.*ettercap|.*backdoor|.*tunnel\.(php|jsp|asp|py)|.*\bnc|.*socks.(php|jsp|asp|py)|.*proxy.(php|jsp|asp|py)|.*brook.*|.*frps|.*frpc'
printf "\n"
echo -e "\033[34m[-]新增特权文件列表：\033[0m"
echo -e "
[1]: php
[2]: jsp
[3]: asp
"
echo "请根据编号选择语言[if input == 0; break]:"
read language
if [ $language -eq 1 ]; then
	php=`find / -mtime 0 -name "*.php"`
	php1=`find / -name *.php -perm 777`
	echo -e "$php"
	echo -e "777 = $php1"
elif [ $language -eq 2 ]; then
	jsp=`find / -mtime 0 -name "*.jsp"`
	jsp1=`find / -name *.jsp -perm 777`
	echo -e "$jsp"
	echo -e "777 = $jsp1"
elif [ $language -eq 3 ]; then
	asp=`find / -mtime 0 -name "*.asp"`
	asp1=`find / -name *.asp -perm 777`
	echo -e "$asp"
	echo -e "777 = $asp1"
else
	echo "未匹配语言！"
fi
printf "\n"

# 查看指定目录下文件的时间排序[从新到旧]
echo -e "\033[34m[-]查看指定目录下文件的时间排序[从新到旧]：\033[0m"
#ls -alt | head -n 10
for x in {1..10}
do
	echo -e "\033[34m[-]请输入目录名称[if input == 0; break]：\033[0m"
	read directory 
	# if [ `echo $directory | awk -v tem=0 '{print($1>tem)? "1":"0"}'` == "0" ];then
	if [ $directory == "0" ]; then
		break
	else
		directory1=`ls $directory -alt | head -n 30`
		echo -e "$directory1"
	fi
done
printf "\n"

# 查看异常文件的创建修改等的时间
echo -e "\033[34m[-]查看异常文件的创建修改等的时间：\033[0m"
for y in {1..10}
do
	echo -e "\033[34m[-]请输入文件的绝对路径[if input == 0; break]：\033[0m"
	read file
	if [ $file == "0" ]; then
		break
	else
		files=`stat $file`
		echo -e "$files"
	fi
done

echo -e "\033[34m[+]检查结束！\033[0m"