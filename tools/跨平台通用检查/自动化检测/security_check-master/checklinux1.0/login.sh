#!/bin/bash
#version 2.0
#author by 網菔務卐
cat <<EOF
*************************************************************************************
*****使用说明：                                                                    ** 
*****   1.由于安全方面root可能无法远程登陆，所以本程序是先登陆普通用户在su到root用户
最后使用root执行login.sh脚本实现无交互批量登陆                                     ** 
*****   2.使用普通用户上传须可对/tmp目录读写执行权限                               **
*****   3.远程服务器执行检查脚本和输出结果都在/tmp目录下面                         **
*****   4.远程服务器执行完脚本后将会上传到本地/tmp目录下                           **
*****   5.远程服务器将会自动删除checklinux.sh和输出结果                            **
*************************************************************************************
EOF
for i in `cat hosts.txt`
do
  #远程IP地址
  ipadd=`echo $i | awk -F "[~]" '{print $1}'`
  #普通用户
  username=`echo $i | awk -F "[~]" '{print $2}'`
  #普通用户密码
  userpasswd=`echo $i | awk -F "[~]" '{print $3}'`
  #root用户密码
  rootpasswd=`echo $i | awk -F "[~]" '{print $4}'`
  #传checklinux
  expect put.exp $ipadd $username $userpasswd 
  #登陆执行checklinux
  expect sh.exp $ipadd $username $userpasswd $rootpasswd 
  #从远程拿取结果
  expect get.exp $ipadd $username $userpasswd 
  #删除远程结果和脚本
  expect del.exp $ipadd $username $userpasswd $rootpasswd
done
