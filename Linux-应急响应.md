## 一. 账户安全

`/etc/passwd`

`/etc/shadow`

格式：
用户名：密码：用户ID：组ID：用户说明：家（home）目录：登陆之后shell

注意：无密码只允许本机登陆，远程不允许ssh登陆

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/1.png)

用户名：加密密码：密码最后一次修改日期：两次密码的修改时间间隔：密码有效期：密码修改到期到的警告天数：密码过期之后的宽限天数：账号失效时间：保留

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/2.png)

### 1.1 查询特权账户 & 组
```bash
awk -F: '$3==0{print $1}' /etc/passwd
awk -F: '$4==0{print $1}' /etc/passwd
```

### 1.2 查询可远程登录账户及其它信息

```bash
awk '/\$1|\$6/{print $1}' /etc/shadow   #可以远程登录的账号
cat /etc/passwd | grep /bin/bash #查看哪些用户使用shell
cat /etc/passwd | grep x:0 #查看哪些用户为root权限
cat /etc/passwd | grep -v nologin #查看除了不可登录以外的用户都有哪些，有没有新增的
```

```bash
who #查看当前登录用户（tty 本地登陆  pts 远程登录）
w   #查看系统信息，想知道某一时刻用户的行为
uptime  #查看登陆多久、多少用户，负载
stat /etc/passwd #查看密码文件上一次修改的时间
```

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/3.png)
![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/4.png)
![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/5.png)


### 1.3 查询 sudo 权限账户

```bash
more /etc/sudoers | grep -v "^#\|^$" | grep "ALL=(ALL)"
```
![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/6.png)

### 1.4 禁用or删除账户
```bash
usermod -L username
userdel username
```

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/7.png)



### 1.5 用户历史命令

```bash
cat /home/hack/.bash_history
history #root历史命令
root/.bash_profile
```

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/8.png)



##  二. 异常端口进程服务文件


### 2.1 端口

```bash
netstat -atnlp
```
![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/9.png)


```bash
cat /proc/PID/exe | more
ls -l /proc/PID/exe
netstat -antlp | grep 异常IP | awk '{print $7}' | cut -f1 -d"/"
```

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/10.png)
![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/11.png)


### 2.2 进程
```bash
ps aux | grep PIDnet
file /proc/PID/exe
/proc/7252/exe: symbolic link to '/usr/bin/php'
```

```bash
ps aux --sort=pcpu | head -10   #查看cpu占用率前十的进程
ps -ef --sort -pcpu #按CPU 使用率从高到底排序
ps -ef --sort -pmem  #按内存从高到低
```

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/12.png)

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/13.png)

### 2.3 使用 lsof 分析进程

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/14.png)

使用 ps 查看启动 时间并 杀掉危险进程
```bash
ps -p 7224 -o lstart
kill -9 7224
```


![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/15.png)


### 2.4 服务

```bash
chkconfig #查看开机启动项目
chkconfig  --list #查看服务自启状态
systemctl list-unit-files |grep enabled #ubuntu，centos及使用 systemctl 控制的系统
```
![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/16.png)


### 2.5 文件

很多时候会遇到无法常看文件权限或是病毒在一直向某个文件写入程序，可尝试如下命令：

lsattr 文件 查看权限
chattr -i 文件 接触文件后删除




### 2.6 其它
host 文件
`/etc/hosts`
find 命令
md5sum 命令
grep 命令
diff 命令

很多情况下，存在ps、netstat等一些常见命令被替换，可利用stat查看该状态，查看其修改时间

`stat /bin/netstat`


![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/17.png)





## 三. 启动项与定时任务


系统运行级别：


```bash
l  0 所有进程将被终止，机器将有序的停止，关机时系统处于这个运行级别

l  1 单用户模式。用于系统维护，只有少数进程运行，同时所有服务也不启动

l  2多用户模式。和运行级别3一样，只是网络文件系统(NFS)服务没被启动

l  3多用户模式。允许多用户登录系统，是系统默认的启动级别

l  4留给用户自定义的运行级别

l  5多用户模式，并且在系统启动后运行X-Window，给出一个图形化的登录窗口

l  6所有进程被终止，系统重新启动
```


