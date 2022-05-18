#!/bin/bash
## 自动化安全检查基线设备  2017-01-24
## http://www.aqzt.com
## email: ppabc@qq.com
## 脚本已修改robert yu 适用于 centos 6 系统
## 原作者nothing100 網菔務卐 原文http://www.freebuf.com/sectool/123094.html

ipadd=`ifconfig -a | grep Bcast | tail -n 1 | awk -F "[ :]+" '{print $4}'`
savefile=/tmp/out_"$ipadd".txt
cat <<EOF
*************************************************************************************
*****				linux基线检查脚本	  	     		*****
*************************************************************************************
*****				linux基线配置规范设计				*****
*****				输出结果$savefile		*****
*************************************************************************************
EOF
echo "***************************"
echo "账号策略检查中..."
echo "***************************"
passmax=`cat /etc/login.defs | grep PASS_MAX_DAYS | grep -v ^# | awk '{print $2}'`
passmin=`cat /etc/login.defs | grep PASS_MIN_DAYS | grep -v ^# | awk '{print $2}'`
passlen=`cat /etc/login.defs | grep PASS_MIN_LEN | grep -v ^# | awk '{print $2}'`
passage=`cat /etc/login.defs | grep PASS_WARN_AGE | grep -v ^# | awk '{print $2}'`
  echo " " > $savefile
if [ $passmax -le 90 -a $passmax -gt 0 ];then
  echo "口令生存周期为${passmax}天,符合要求" >> $savefile
else
  echo "口令生存周期为${passmax}天,不符合要求,建议设置不大于90天" >> $savefile
fi

if [ $passmin -ge 6 ];then
  echo "口令更改最小时间间隔为${passmin}天,符合要求" >> $savefile
else
  echo "口令更改最小时间间隔为${passmin}天,不符合要求,建议设置大于等于6天" >> $savefile
fi

if [ $passlen -ge 8 ];then
  echo "口令最小长度为${passlen},符合要求" >> $savefile
else
  echo "口令最小长度为${passlen},不符合要求,建议设置最小长度大于等于8" >> $savefile
fi

if [ $passage -ge 30 -a $passage -lt $passmax ];then
  echo "口令过期警告时间天数为${passage},符合要求" >> $savefile
else
  echo "口令过期警告时间天数为${passage},不符合要求,建议设置大于等于30并小于口令生存周期" >> $savefile
fi
echo "***************************"
echo "账号是否会主动注销检查中..."
echo "***************************"
CHECK_TMOUT=`cat /etc/profile | grep TMOUT | awk -F[=] '{print $2}'` 
if [ "$CHECK_TMOUT" != ""  ];then
  TMOUT=`cat /etc/profile | grep TMOUT | awk -F[=] '{print $2}'`
  if [ $TMOUT -le 600 -a $TMOUT -ge 10 ];then
    echo "账号超时时间${TMOUT}秒,符合要求" >> $savefile
  else
    echo "账号超时时间${TMOUT}秒,不符合要求,建议设置小于600秒" >> $savefile
  fi
else
  echo "账号超时不存在自动注销,不符合要求,建议设置小于600秒" >> $savefile 
fi
#grub和lilo密码是否设置检查
cat /etc/grub.conf | grep password 2> /dev/null
if [ $? -eq 0 ];then
  echo "已设置grub密码,符合要求" >> $savefile
else
  echo "没有设置grub密码,不符合要求,建议设置grub密码" >> $savefile
fi

if [ -f  /etc/lilo.conf ] ; then
cat /etc/lilo.conf | grep password 2> /dev/null
if [ $? -eq 0 ];then
  echo "已设置lilo密码,符合要求" >> $savefile
else
  echo "没有设置lilo密码,不符合要求,建议设置lilo密码" >> $savefile
fi
else
  echo "没有/etc/lilo.conf文件,不检查,符合要求" >> $savefile
fi

#查找非root账号UID为0的账号
UIDS=`awk -F[:] 'NR!=1{print $3}' /etc/passwd`
flag=0
for i in $UIDS
do
  if [ $i = 0 ];then
    echo "存在非root账号的账号UID为0,不符合要求" >> $savefile
  else
    flag=1
  fi
done
if [ $flag = 1 ];then
  echo "不存在非root账号的账号UID为0,符合要求" >> $savefile
fi
#检查umask设置
umask1=`cat /etc/profile | grep umask | grep -v ^#  | tail -n 1 | awk '{print $2}'`
umask2=`cat /etc/csh.cshrc | grep umask | grep -v ^#  | tail -n 1 | awk '{print $2}'`
umask3=`cat /etc/bashrc | grep umask | grep -v ^#  | tail -n 1 | awk '{print $2}'`
flags=0
for i in $umask1
do
  if [ $i = "027" ];then
    echo "/etc/profile文件中所设置的umask为${i},符合要求" >> $savefile
  else
    flags=1
  fi
done
if [ $flags = 1 ];then
  echo "/etc/profile文件中所所设置的umask为${i},不符合要求,建议设置为027" >> $savefile
fi 


