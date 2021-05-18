## 一. 账户安全

```bash
query user  查看当前登录账户
logoff ID 注销用户id
net  user 查看用户
net user username 查看用户登录情况
lusrmgr.msc 打开本地用户组
```


`HKEY_LOCAL_MACHINE\SAM\SAM\Domains\Users\\`
regedit注册表查看账户，确认系统是否存在隐藏账户


![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/22.png)

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/23.png)


### 利用LogParser查看日志

[https://www.microsoft.com/en-us/download/confirmation.aspx?id=24659](https://www.microsoft.com/en-us/download/confirmation.aspx?id=24659)

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/24.png)

查询用户登录情况
```bash
LogParser.exe -i:EVT "SELECT TimeGenerated,EXTRACT_TOKEN(Strings,5,'|') AS USERNAME,EXTRACT_TOKEN(Strings,5,'|') AS SERVICE_NAME,EXTRACT_TOKEN(Strings,5,'|') AS Client_IP FROM 'C:\Users\wp_bj_windows\Desktop\安全.evtx' WHERE EventID=4624"
```

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/25.png)


查询登录成功的事件
```bash
LogParser.exe -i:EVT –o:DATAGRID  "SELECT * FROM 'C:\Users\wp_bj_windows\Desktop\安全.evtx' where EventID=4624"   
```

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/26.png)


## 二. 检查异常端口进程

目前连接

```bash
netstat  -ano
netstat -ano | findstr "ESTABLISHED"    #已经成功建立的连接
```

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/27.png)

msfinfo32

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/28.png)

利用wmic查看进程执行时的命令
约束条件 name
```bash
Wmic process where name='sqlceip.exe' getname,Caption,executablepath,CommandLine ,processid,ParentProcessId  /value
```
或者pid
```bash
Wmic process where processid='2352' get name,Caption,executablepath,CommandLine ,processid,ParentProcessId  /value
```

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/29.png)

## 三. 启动项
![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/30.png)


注册表

`HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Run`


`HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Runonce`

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/31.png)



## 系统定时任务
win7系统利用at

```bash
schtasks | more
```
![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/32.png)

### 任务清单

`C:\Windows\System32\Tasks `

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/33.png)


### 删除任务计划

建议删除任务计划时以管理员登录
SchTasks /Delete /TN  任务计划名称

## 五. 系统服务

`services.msc`


![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/34.png)




`sc stop` [服务名称]停止服务后，


`sc delete` [服务名称]删除服务


## 六. 文件

最近打开的文件

`%UserProfile%\Recent`

![image](https://github.com/wpsec/Emergency-response-notes/blob/main/images/35.png)




C:\Users 目录下文件

## 七. 安防软件日志记录





版权声明：本文为个人笔记，遵循 [CC 4.0 BY-SA](http://creativecommons.org/licenses/by-sa/4.0/) 版权协议，转载请附上原文出处链接和本声明。

本文链接:[https://github.com/wpsec/Emergency-response-notes](https://github.com/wpsec/Emergency-response-notes)

