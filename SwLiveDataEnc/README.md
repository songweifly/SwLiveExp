# 云桌面安全管理系统


通过X86服务器虚拟化实施，整合VmWare虚拟化平台（VCenter、View Admin、连接服务器、安全服务器等）以及Windows AD域控、DNS和DHCP等服务，构建了桌面云管理平台。目前，不同的windows域配置不同的权限，可以控制访问不同的外设（USB），但windows没有记录USB等外设考出数据的有效记录，即该云桌面系统不具备数据考出的审计功能，因此能否找到一种方法，能够实时跟踪云桌面数据的拷贝记录行为日志。

   本实验就是通过拦截Windows的拷贝、移动、新建和保存等底层API函数，加入用户自定义日志的功能，但是我们只有WindowsAPI的二进制DLL，没有windowAPI的源代码，又不熟悉汇编，
难道这个方法行不通码！最后通过windows API远程注入，改写了windows系统函数，成功实现以上功能。
    实验一共分四个工程：
    1. Hook windows API
    2. DLL注入器
    3. 日志接收服务
    4. windows密码拦截器