flags=0
for i in $umask2
do
  if [ $i = "027" ];then
    echo "/etc/csh.cshrc文件中所设置的umask为${i},符合要求" >> $savefile
  else
    flags=1
  fi
done  
if [ $flags = 1 ];then
  echo "/etc/csh.cshrc文件中所所设置的umask为${i},不符合要求,建议设置为027" >> $savefile
fi


flags=0
for i in $umask3
do
  if [ $i = "027" ];then
    echo "/etc/bashrc文件中所设置的umask为${i},符合要求" >> $savefile
  else
    flags=1
  fi
done
if [ $flags = 1 ];then
  echo "/etc/bashrc文件中所设置的umask为${i},不符合要求,建议设置为027" >> $savefile
fi




echo "***************************"
echo "检查重要文件权限中..."
echo "***************************"

file1=`ls -l /etc/passwd | awk '{print $1}'`
file2=`ls -l /etc/shadow | awk '{print $1}'`
file3=`ls -l /etc/group | awk '{print $1}'`
file4=`ls -l /etc/securetty | awk '{print $1}'`
file5=`ls -l /etc/services | awk '{print $1}'`

file7=`ls -l /etc/grub.conf | awk '{print $1}'`


if [ $file1 = "-rw-r--r--" ];then
  echo "/etc/passwd文件权限为644,符合要求" >> $savefile
else
  echo "/etc/passwd文件权限不为644,不符合要求,建议设置权限为644" >> $savefile
fi

if [ $file2 = "-r--------" ];then
  echo "/etc/shadow文件权限为400,符合要求" >> $savefile
else
  echo "/etc/shadow文件权限不为400,不符合要求,建议设置权限为400" >> $savefile
fi

if [ $file3 = "-rw-r--r--" ];then
  echo "/etc/group文件权限为644,符合要求" >> $savefile
else
  echo "/etc/group文件权限不为644,不符合要求,建议设置权限为644" >> $savefile
fi

if [ $file4 = "-rw-------" -o $file4 = "-rw-------." ];then
  echo "/etc/security文件权限为600,符合要求" >> $savefile
else
  echo "/etc/security文件权限不为600,不符合要求,建议设置权限为600" >> $savefile
fi

if [ $file5 = "-rw-r--r--" -o $file4 = "-rw-------." ];then
  echo "/etc/services文件权限为644,符合要求" >> $savefile
else
  echo "/etc/services文件权限不为644,不符合要求,建议设置权限为644" >> $savefile
fi

if [ -f  /etc/xinetd.conf ] ; then
file6=`ls -l /etc/xinetd.conf | awk '{print $1}'`
if [ $file6 = "-rw-------" ];then
  echo "/etc/xinetd.conf文件权限为600,符合要求" >> $savefile
else
  echo "/etc/xinetd.conf文件权限不为600,不符合要求,建议设置权限为600" >> $savefile
fi
else
  echo "没有/etc/xinetd.conf文件,不检查,符合要求" >> $savefile
fi

if [ -h  /etc/grub.conf ] ; then
  echo "/etc/grub.conf文件是符号链接文件,不检查,符合要求" >> $savefile
else
if [ $file7 = "-rw-------" ];then
  echo "/etc/grub.conf文件权限为600,符合要求" >> $savefile
else
  echo "/etc/grub.conf文件权限不为600,不符合要求,建议设置权限为600" >> $savefile
fi
fi

if [ -f /etc/lilo.conf ];then
file8=`ls -l /etc/lilo.conf | awk '{print $1}'`
  if [ $file8 = "-rw-------" ];then
    echo "/etc/lilo.conf文件权限为600,符合要求" >> $savefile
  else
    echo "/etc/lilo.conf文件权限不为600,不符合要求,建议设置权限为600" >> $savefile
  fi
  
else
  echo "/etc/lilo.conf文件夹不存在,不检查,符合要求"
fi

cat /etc/security/limits.conf | grep -v ^# | grep core
if [ $? -eq 0 ];then
  soft=`cat /etc/security/limits.conf | grep -v ^# | grep core | awk -F ' ' '{print $2}'`
  for i in $soft
  do
    if [ $i = "soft" ];then
      echo "* soft core 0 已经设置,符合要求" >> $savefile
    fi
    if [ $i = "hard" ];then
      echo "* hard core 0 已经设置,符合要求" >> $savefile
    fi
  done
else 
  echo "没有设置core,建议在/etc/security/limits.conf中添加* soft core 0和* hard core 0" >> $savefile
fi


echo "***************************"
echo "检查ssh配置文件中..."
echo "***************************"
cat /etc/ssh/sshd_config | grep -v ^# |grep "PermitRootLogin no"
if [ $? -eq 0 ];then
  echo "已经设置远程root不能登陆,符合要求" >> $savefile
else
  echo "允许远程root登陆,不符合要求,建议/etc/ssh/sshd_config添加PermitRootLogin no" >> $savefile
fi

#检查telnet是否开启
if [ -f  /etc/xinetd.d/telnet ] ; then
telnetd=`cat /etc/xinetd.d/telnet | grep disable | awk '{print $3}'`
if [ $telnetd = "yes" ];then
  echo "检测到telnet服务开启,不符合要求,建议关闭telnet" >> $savefile
