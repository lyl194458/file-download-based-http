#pragma once
#include "Common.h"
using namespace std;

class Socket
{
private:
	int _sock;//socket文件描述符

public:
	//构造函数
	Socket()
	{
		_sock = -1;
	}

	SOCKET get_socket()
	{
		return _sock;
	}

	//创建socket
	bool Create();

	//绑定地址信息(给定服务端域名，端口号)
	bool Bind(string& host, short port);

	//建立连接(传入要建立连接的服务器域名，端口)
	bool Connect(string& host, short port);

	//接收数据(接收到后放在buf中)
	int Recv(char buf[]);

	//发送数据(要发送的数据是str)
	int Send(string& str);

	//关闭连接
	void Close();

};
