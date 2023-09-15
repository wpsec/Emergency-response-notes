@echo off
echo "Windows应急检查脚本"
:管理员权限打开CMD窗口，执行bat脚本。
:如出现中文乱码，需要先将脚本用记事本打开，可尝试将文本编码改为ANSI保存。
:1.系统基本信息
systeminfo>系统基本信息.txt

:2.网卡配置信息
ipconfig /all>网卡配置信息.txt

:3.端口信息检查
netstat -anob>端口信息检查.txt

:4.进程信息
wmic process get processid,parentprocessid,caption,commandline,executablepath,usermodetime,kernelmodetime,workingsetsize /value >进程信息1.txt
tasklist /v > 进程信息2.txt

:5.共享位置
net share >共享位置.txt

:6.用户位置
net user > 用户相关(netuser).txt
wmic useraccount > 用户相关(wmic).txt
net localgroup administrators>用户组.txt

:7.启动项
echo 启动文件夹>启动项.txt
dir "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\StartUp" >>启动项.txt
for %%i in ("C:\ProgramData\Microsoft\Windows\Start Menu\Programs\StartUp\*") do ( certutil -hashfile "%%i" md5 )>>启动项.txt
echo Run:>>启动项.txt
reg query HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run >>启动项.txt
for /f "tokens=1-3 delims= " %%a in ('reg query HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run') do (certutil -hashfile %%c md5)>>启动项.txt
reg query HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\RunOnce >>启动项.txt
for /f "tokens=1-3 delims= " %%a in ('reg query HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\RunOnce') do (certutil -hashfile %%c md5)>>启动项.txt
reg query HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Run >>启动项.txt
for /f "tokens=1-3 delims= " %%a in ('reg query HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Run') do (certutil -hashfile %%c md5)>>启动项.txt
reg query HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\RunOnce >>启动项.txt
for /f "tokens=1-3 delims= " %%a in ('reg query HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\RunOnce') do (certutil -hashfile %%c md5)>>启动项.txt
:reg query HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\RunOnceEx >>check.log

echo RunServices:>>启动项.txt
reg query HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\RunServicesOnce>>启动项.txt
reg query HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\RunServicesOnce>>启动项.txt
reg query HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\RunServices>>启动项.txt
reg query HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\RunServices>>启动项.txt

:8.IE浏览器记录
reg query "HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\TypedURLs" >IE浏览器记录.txt

:9.程序记录
reg query HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\RunMRU >程序记录.txt
reg query HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\TypedPaths >>程序记录.txt

:10.防火墙信息
netsh advfirewall firewall show rule name=all dir=in type=dynamic status=enabled >防火墙信息.txt

:11.关键文件
echo "Shift|放大镜后门检测:" >关键文件.txt
certutil -hashfile C:\windows\system32\sethc.exe md5 >>关键文件.txt
certutil -hashfile C:\windows\system32\magnify.exe md5 >>关键文件.txt
echo "Time_Providers:">>关键文件.txt
for /f "delims=" %%i in ('reg query HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\W32Time\TimeProviders\') do (reg query "%%i")>>关键文件.txt
certutil -hashfile %systemroot%\system32\w32time.dll md5 >>关键文件.txt
certutil -hashfile %SystemRoot%\System32\vmictimeprovider.dll md5 >>关键文件.txt
echo "netsh:">>关键文件.txt
reg query HKLM\SOFTWARE\Microsoft\Netsh >>关键文件.txt
echo "IFEO:">>关键文件.txt
for /f "delims=" %%i in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options"') do (reg query "%%i")>>关键文件.txt
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options"

:12.计划任务schtasks
schtasks /query /v /fo csv>>计划任务.csv

:13.计划任务at
at >计划任务at.txt

:14.计划任务reg
reg query "HKEY_LOCAL_MACHINE\Software\Microsoft\SchedulingAgent" >计划任务reg.txt
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Schedule\TaskCache\Tree" >>计划任务reg.txt

:15.计划任务at
md "Tasks"
xcopy "%SystemRoot%\System32\Tasks" "Tasks" /E /Y /F

:16.系统日志
md "Logs"
xcopy "%windir%\System32\config" "Logs" /E /Y /F & xcopy "%windir%\System32\winevt\Logs" "Logs" /E /Y /F

:17.powershell历史命令
md "powershell命令"
xcopy "%appdata%\Microsoft\Windows\PowerShell\PSReadline\ConsoleHost_history.txt" "powershell命令" /E /Y /F