fi
else
  echo "没有/etc/xinetd.d/telnet文件,不检查,符合要求" >> $savefile
fi

Protocol=`cat /etc/ssh/sshd_config | grep -v ^# | grep Protocol | awk '{print $2}'`
if [ $Protocol = 2 ];then
  echo "openssh使用ssh2协议,符合要求" >> $savefile
fi
if [ $Protocol = 1 ];then
  echo "openssh使用ssh1协议,不符合要求" >> $savefile
fi

#检查保留历时命令条数
HISTSIZE=`cat /etc/profile|grep HISTSIZE|head -1|awk -F[=] '{print $2}'`
if [ $HISTSIZE -eq 5 ];then
  echo "保留历时命令条数为$HISTSIZE,符合要求" >> $savefile
else
  echo "保留历时命令条数为$HISTSIZE,不符合要求,建议/etc/profile的HISTSIZE设置为5" >> $savefile
fi

#检查重要文件的属性
flag=0
for ((x=1;x<=15;x++))
do
  apend=`lsattr /etc/passwd | cut -c $x`
  if [ $apend = "i" ];then
    echo "/etc/passwd文件存在i安全属性,符合要求" >> $savefile
    flag=1
  fi
  if [ $apend = "a" ];then
    echo "/etc/passwd文件存在a安全属性" >> $savefile
    flag=1
  fi
done
if [ $flag = 0 ];then
  echo "/etc/passwd文件不存在相关安全属性,建议使用chattr +i或chattr +a防止/etc/passwd被删除或修改" >> $savefile
fi

flag=0
for ((x=1;x<=15;x++))
do
  apend=`lsattr /etc/shadow | cut -c $x`
  if [ $apend = "i" ];then
    echo "/etc/shadow文件存在i安全属性,符合要求" >> $savefile
    flag=1
  fi
  if [ $apend = "a" ];then
    echo "/etc/shadow文件存在a安全属性" >> $savefile
    flag=1
  fi
done
if [ $flag = 0 ];then
  echo "/etc/shadow文件不存在相关安全属性,建议使用chattr +i或chattr +a防止/etc/shadow被删除或修改" >> $savefile
fi

flag=0
for ((x=1;x<=15;x++))
do
  apend=`lsattr /etc/gshadow | cut -c $x`
  if [ $apend = "i" ];then
    echo "/etc/gshadow文件存在i安全属性,符合要求" >> $savefile
    flag=1
  fi
  if [ $apend = "a" ];then
    echo "/etc/gshadow文件存在a安全属性" >> $savefile
    flag=1
  fi
done
if [ $flag = 0 ];then
  echo "/etc/gshadow文件不存在相关安全属性,建议使用chattr +i或chattr +a防止/etc/gshadow被删除或修改" >> $savefile
fi

flag=0
for ((x=1;x<=15;x++))
do
  apend=`lsattr /etc/group | cut -c $x`
  if [ $apend = "i" ];then
    echo "/etc/group文件存在i安全属性,符合要求" >> $savefile
    flag=1
  fi
  if [ $apend = "a" ];then
    echo "/etc/group文件存在a安全属性" >> $savefile
    flag=1
  fi
done
if [ $flag = 0 ];then
  echo "/etc/group文件不存在相关安全属性,建议使用chattr +i或chattr +a防止/etc/group被删除或修改" >> $savefile
fi


#检查snmp默认团体口令public、private
if [ -f /etc/snmp/snmpd.conf ];then
  public=`cat /etc/snmp/snmpd.conf | grep public | grep -v ^# | awk '{print $4}'`
  private=`cat /etc/snmp/snmpd.conf | grep private | grep -v ^# | awk '{print $4}'`
  if [ $public = "public" ];then
    echo "发现snmp服务存在默认团体名public,不符合要求" >> $savefile
  fi
  if [[ $private = "private" ]];then
    echo "发现snmp服务存在默认团体名private,不符合要求" >> $savefile
  fi
else
  echo "snmp服务配置文件不存在,可能没有运行snmp服务" 
fi

#检查主机信任关系
rhosts=`find / -name .rhosts`
rhosts2=`find / -name hosts.equiv`
for i in $rhosts
do
  if [ -f $i ];then
  echo "找到信任主机关系,请查看${i}文件,请自行判断是否属于正常业务需求,建议删除信任IP" >> $savefile
  fi 
done

#检查日志审核功能是否开启
service auditd status
if [ $? = 0 ];then
  echo "系统日志审核功能已开启,符合要求" >> $savefile
fi
if [ $? = 3 ];then
  echo "系统日志审核功能已关闭,不符合要求,建议service auditd start开启" >> $savefile
fi

#检查磁盘动态空间,是否大于等于80%
space=`df -h | awk -F "[ %]+" 'NR!=1{print $5}'`
for i in $space
do
  if [ $i -ge 80 ];then
    echo "警告！磁盘存储容量大于80%,建议扩充磁盘容量或者删除垃圾文件" >> $savefile
  fi
done

echo "***************************"
echo "***	检查完毕      ***"
echo "***************************"



