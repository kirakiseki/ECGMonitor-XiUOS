## w5500 测试文档

通过board/xidatong-riscv64/third_party_driver/ethernet/connect_w5500.c 可以找到内写的3个，包含ping, tcp server test, tcp client test;

 - ping 测试，测试结果包括：
 	- pc ping w5500
 	- w5500 ping baidu.com (DNS实现暂未实现，因此使用baidu.com的ip地址进行寻址)

 <img src="imgs\ping w5500.png" style="zoom:60%;" />

 <img src="imgs\ping baidu.png" style="zoom:60%;" />

- tcp server 测试：在xizi中调用wiz_server_op，指定port，之后可以在pc中向该端口发送数据
	- wiz_server 将额外启动一个线程执行server工作，server将向client返回client发来的消息

<img src="imgs\server1.png" style="zoom:90%;" />

<img src="imgs\server0.png" style="zoom:60%;" />

- client 测试：通过wiz_client_op可以向pc中打开的tcp server发送消息
	- 由于wiz_client_test函数参数接收问题，测试使用的ip地址暂时使用硬编码实现

<img src="imgs\client0.png" style="zoom:67%;" />

<img src="imgs\client1.png" style="zoom:67%;" />