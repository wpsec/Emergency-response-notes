@echo off
cd %~dp0
echo ****************************************************
echo. 
echo 360星图-Web日志分析引擎
echo Copyright@2014 360网站卫士 [http://wangzhan.360.cn]
echo 交流QQ群：12803537
echo.
echo ****************************************************
start "" "%cd%/bin/xingtu.exe"
ping 1.1.1.1 -n 1 -w 1000 > nul
tail -f logs/output.log
pause

