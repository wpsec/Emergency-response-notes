@echo off
title 应急响应信息采集工具V1.4
@echo ================================
@echo * 应急响应信息采集工具 *
@echo                Venustech
@echo        北京启明星辰信息安全技术有限公司
@echo ================================

color 0a
cd  %~dp0
set "filename=%date:~0,4%%date:~5,2%%date:~8,2%"
mkdir .\%filename%\
echo "开始收集进程列表"
wmic process  get name,processid,executablepath /format:htable > ./%filename%/tasklist.html
echo "进程列表收集完毕"
@echo ================================

echo "开始收集系统服务"
wmic service get Name,Description,Caption,DisplayName,ProcessId,started,StartMode,StartName,State,Status,AcceptPause,AcceptStop,PathName /format:htable > ./%filename%/service.html
echo "系统服务收集完毕"
@echo ================================

echo "开始收集系统日志"
xcopy  C:\Windows\System32\winevt\Logs  .\%filename%\logs\ /Y >nul
echo "系统日志收集完毕"
@echo ================================

echo "开始收集网络链接"
netstat -ano > ./%filename%/netstat.txt
echo "网络链接收集完毕"
@echo ================================

echo "开始收集账户信息"
wmic USERACCOUNT list /format:htable >  ./%filename%/account.html
echo "账户信息收集完毕"
@echo ================================

echo "开始收集共享信息"
wmic share list /format:htable > ./%filename%/netshare.html
echo "共享信息收集完毕"
@echo ================================



echo "开始收集路由表"
route print > ./%filename%/route.txt
echo "路由表收集完毕"
@echo ================================


echo "开始收集HOSTS"
xcopy  C:\Windows\System32\drivers\etc  .\%filename%\etc\ /Y >nul
echo "HOSTS收集完毕"
@echo ================================


echo "开始收集mstsc"
reg export   "HKEY_CURRENT_USER\Software\Microsoft\Terminal Server Client\Default" ".\%filename%\mstsc.txt" /y >nul
echo "mstsc收集完毕"
@echo ================================


echo "开始收集注册表启动项"
reg export  "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run" ".\%filename%\autorun.txt"  /y >nul
echo "注册表启动项收集完毕"
@echo ================================


echo "开始收集已安装软件信息"
reg export  "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall" ".\%filename%\software.txt" /y >nul
echo "已安装软件信息收集完毕"
@echo ================================

echo "开始收集计划任务"
schtasks /query /v /fo list > ./%filename%/schtasks.txt
echo "计划任务收集完毕"
@echo ================================

echo "开始ARP绑定"
arp -a > ./%filename%/arp.txt
echo "计划任务收集完毕"
@echo ================================


echo "开始收集系统信息"
systeminfo > ./%filename%/systeminfo.txt
echo "系统信息收集完毕"
@echo ================================

echo "开始收集补丁信息"
wmic qfe list /format:htable > ./%filename%/hotfix.html
echo "补丁信息收集完毕"
@echo ================================

echo "开始收集powershell历史命令"
copy  %userprofile%\AppData\Roaming\Microsoft\Windows\PowerShell\PSReadline\ConsoleHost_history.txt .\%filename%\ConsoleHost_history.txt /Y >nul
copy  %appdata%\Microsoft\Windows\PowerShell\PSReadline\ConsoleHost_history.txt .\%filename%\ConsoleHost_history_1.txt /Y >nul
echo "开始powershell收集完毕"

echo "信息采集完毕，数据存放在当前目录%filename%中"
pause>nul



