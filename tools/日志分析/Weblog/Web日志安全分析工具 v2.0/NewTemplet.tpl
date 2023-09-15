<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Web日志安全分析报告</title>
<style type="text/css">
body,td,th {
	font-family: "微软雅黑";
}
body {
	background-color:#D2F0FF;
}
.header {
	background-color:#D2F0FF;
	width: 80%;
	margin:0 auto;	
	margin-top:30px;
}
.mid {
	margin: 0 auto;
	width: 90%;
	height: 80%;
	margin: 0 auto;
}
.title {
	font-size: 14px;
	height:30px;
}
.content{
	font-size:12px;
        word-break: break-all;
        word-wrap:break-word;
	height:30px;
}
a:link {
	text-decoration: none;
}
a:visited {
	text-decoration: none;
}
a:hover {
	text-decoration: none;
}
a:active {
	text-decoration: none;
}
</style>
<script language="javascript">
function toggledetail(domid){
if(document.getElementById(domid).style.display == "none"){
document.getElementById(domid).style.display = '';
}else{		
document.getElementById(domid).style.display = 'none';
}
}
</script>
</head>
<body>
<div class="header">
<h1 align="center" style="margin-top:50px"><strong>Web日志安全分析报告</strong></h1>
<p align="right" style="margin-right:42px">报告生成时间：localdate
</p>
</div>
<div class="mid">
  <table width="100%" border="0" name="test">
	<tr bgcolor="#6699CC" class="title">
      <th width="100%" scope="col" align="left">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;恶意IP列表</th>
    </tr>
  </table>
  <table width="100%" border="0">
	<tr bgcolor="#80BFFF" class="title">
      <th width="10%" scope="col">ID</th>
      <th width="30%" scope="col">攻击者IP</th>
      <th width="40%" scope="col">所在地区</th>
	  <th width="20%" scope="col">查&nbsp;&nbsp;&nbsp;&nbsp;看</th>
    </tr>