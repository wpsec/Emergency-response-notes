<a name="SRSyR"></a>
## 容器网络简单理解
容器拥有n多张veth网卡与一张docker0网卡<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695092509486-8473efcc-6a83-48e7-8790-3b42f963b0d2.png#averageHue=%232a2a2a&clientId=u9ef1c6a4-c29b-4&from=paste&height=554&id=uadbfad9d&originHeight=1490&originWidth=976&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=238008&status=done&style=none&taskId=u8b523ddd-9e56-480e-895f-e645b5464b0&title=&width=362.6153869628906)
<a name="rvDbX"></a>
### docker 五种网络

- bridge
   - 默认网络，Docker启动后创建一个docker0网桥，默认创建的容器也是添加到这个网桥中。
- host
   - 容器不会获得一个独立的network namespace，而是与宿主机共用一个。这就意味着容器不会有自己的网卡信息，而是使用宿主机的。容器除了网络，其他都是隔离的。
- none
   - 获取独立的network namespace，但不为容器进行任何网络配置，需要我们手动配置。
- container
   - 与指定的容器使用同一个network namespace，具有同样的网络配置信息，两个容器除了网络，其他都还是隔离的。
- 自定义网络
   - 与默认的bridge原理一样，但自定义网络具备内部DNS发现，可以通过容器名容器之间网络通信。

