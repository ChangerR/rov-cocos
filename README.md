rov-cocos
=====
此项目是使用cocos2d-x引擎制作的`OpenROV`(`http://www.openrov.com`)的控制台<BR>
原OpenROV只有网页端控制台无法在Android环境下很好运行，所以就使用cocos2d-x制作其控制台的APP版本，目前已可以编译到<BR>
windows和Android客户端，下一步移植到Apple平台<BR>

使用方法
--------------
将OpenROV上电，并把网线插口接到路由器的LAN插口上，进入路由器控制台，找到DHCP客户端地址，找到带有OpenROV名字<BR>
的IP地址，打开APP,将IP地址填入地址栏，点击login即可登录。

编译
------
1.windows下使用Visual Studio 2013 打开 proj.win32中的sln文件编译
2.android下使用带有ADT插件的eclipse将proj.android导入到Eclipse中，编译即可

Demo
-----
#Windows<BR>
![](https://github.com/MysteriousChanger/rov-cocos/blob/master/demo/demo-1.png)
![](https://github.com/MysteriousChanger/rov-cocos/blob/master/demo/demo-2.png)
![](https://github.com/MysteriousChanger/rov-cocos/blob/master/demo/demo-3.png)
#Android<BR>
![](https://github.com/MysteriousChanger/rov-cocos/blob/master/demo/demo-4.png)
![](https://github.com/MysteriousChanger/rov-cocos/blob/master/demo/demo-5.png)
![](https://github.com/MysteriousChanger/rov-cocos/blob/master/demo/demo-6 .png)
