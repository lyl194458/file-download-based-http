#include "Communation.h"


//创建socket
bool Socket::Create()
{
	//版本初始化
	WORD version(0);
	WSADATA wsadata;
	int socket_return(0);
	version = MAKEWORD(2, 0);
	
	


	socket_return = WSAStartup(version, &wsadata); //添加链接库函数
	if (socket_return != 0)
	{
		return false;
	}

	_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock < 0)//失败
	{
		perror("socket create error!");
		cout << endl;
		return false;
	}

	int re = 0;
	unsigned long ul = 1;
	re = ioctlsocket(_sock, FIONBIO, (unsigned long*)&ul);

	return true;
}

//绑定地址信息(给定服务端域名，端口号)
bool Socket::Bind(string& host, short port)
{
	WORD version(0);
	WSADATA wsadata;
	int socket_return(0);
	version = MAKEWORD(2, 0);
	socket_return = WSAStartup(version, &wsadata); //添加链接库函数
	if (socket_return != 0)
	{
		cout << "添加链接库函数出错" << endl;
		return false;
	}

	//先根据域名，得到对应的ip地址
	hostent* p = gethostbyname(host.c_str());
	if (p == nullptr)
	{
		cout << "域名解析失败" << endl;
		return false;
	}

	sockaddr_in addr;//定义服务端的地址结构
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(inet_ntoa(*(struct in_addr*)*p->h_addr_list));

	int ret = bind(_sock, (sockaddr*)& addr, sizeof(addr));
	if (ret < 0)
	{
		perror("bind error");
		return false;
	}
	return true;
}

//建立连接(传入要建立连接的服务器域名，端口)
bool Socket::Connect(string& host, short port)
{
	//初始化
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);

	// 因为创建的socket默认是阻塞的，这里设置为非阻塞的socket
	int iMode = 1;
	ioctlsocket(_sock, FIONBIO, (u_long FAR*) & iMode);

	//先根据域名，得到对应的ip地址
	// _WINSOCK_DEPRECATED_NO_WARNINGS
	struct hostent* p = gethostbyname(host.c_str());
	if (p == nullptr)
	{
		cout << "域名解析失败" << endl;
		return false;
	}
	sockaddr_in addr;//定义服务端的地址结构

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(inet_ntoa(*(struct in_addr*)*p->h_addr_list));

	// 设置连接超时时间(5s)
	struct timeval tm;
	tm.tv_sec = 10;
	tm.tv_usec = 0;
	int ret = -1;

	// 尝试去连接服务端
	if (-1 != connect(_sock, (SOCKADDR*)&addr, sizeof(SOCKADDR)))// 连接成功
	{
		return true;
	}
	else
	{
		fd_set set;
		FD_ZERO(&set);
		FD_SET(_sock, &set);

		if (select(-1, NULL, &set, NULL, &tm) <= 0)// 有错误(select错误或者超时)
		{
			cout << "connect timeout" << endl;
			return false;
		}
		else
		{
			int error = -1;
			int optLen = sizeof(int);
			getsockopt(_sock, SOL_SOCKET, SO_ERROR, (char*)& error, &optLen);

			if (0 != error)
			{
				perror("connect error");
				return false;
			}
			else
			{
				return true;
			}
		}
	}

	// 设回为阻塞socket
	//iMode = 0;
	//ioctlsocket(_sock, FIONBIO, (u_long FAR*) & iMode); //设置为阻塞模式
}

int Socket::Recv(char buf[])//接收数据(接收到后放在buf中)
{
	// 设置收的超时时间
	int recvTimeout = 5 * 1000;  //5s
	setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)& recvTimeout, sizeof(int));

	//char buf[1024] = { 0 };
	int ret = recv(_sock, buf, LIMIT, 0);//_sock希望拿1024大小的buf去接收服务端的数据
	if (ret > 0)
	{
		return ret;//返回真实接收到的字节数
	}
	/*else if (ret == 0)
	{
		cout << "对方关闭" << endl;
		return 0;
	}*/
	else
	{
		if (errno == EAGAIN)
		{
			cout << "have no data" << endl;
			return 0;
		}
		//perror("recv error");
		//cout << WSAGetLastError() << endl;
		return -1;
	}
}

int Socket::Send(string& str)//发送数据(要发送的数据是str)
{
	// 设置收的超时时间
	int sendTimeout = 5 * 1000;  //5s
	setsockopt(_sock, SOL_SOCKET, SO_SNDTIMEO, (char*)& sendTimeout, sizeof(int));

	int ret = send(_sock, str.c_str(), str.size(), 0);//_sock希望将str发送给服务端
	if (ret > 0)
	{
		return ret;
	}
	else if (ret < 0)
	{
		perror("send error");
		return -1;
	}
	else
	{
		cout << "没发数据，对方关闭" << endl;
		return 0;
	}
}

void Socket::Close()//关闭连接
{
	closesocket(_sock);
}