# Memory image auto-analyzer

## 一个雏形，一个基于Volatility进行可视化、自动化内存镜像分析的工具

目前仅针对Windows平台的内存数据，后端基于在环境中已经安装的 Volatility2 （即 vol.py ）

## 如何使用

```shell
# 配置虚拟环境
python3 -m venv env
# 启动虚拟环境，这个因环境而异
# 然后安装相关依赖
pip install -r requirements.txt
# 启动项目
python3 main.py
```

## TODO - 项目规划-windows
- pslist 和 pstree 整合进树状组件，支持选中进程进行 dump （ memdump 和 procdump ）
- 加入网络分析，包含 netscan
- 加入环境信息分析，包含注册表 printkey，环境变量 envars
- 加入凭据分析，包含 lsadump ， hashdump 和 mimikatz （需要处理 crypto 库）
- filescan 加入常见结果筛选，加入正则搜索，加入提取功能
- 加入常见信息提取，如剪贴板，桌面截图

## filescan 搜索框正则示例
```shell
flag|.zip$|.rar$|.7z$|.txt$|.png$|.jpg$|.gif$|.pdf$|.doc$|.docx$|.pcap$|.pcapng$|.raw$|.kdbx$|Desktop
```