默认 级别 `/etc/inittab`


![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/18.png)


### 3.1 开机启动文件


`/etc/rc.local`

`/etc/rc.d/rc[0~6].d`

` /etc/rc.d/rc3.d/`




![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/19.png)

排查环境变量

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/20.png)



### 3.2 定时任务
计划任务文件

```bash
/var/spool/cron/* #centos
/var/spool/cron/crontabs/* #ubuntu的
/etc/crontab
/etc/cron.d/*
/etc/cron.daily/* 
/etc/cron.hourly/* 
/etc/cron.monthly/*
/etc/cron.weekly/
/etc/anacrontab #异步定时
/var/spool/anacron/*
```
 /etc/cron.daily/*  查看目录下所有文件

```bash
crontab  -l #查看定时任务
crontab –e #编辑定时任务
crontab -u root –l  #查看root用户任务计划
ls /var/spool/cron/  #查看每个用户自己的执行计划

sed '/gcc.sh/d' /etc/crontab && chmod 0000 /etc/crontab && chattr +i /etc/crontab   #删除计划任务且控制计划任务不能写东西，慎用。




```

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/21.png)



### 3.3 日志

系统常见的日志目录
| 文件名  | 说明  |
| --- | --- |
| cron | 记录系统定时任务相关  |
| dmesg |记录了系统在开机时内核自检的信息  |
| btmp  | 记录二进制错误登录日志，使用lastb查看 |
| lastlog  | 记录二进制系统中所有用户最后一次登录时间的日志 |
| secure  |记录验证和授权方面的信息，只要涉及账号和密码的程序都会记录，比如SSH登录，su切换用户，sudo授权，telnet，ftp等服务登录信息  |
| message  | 记录系统启动后的信息和错误日志 |
| wtmp | 登录进入，退出，数据交换、关机和重启纪录 |


默认日志位置：`var/log`


`/var/log/auth.log` 包含系统授权信息，包括用户登录和使用的权限机制等信息
`/var/log/lastlog` 记录登录的用户，可以使用命令lastlog查看
`/var/log/secure` 记录大多数应用输入的账号与密码，登录成功与否
日志一般信息量巨大，根据自己的环境索引相关的有效的 信息
`/var/log/cron` 记录crontab命令是否被正确的执行

```bash
grep "Failed password for root" /var/log/secure | awk '{print $11}' | sort | uniq -c | sort -nr | more  
```

###### 定位有哪些IP在爆破：  

```bash
grep "Failed password" /var/log/secure|grep -E -o "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"|uniq -c 
```
###### 爆破用户名字典是什么？  

```bash
grep "Failed password" /var/log/secure|perl -e 'while($_=<>){ /for(.*?) from/; print "$1\n";}'|uniq -c|sort -nr  
```

##### 登录成功的IP有哪些：     

```bash
grep "Accepted " /var/log/secure | awk '{print $11}' | sort | uniq -c | sort -nr | more  
```

##### 登录成功的日期、用户名、IP：  

```bash
grep "Accepted " /var/log/secure | awk '{print $1,$2,$3,$9,$11}'   
```



##### 日志中的敏感特征

文件内容中的恶意函数

```bash
PHP：eval(、system(、assert(
JSP：getRunTime(、 FileOutputStream(
ASP：eval(、execute(、 ExecuteGlobal（
```


webshell特征

```bash
Darkblade：goaction=login
JspSpy：o=login
PhpSpy：action=phpinfo
Regeorg：cmd=connect
Other：cmd=
```


linux日志分析工具

[https://github.com/grayddq/GScan](https://github.com/grayddq/GScan)

### 3.4 自动化辅助工具
[https://github.com/grayddq/GScan](https://github.com/grayddq/GScan)

[https://github.com/ppabc/security_check](https://github.com/ppabc/security_check)

[https://github.com/T0xst/linux](https://github.com/T0xst/linux)

[https://github.com/P4ck/Emergency](https://github.com/P4ck/Emergency)

[https://github.com/al0ne/LinuxCheck](https://github.com/al0ne/LinuxCheck)





### 3.5 勒索病毒引擎
深信服
[https://edr.sangfor.com.cn/#/information/ransom_search](https://edr.sangfor.com.cn/#/information/ransom_search)

360
[https://lesuobingdu.360.cn/](https://lesuobingdu.360.cn/)

奇安信
[https://lesuobingdu.qianxin.com/](https://lesuobingdu.qianxin.com/)

腾讯
[https://guanjia.qq.com/pr/ls/](https://guanjia.qq.com/pr/ls/)


VenusEye
[https://lesuo.venuseye.com.cn/](https://lesuo.venuseye.com.cn/)

### 3.6 反勒索病毒工具
腾讯
[https://habo.qq.com/tool/index](https://habo.qq.com/tool/index)


金山毒霸
[https://www.duba.net/dbt/wannacry.html](https://www.duba.net/dbt/wannacry.html)

瑞星
[http://it.rising.com.cn/fanglesuo/index.html](http://it.rising.com.cn/fanglesuo/index.html)

Avast
[https://www.avast.com/zh-cn/ransomware-decryption-tools](https://www.avast.com/zh-cn/ransomware-decryption-tools)

Github tools
[https://github.com/jiansiting/Decryption-Tools](https://github.com/jiansiting/Decryption-Tools)




### 3.7 溯源分析脚本
1、非工作时间段是否有登录行为
2、当日是否有新建/登录失败纪录
3、指定监控目录是否有webshell文件
4、查看是否有新建非法用户
5、查看是否有外联异常端口
6、检测access日志，分析当天攻击行为

```python

#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import re

TRUST_USER = ['root','piaox']
TRUST_IP =['10.0.211.71']
FILE_PATH=['/var/www/html','/tmp','/var/tmp','/home']
FILE_LIST =[]
ACCESS_LOG = '/var/log/httpd/access_log'
php_Features_list = [
'(\$_(?:GET|POST|REQUEST)\[.*?](\s|\n)*\((\s|\n)*\$_(?:GET|POST|REQUEST)\[.*?\](\s|\n)*\))',
'(?:base64_decode)\([\'"][\w\+/=]{200,}[\'"]\)',
'function\_exists\s*\(\s*[\'"](popen|exec|proc\_open|system|passthru)+[\'"]\s*\)',
'@?(eval\_?r?|assert|include|require|include\_once|require\_once|array\_map|array\_walk)+\s*\(\s*\$\_(GET|POST|REQUEST|COOKIE|SERVER|SESSION)+\[?(.*)\]?\s*\)',
'eval\s*\(\s*\(\s*\$\$(\w+)',
'(\$[\w_]{0,15}(\s|\n)*\((\s|\n)*\$_(?:POST|GET|REQUEST)\[.*?\](\s|\n)*\))',
'(ReDuh|silic)',
'(?:call_user_func)(\s|\n)*\(.{0,15}\$_(?:GET|POST|REQUEST)',
'(?:wscript)\.(?:shell)',
'(?:cmd)\.(?:exe)',
'(?:shell)\.(?:application)',
'(?:documents)\s+(?:and)\s+(?:settings)',
'(?:system32)',
'(?:serv\-u)',
'(?:phpspy)',
'(?:webshell)',
'(?:Program)\s+(?:Files)',
'(?:include|require)(?:_once)?\s*["\']?\(?\s*\$?\w+["\']?\)?\s*\;?',
'ec38fe2a8497e0a8d6d349b3533038cb|88f078ec861a3e4baeb858e1b4308ef0|7Zt/TBNnGMfflrqBFnaes|\\x50\\x4b\\x05\\x06\\x00\\x00\\x00\\x00|9c3a9720372fdfac053882f578e65846|silic1234',
'((udp|tcp)\://(.*)\;)+',
'preg\_replace\s*\((.*)/e(.*)\,\s*\$\_\[?(.*)\]?\,(.*)\)',
'preg\_replace\s*\((.*)\(base64\_decode\(\$',
'.*?\$\_\w+.*?@?preg_replace\(("|\').*?/e("|\'),.*?,.*?\)',
'(eval|assert|include|require|include\_once|require\_once)+\s*\(\s*(base64\_decode|str\_rot13|gz(\w+)|file\_(\w+)\_contents|(.*)php\://input)+',
'(include|require|include\_once|require\_once)+\s*\(\s*[\'"](\w+)\.(jpg|gif|ico|bmp|png|txt|zip|rar|htm|css|js)+[\'"]\s*\)',
'\$\_(\w+)\s*=?\s*(eval|assert|include|require|include\_once|require\_once)+\s*\(\s*\$(\w+)\s*\)',
'\(\s*\$\_FILES\[(.*)\]\[(.*)\]\s*\,\s*\$\_(GET|POST|REQUEST|FILES)+\[(.*)\]\[(.*)\]\s*\)',
'(fopen|fwrite|fputs|file\_put\_contents)+\s*\((.*)\$\_(GET|POST|REQUEST|COOKIE|SERVER)+\[(.*)\](.*)\)',
'(fopen|fwrite|fputs|file\_put\_contents)+\s*\(\$\_(GET|POST|REQUEST|COOKIE|SERVER)+\[(.*)\](.*)\)',
'echo\s*curl\_exec\s*\(\s*\$(\w+)\s*\)',
'new com\s*\(\s*[\'"]shell(.*)[\'"]\s*\)',
'\$(.*)\s*\((.*)\/e(.*)\,\s*\$\_(.*)\,(.*)\)',
'\$\_\=(.*)\$\_',
'\$\_(GET|POST|REQUEST|COOKIE|SERVER)+\[(.*)\]\(\s*\$(.*)\)',
'\$(\w+)\s*\(\s*\$\_(GET|POST|REQUEST|COOKIE|SERVER)+\[(.*)\]\s*\)',
'\$(\w+)\s*\(\s*\$\{(.*)\}',
'\$(\w+)\s*\(\s*chr\(\d+\)',
'\$\w*\s*\=\s*\$\_(GET|POST|REQUEST|COOKIE|SERVER|SESSION)\[.*?\]\;\s*include\s+\(\s*\$(.*?)\s*\)\;',
'\$\w+\s*\=\s*\$\_\w+\[.*?\]\;\s*@eval\(.*?\)',
'\$\w+\s*\=\s*base64_decode\(\$\_\w+\[(.*?)\]\);\s*@eval\(.*?\)',
'\$\_\w+\[.*?\]\s*\(\s*\$\_\w+\[.*?\]\s*\)\;',
'\(\$\_\=@\$\_\w+\[.*?\]\s*\)\.@\$\_\(\$\_\w+\[.*?\]\s*\)',
'\$\_\w+\[.*?\]\s*\(\s*\$\_\w+\[.*?\]\s*,\$\_\w+\[.*?\]\)',
'\$\{\'\_\'.\$\_}\[\'\_\'\]\(\$\{\'\_\'.\$\_\}\[\'\_\_\'\]\)\;',
'\_\_angel\_1111111111\_eof\_\_',
'xx.php\?pwd=e',
'687474703a2f2f377368656c6c2e676f6f676c65636f64652e636f6d2f73766e2f6d616b652e6a7067',
'Changed by pnkoo.cn|Jakub Vrana|blackbap.org|Code By isosky|16jTwyAtIHBocCZNeVNRTMr9vt2/4rG4t925pL7fIC0g']

asp_Features_list = [
'(?:eval|execute)(\s|\n)*(?:request)(\s|\n)*\((\s|\n)*(.*?)(\s|\n)*\)',
'(?:eval)(\s|\n)*\((\s|\n)*(?:Request)(\s|\n)*\.(\s|\n)*(?:Item)(\s|\n)*\[(\s|\n)*(.*?)(\s|\n)*\]',
'(?:ExecuteStatement)\(.*?request',
'FromBase64String\("UmVxdWVzdC5JdGVtWyJ6Il0="\)',
'tseuqer\s*lave.*',
'Request.form(.*)eval(.*)',
'<SCRIPT\s*RUNAT=SERVER\s*LANGUAGE=JAVASCRIPT>(.*)eval',
'reDuh(.*)',
'PublicKeyToken\=B03F5F7F11D50A3A',
'20132165414621325641311254123112512',
'#@~\^oHMBAA==@#@&@#@&"\+kwW',
'(Client/Login\.xml\?Command=Login&Sync=1227081437828)|(因为serv-u的userid变化我搞不懂)',
'aspmuma|(免杀去后门版\s*by\s*UnKnown)',
'芝麻开门|F4ck|1c1f81a8b0a630f530f52fa9aa9dda1b|法客论坛|F4ckTeam',
'silicname|silicpass|命令行执行',
'server.mappath\("go.asp"\)',
'MSSQL语句执行工具',
'gif89a(\s|\n)*<%@?',
'UJ@!z\(G9X@\*@!z4Y:\^@\*ryglpAA==\^#~@',
'clsid:72C24DD5-D70A-438B-8A42-98424B88AFB8',
'WebSniff(.*?)Powered\s*by',
'11\.asp',
'mssql导库|闪电小子|\_tysan|MYSQL Manager',
'#@~\^bGsBAA==@#@&AC13`DV{J@!8D@\*@!8D@\*@!\^n']

jsp_Features_list = [
'reDuh',
'没有权限执行该操作',
'(chopper|QQ\(cs,z1,z2,sb\)|caicaihk|Alanwalker|(by\s*n1nty)|(Code\s*By\s*Ninty)|JspSpyPwd|JspSpy|6625108|(charles\s*QQ\s*77707777))',
'((21,25,80,110,1433,1723,3306,3389,4899,5631,43958,65500)|(192.168.230.1\s*4444))',
'1decc1ce886d1b2f9f91ecb39967832d05f8e8b8',
'JFolder\.jsp|Steven\s*Cee|JFileMan\.jsp|hack520\s*by|mailto\:hack520@77169\.org|by\s*Bagheera|luoluonet|Recoding\s*by\s*Juliet|lovehacker|webshell\.jsp|Hacker|jsp\s*File\s*Browser|jshell|ceshi2009']

other_Features_list = ['System32\\cmd\.exe|cmd|CFM\s*shell',
'俺的门门',
'Gamma Web Shell']

attack__Features_list = {"\.\./":"目录穿越攻击",
"select.+(from|limit)":"SQL注入攻击",
"(?:(union(.*?)select))":"SQL注入攻击",
"having|rongjitest":"SQL注入攻击",
"sleep\((\s*)(\d*)(\s*)\)":"SQL注入攻击",
"benchmark\((.*)\,(.*)\)":"SQL注入攻击",
"base64_decode\(":"SQL注入攻击",
"(?:from\W+information_schema\W)":"SQL注入攻击",
"(?:(?:current_)user|database|schema|connection_id)\s*\(":"SQL注入攻击",
"(?:etc\/\W*passwd)":"敏感文件操作",
"into(\s+)+(?:dump|out)file\s*":"SQL注入攻击",
"group\s+by.+\(":"SQL注入攻击",
"xwork.MethodAccessor":"命令注入攻击",
"xwork\.MethodAccessor":"命令注入攻击",
"(gopher|doc|php|glob|file|phar|zlib|ftp|ldap|dict|ogg|data)\:\/":"敏感文件操作",
"\<(iframe|script|body|img|layer|div|meta|style|base|object|input)":"XSS跨站脚本",
"(alert|confirm|prompt)":"XSS跨站脚本",
"(onmouseover|onerror|onload)\=":"XSS跨站脚本",
"java\.lang":"命令注入攻击",
"\.(svn|htaccess|bash_history)":"敏感文件访问",
"\.(bak|inc|old|mdb|sql|backup|java|class)$":"敏感文件访问",
"(vhost|bbs|host|wwwroot|www|site|root|hytop|flashfxp).*\.rar":"敏感文件访问",
"(phpmyadmin|jmx-console|jmxinvokerservlet)":"管理控制台探测行为",
"/(attachments|upimg|images|css|uploadfiles|html|uploads|templets|static|template|data|inc|forumdata|upload|includes|cache|avatar)/(\\w+).(php|jsp)":"敏感文件访问",
"(HTTrack|harvest|audit|dirbuster|pangolin|nmap|sqln|-scan|hydra|Parser|libwww|BBBike|sqlmap|w3af|owasp|Nikto|fimap|havij|PycURL|zmeu|BabyKrokodil|netsparker|httperf|bench| SF/)":"Web扫描器扫描"
}

def Un_worktime_login(localday):
if os.popen("last |grep -v 'reboot' |grep " + "'" + localday.strip() + "'" + "|awk '($7>=\"12:00\" && $7<=\"14:00\") || ($7>=\"00:00\" && $7<=\"06:00\")'").readlines():
print '[+] Un_worktime_login ------------------------------------ [ DONE ]'
for bb in os.popen("last |grep -v 'reboot' |grep " + "'" + localday.strip() + "'" + "|awk '($7>=\"12:00\" && $7<=\"14:00\") || ($7>=\"00:00\" && $7<=\"06:00\")'").readlines():
print bb.strip()
return True
else:
return False

def Faild_user(localday):
print '\n[+] Faild_user ------------------------------------ [ DONE ]'
os.system("egrep '(new user: name=)|(Failed password for)' /var/log/secure*|grep " + "'" + localday + "'")

def Un_trust_user():
all_user = os.popen("grep '/bin/bash' /etc/passwd | awk -F ':' '{print $1}'").readlines()
print '\n[+] Un_trust_user ------------------------------------ [ DONE ]'
for l in all_user:
if l.strip() not in TRUST_USER:
os.system("grep "+l.strip()+" /etc/passwd")

def Dubious_netstat():
print '\n[+] Dubious_netstat ------------------------------------ [ DONE ]'
for ip in TRUST_IP:
os.system("netstat -antp|egrep -v ':::*' |egrep 'ESTABLISHED|LISTEN'|awk '$5 !~ /"+ip.strip()+"/' | awk '$5 ~/:(21|22|443|80|8080|7001|8443|9080)$/{print}'")

def Dubious_files(filepath):
for f in os.listdir(filepath):
path = os.path.join(filepath, f)
if os.path.isfile(path):
FILE_LIST.append(path)
if os.path.isdir(path):
Dubious_files(path)

def Webshell_detect(filename,rulename):
f1 = open(filename).read()
for rule in rulename:
result = re.compile(rule).findall(f1)
if result:
print "============================================"
print 'Risk file:' + filename
print 'Risk code：' + str(result[0])
os.system("stat " +str(filename)+ " |grep 'Modify'")

def Attack_analysis():
f2 = open(ACCESS_LOG).readlines()
print "\n[+] Attack_analysis ------------------------------------ [ DONE ]"
for line in f2:
for r in attack__Features_list:
result = re.compile(r).findall(line)
if result:
print '匹配特征：' + '[' + str(result[0]) + ']' + '===>' + line.strip()
print '攻击类型:' + '[' + attack__Features_list[r] + ']'

if __name__=='__main__':
localday = os.popen("env LANG=en_US.UTF-8 date |awk '{print $2,$3}'").readline().strip()
Un_worktime_login(localday)
Faild_user(localday)
Un_trust_user()
Dubious_netstat()
for file in FILE_PATH:
Dubious_files(file.strip())
print '\n[+] Webshell_detect ------------------------------------ [ DONE ]'
for f in FILE_LIST:
if str(f).lower().endswith(".php"):
Webshell_detect(f, php_Features_list)
elif str(f).lower().endswith(".jsp"):
Webshell_detect(f, jsp_Features_list)
elif str(f).lower().endswith(".aspx"):
Webshell_detect(f, asp_Features_list)
Attack_analysis()

```



版权声明：本文为个人笔记，遵循 [CC 4.0 BY-SA](http://creativecommons.org/licenses/by-sa/4.0/) 版权协议，转载请附上原文出处链接和本声明。

本文链接:[https://github.com/wpsec/Emergency-response-notes](https://github.com/wpsec/Emergency-response-notes)























