默认都为bridge网卡<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695092468969-d728f0a5-573c-4741-9186-5cb8d4a99551.png#averageHue=%23202020&clientId=u9ef1c6a4-c29b-4&from=paste&height=341&id=u9b032d2d&originHeight=554&originWidth=2560&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=140016&status=done&style=none&taskId=ua19f4c9f-20ac-4a9f-809f-0d61a4a7377&title=&width=1575.3846153846155)<br />容器内为什么能通baidu<br />容器内虚拟网卡网卡——》容器网关（宿主机虚拟网卡）——》docker0网卡——》物理网卡<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695092854998-c7974aa9-1d8f-4da5-a40b-369a024e4e96.png#averageHue=%23292929&clientId=u9ef1c6a4-c29b-4&from=paste&height=652&id=u38de0a54&originHeight=1059&originWidth=1040&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=185560&status=done&style=none&taskId=ufdb59252-227c-4cd9-8b3f-d7803b2d32f&title=&width=640)
<a name="bqAux"></a>
### 两种情况
我做了一个实验，发现两种情况<br />第一种情况，容器会走vet网卡在到docker0网卡，所以在两张网卡上都能抓到对应的数据包。<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695111887186-0dd1bb34-a0cb-43a0-b8a8-8532b75e1169.png#averageHue=%23333333&clientId=u9ef1c6a4-c29b-4&from=paste&height=473&id=u180c2810&originHeight=769&originWidth=2560&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=346207&status=done&style=none&taskId=ucd3130d2-c745-4a2c-a733-e8af0166d77&title=&width=1575.3846153846155)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695111513753-17c312cb-f572-49ca-a7c1-98debf5c94af.png#averageHue=%23262626&clientId=u9ef1c6a4-c29b-4&from=paste&height=703&id=ua2ed3f11&originHeight=1143&originWidth=2559&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=406335&status=done&style=none&taskId=uda9c3278-5509-4f68-a785-9bb6aaa1149&title=&width=1574.7692307692307)<br />第二种情况，在docker0网卡上抓不到流量，只能在对应的veth网卡上才能抓到流量<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695112093591-5e7d3901-8a92-480e-9b3b-8a585a23d089.png#averageHue=%23252525&clientId=u9ef1c6a4-c29b-4&from=paste&height=295&id=ud485a492&originHeight=384&originWidth=952&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=49148&status=done&style=none&taskId=ud4f205f4-4758-416a-8ea7-413abf8042e&title=&width=730.84619140625)<br />我发现docker0上并不能抓到容器的流量<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695112076799-fde718b0-0ad8-4ff8-b3a9-98a6d8bafdb2.png#averageHue=%231e1e1e&clientId=u9ef1c6a4-c29b-4&from=paste&height=228&id=u43072be9&originHeight=324&originWidth=1053&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=27187&status=done&style=none&taskId=u5b9f033d-71a7-4bf7-9703-3e6119459e2&title=&width=740)<br />在使用traceroute查看路由后，我发现，容器在出网关后到了192.168.68.2，该ip为物理机物理网卡网关，意思是，该容器，从容器虚拟网关出来后直接到了物理机网关，跳过了docker0网卡，所以使用tcpdump -i docker0抓取该容器流量是抓不到的<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695112198657-aacfaff4-bd94-4708-885e-8f5c2cb83152.png#averageHue=%23202020&clientId=u9ef1c6a4-c29b-4&from=paste&height=714&id=u574d835b&originHeight=1160&originWidth=1013&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=119679&status=done&style=none&taskId=uec4f6955-de7c-43ab-bf6a-493d279d3ed&title=&width=623.3846153846154)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695112368145-bf19b8e3-9fcd-4032-9e02-289e335c20f8.png#averageHue=%23252525&clientId=u9ef1c6a4-c29b-4&from=paste&height=258&id=u8a8dfc4a&originHeight=419&originWidth=1116&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=55124&status=done&style=none&taskId=u5cc50f68-faab-420d-94c4-6f92a870bb2&title=&width=686.7692307692307)<br />为什么会出现这总问题，在查看iptables规则时，我发现，在docker创建时可能会添加一些规则
```
iptables -t nat -S
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695110631122-9a658f59-96a4-473c-ac66-356b8a076ab7.png#averageHue=%23242424&clientId=u9ef1c6a4-c29b-4&from=paste&height=687&id=KREKF&originHeight=1117&originWidth=1173&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=187218&status=done&style=none&taskId=uc8a878c2-d502-4fa5-8d79-d72d7f43d10&title=&width=721.8461538461538)
```
-A POSTROUTING -s 172.17.0.0/16 ! -o docker0 -j MASQUERADE
```
这条规则的意思是如果docker0收到来自172.17.0.0/16这个网段的外出包，docker0会交给MASQUERADE处理，而MASQUERADE处理方式是将包源地址替换成host地址发出<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695110735166-190f7a7b-a7eb-4fc4-924f-59ee962b4d43.png#averageHue=%23232323&clientId=u9ef1c6a4-c29b-4&from=paste&height=153&id=rv0t5&originHeight=218&originWidth=992&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=34695&status=done&style=none&taskId=u382cd335-a69f-4390-9b04-b17e025d6d9&title=&width=695.4615478515625)<br />所以，跳过docker0网卡直接到物理网卡的原因是iptables规则中有该网卡直接nat出去的规则
<a name="wSGMu"></a>
## 基础信息收集
<a name="ckghx"></a>
### 基础信息
观察其状态，映射端口
```
docker container ls
docker ps
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695094166016-186519f5-6599-4735-bd5a-c7224e24b89c.png#averageHue=%23262626&clientId=u9ef1c6a4-c29b-4&from=paste&height=278&id=u5f38961c&originHeight=451&originWidth=2073&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=113797&status=done&style=none&taskId=uf0349dfd-1519-4e04-a9d5-d2068bea224&title=&width=1275.6923076923076)
<a name="G4dM8"></a>
### 详细信息
检查容器详细信息，挂载的数据卷，运行时间，mac地址等信息
```
docker inspect ID
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695094364753-71ab27ee-9084-4562-8987-c2f3aab4b230.png#averageHue=%23252525&clientId=u9ef1c6a4-c29b-4&from=paste&height=454&id=u13ecd727&originHeight=737&originWidth=1060&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=105264&status=done&style=none&taskId=uf88c160d-5429-4ebc-b48f-ff761429ce3&title=&width=652.3076923076923)
<a name="mLc3B"></a>
### 资源使用
检查容器资源使用情况
```
docker stats ID
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695094532584-fca83c32-d86e-42b5-9fcc-857fe226a073.png#averageHue=%23292929&clientId=u9ef1c6a4-c29b-4&from=paste&height=569&id=u24afe8d7&originHeight=924&originWidth=2004&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=301548&status=done&style=none&taskId=ua1cd92f1-1a10-47bf-9e0d-1fc961cffec&title=&width=1233.2307692307693)
<a name="gLpPI"></a>
### 进程信息
容器进程信息
```
docker top ID
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695094619058-68cec7a7-c269-4e12-b4a3-5ed9bd8f3aab.png#averageHue=%23232323&clientId=u9ef1c6a4-c29b-4&from=paste&height=335&id=u62466193&originHeight=545&originWidth=2560&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=155903&status=done&style=none&taskId=ud39715c2-2370-41d8-bbec-9f8e22fc78b&title=&width=1575.3846153846155)
<a name="uOnUR"></a>
### 文件
容器文件信息
```
docker diff ID | grep A
# A -add
# D -delete
# C -change
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695094757574-2fa4b4d8-d24c-49b3-bee9-a96d1fe5e9c2.png#averageHue=%23212121&clientId=u9ef1c6a4-c29b-4&from=paste&height=405&id=u8285e15e&originHeight=658&originWidth=1470&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=146710&status=done&style=none&taskId=u09fd5b73-1990-4d1d-95fa-ab17d3790f7&title=&width=904.6153846153846)
<a name="l6jqb"></a>
### 应急处置

1. 构建镜像，保留证据
2. 检查异常
3. 暂停容器内进程
4. 断开容器网络

