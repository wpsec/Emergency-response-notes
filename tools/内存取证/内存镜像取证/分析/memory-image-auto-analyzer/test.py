a=r"""
Offset(P)            #Ptr   #Hnd Access Name
------------------ ------ ------ ------ ----
0x000000003e001150      1      1 R--r-- \Device\HarddiskVolume2\Windows\Registration\R000000000006.clb
0x000000003e007f20      1      1 R--rw- \Device\HarddiskVolume2\Windows\winsxs\amd64_microsoft.windows.common-controls_6595b64144ccf1df_6.0.7601.17514_none_fa396087175ac9ac
0x000000003e009690     18      0 RW-rwd \Device\HarddiskVolume2\$Directory
0x000000003e009870      1      1 R--r-d \Device\HarddiskVolume2\Windows\System32\en-US\KernelBase.dll.mui
0x000000003e00ac80     10      0 R--r-d \Device\HarddiskVolume2\Windows\System32\drmv2clt.dll
0x000000003e00af20     11      0 R--r-d \Device\HarddiskVolume2\Windows\System32\fdProxy.dll
0x000000003e00d070     16      0 R--r-d \Device\HarddiskVolume2\Windows\System32\WindowsPowerShell\v1.0\en-US\powershell.exe.mui
0x000000003e00d590     11      0 R--r-- \Device\HarddiskVolume2\Windows\System32\wmdrmdev.dll
0x000000003e010660      2      0 R--r-- \Device\HarddiskVolume2\Windows\System32\Microsoft\Protect\S-1-5-18\Preferred
0x000000003e0107b0      2      1 ------ \Device\Afd\Endpoint
0x000000003e011a70     14      0 R--r-d \Device\HarddiskVolume2\Windows\System32\WindowsPowerShell\v1.0\powershell.exe
0x000000003e013140     18      0 RW-rwd \Device\HarddiskVolume2\$Directory
0x000000003e0133b0      2      1 ------ \Device\Afd\Endpoint
0x000000003e01c370      2      1 ------ \Device\Afd\Endpoint
0x000000003e01c770      2      1 ------ \Device\Afd\Endpoint
0x000000003e01da70      2      1 ------ \Device\Afd\Endpoint
0x000000003e01ef20      2      1 ------ \Device\Afd\Endpoint
0x000000003e023310     13      0 R--r-d \Device\HarddiskVolume2\Windows\System32\mssitlb.dll
0x000000003e023520      3      0 R--rw- \Device\HarddiskVolume2\Program Files\Windows Media Player\Network Sharing\MediaReceiverRegistrar.xml
0x000000003e024b30      8      0 R--r-d \Device\HarddiskVolume2\Windows\System32\P2P.dll
0x000000003e024c80      9      0 R--r-d \Device\HarddiskVolume2\Windows\System32\IdListen.dll
0x000000003e024dd0      7      0 R--r-d \Device\HarddiskVolume2\Windows\System32\ListSvc.dll
0x000000003e025a30      8      0 R--r-d \Device\HarddiskVolume2\Windows\System32\FXSST.dll
0x000000003e026dd0     14      0 R--r-- \Device\HarddiskVolume2\Windows\System32\catroot2\{127D0A1D-4EF2-11D1-8608-00C04FC295EE}\catdb
0x000000003e027aa0      1      1 R--r-d \Device\HarddiskVolume2\Windows\System32\en-US\FirewallAPI.dll.mui
0x000000003e028070      5      0 R--r-d \Device\HarddiskVolume2\Windows\System32\p2pcollab.dll
0x000000003e028460     15      0 R--r-- \Device\HarddiskVolume2\Windows\System32\catroot2\{F750E6C3-38EE-11D1-85E5-00C04FC295EE}\catdb
0x000000003e028670      1      1 ------ \Device\NamedPipe\wkssvc
0x000000003e029250     17      0 R--r-- \Device\HarddiskVolume2\Windows\System32\catroot2\edb.log
0x000000003e02af20      8      0 R--r-d \Device\HarddiskVolume2\Windows\System32\FXSAPI.dll
0x000000003e02bb40      1      1 RW---- \Device\HarddiskVolume2\Windows\System32\catroot2\edb.log
0x000000003e02c480      1      1 RW---- \Device\HarddiskVolume2\Windows\System32\catroot2\{F750E6C3-38EE-11D1-85E5-00C04FC295EE}\catdb
0x000000003e02c880      9      0 R--r-d \Device\HarddiskVolume2\Windows\System32\hgprint.dll
0x000000003e02d2f0      5      0 R--r-d \Device\HarddiskVolume2\Windows\System32\catroot\{F750E6C3-38EE-11D1-85E5-00C04FC295EE}\Microsoft-Windows-Media-Format-Package~31bf3856ad364e35~amd64~~6.1.7601.17514.cat
0x000000003e031070      1      1 R--rw- \Device\HarddiskVolume2\Windows\System32
0x000000003e032500      1      1 R--rw- \Device\HarddiskVolume2\Windows\winsxs\amd64_microsoft.windows.common-controls_6595b64144ccf1df_6.0.7601.17514_none_fa396087175ac9ac

"""
from backend.res import core_res
res=core_res.format_res(a,"filescan")
for i in res:
    print(i)