构建镜像
```
docker commit -m="说明" ID check08:1.0
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695107167011-4f7ef0cb-5446-4456-b055-2fbc5b4de4fa.png#averageHue=%23212121&clientId=u9ef1c6a4-c29b-4&from=paste&height=336&id=u30b9c601&originHeight=546&originWidth=2560&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=138647&status=done&style=none&taskId=ufec56207-c801-41dc-b210-2c3173fb97a&title=&width=1575.3846153846155)<br />暂停容器中的进程，包括后台，守护进程等，文件系统运行状态不变
```
docker pause ID # 暂停
docker unpause ID # 恢复
```

![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695107392996-d0f540e2-36b1-4380-9f22-18b55522c68d.png#averageHue=%232e2e2e&clientId=u9ef1c6a4-c29b-4&from=paste&height=175&id=u09a7129a&originHeight=284&originWidth=2319&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=97625&status=done&style=none&taskId=uab4c7d17-f228-4aef-b607-115fbf18a16&title=&width=1427.076923076923)
<a name="vtbVS"></a>
## 定位容器
容器通过docker0网卡进行通信，可以通过tcpdump指定网卡找到异常网络连接，然后进一步关联容器。<br />情况一：ids或其他安全设备告警，某台linux上出现了恶意连接，该linux主机上部署了多个容器，该如何排查是那个容器出现了问题？

在宿主机上通过netstat -an 是看不到容器内的网络连接的，而一台台进入容器查看网络连接，排查效率很慢，而且很多容器没有安装net-tools工具，没有netstat工具。
<a name="rxEFo"></a>
### 抓取流量

情况一：直接通过docker0网卡进行tcpdump流量抓取，通过安全设备给出的IP地址定位容器。<br />情况二：docker0网卡无法抓取到，只能一个一个网卡进行排查。

抓取docker0网卡流量
```
tcpdump -i docker0 dst host xx.xx.xx.xx -v -w docker.pcap
```
抓取容器对应的veth流量
```
iptables -t nat -S # 查看对应网卡
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695113276621-66d1cadd-7127-4cb7-bf8a-a15281e9e553.png#averageHue=%232c2c2c&clientId=u9ef1c6a4-c29b-4&from=paste&height=259&id=ua754afc3&originHeight=365&originWidth=984&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=62327&status=done&style=none&taskId=u60494cd3-a248-463c-9f8f-5f43f8d5b8f&title=&width=697.5385131835938)
```
tcpdump -i br-28b6e6930d36 dst host 172.29.246.156 -v -w br-28b6e6930d36.pcap
tcpdump -i br-28b6e6930d36 dst host 172.29.246.156 -v
```
发现172.19.0.2与该恶意IP产生了通信<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695113411884-d4066fde-4400-4809-b9d6-9f1bd3599c6b.png#averageHue=%23252525&clientId=u9ef1c6a4-c29b-4&from=paste&height=422&id=u6ee647cd&originHeight=686&originWidth=2560&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=247041&status=done&style=none&taskId=u1850ae57-e2b3-4481-9d98-1879adf1598&title=&width=1575.3846153846155)
<a name="LtgvK"></a>
### 定位容器
利用docker inspect -f匹配模块文件匹配对应容器
```
docker inspect -f '{{.Name}}{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' $(docker ps -aq) | grep 172.19.0.2

docker container ls -a | grep 82-pte-lamp-1
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695114149794-7933d46f-3a94-4603-b7c6-d186163668bf.png#averageHue=%231e1e1e&clientId=u9ef1c6a4-c29b-4&from=paste&height=157&id=u110c8b0b&originHeight=255&originWidth=2076&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=42752&status=done&style=none&taskId=u11cf7de8-5ff4-4213-ac9f-fb901b2efd1&title=&width=1277.5384615384614)

<a name="DGGk6"></a>
## 开源容器扫描器
[https://github.com/chaitin/veinmind-tools](https://github.com/chaitin/veinmind-tools)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695118895044-b47d2750-4c70-49e5-9407-a6cd156112c5.png#averageHue=%23343434&clientId=u9ef1c6a4-c29b-4&from=paste&height=779&id=uf3d07f7c&originHeight=1266&originWidth=1324&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=274712&status=done&style=none&taskId=u4505b488-6f43-4471-b237-4b36736cba1&title=&width=814.7692307692307)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695120351040-c7af38a6-ebb1-43c7-9afe-0d16ffd38cf5.png#averageHue=%23252525&clientId=u9ef1c6a4-c29b-4&from=paste&height=716&id=uc8a1f57c&originHeight=1164&originWidth=2560&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=360282&status=done&style=none&taskId=u49995092-af31-4321-90af-e22a072282e&title=&width=1575.3846153846155)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/27875807/1695120582668-c18a8f1c-5bc0-4ec3-865d-a298b286e62f.png#averageHue=%23242424&clientId=u9ef1c6a4-c29b-4&from=paste&height=656&id=u2d1ba920&originHeight=1066&originWidth=2532&originalType=binary&ratio=1.625&rotation=0&showTitle=false&size=294367&status=done&style=none&taskId=uea472635-e139-4865-9468-9817a4404dc&title=&width=1558.1538461538462)
