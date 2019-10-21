
#include "Common.h"
#include "HttpDownload.h"
#include <string>
using namespace std;

#define GB(x) ((x.HighPart << 2) + ((DWORD)x.LowPart) / 1024.0 / 1024.0 / 1024.0)

//16进制-》10进制
int convert_16_to_10(string str2)
{
	int sum = 0, times;
	int m;
	string::size_type sz = str2.size();
	for (string::size_type index = 0; index != sz; ++index)
	{
		str2[index] = tolower(str2[index]);
		if (str2[index] >= 'a' && str2[index] <= 'f')
		{
			m = str2[index] - 'a' + 10;
			//求幂次方
			times = pow(16, (sz - 1 - index));
			sum += m * times;

		}
		else if (isdigit(str2[index]))
		{
			//需要将字符类型转换为数字类型
			//因为0的ASCII码是48，所以转换为相应的数字，减去48即可
			m = str2[index] - 48;
			times = pow(16, (sz - 1 - index));
			sum += m * times;

		}
		else
		{
			//cout << "无法识别的十六进制!";
			printf("无法识别的16进制数字\n");
			break;
		}
	}
	return sum;
	return 0;
}


//url解析(解析出域名，端口号，文件名)
void Http_Down_Load::url_relation()
{
	string tmpurl = string(_url);
	auto begin = tmpurl.find("//");
	auto end = tmpurl.find('/', begin + 2);
	_host = tmpurl.substr(begin + 2, end - begin - 2);//域名

	_port = 80;//端口号
	
	auto pos = tmpurl.rfind('/');
	_name = tmpurl.substr(pos + 1);//请求资源路径(文件名)
}

//封装http头(赋给成员变量_header)
void Http_Down_Load::pakage_head()
{
	_header = "GET " + _url + " HTTP/1.1\r\nHost: " + _host +  "\r\nRange:bytes=0-1\r\nConnection:keep-alive\r\n\r\n";
}

//获取请求头
string Http_Down_Load::get_header()
{
	return _header;
}

//获取响应头
string Http_Down_Load::get_response()
{
	return _response;
}

//获取文件大小
bool Http_Down_Load::get_size()
{
	const char* pos = strstr(_response.c_str(), "Content-Range: bytes 0-1/");//找到指向C的指针
	if (pos != nullptr)//有文件大小
	{
		int len = strlen("Content-Range: bytes 0-1/");
		const char* cur = pos + len;//cur指向数字的开始

		string tmp;
		while (*cur != '\r')
		{
			tmp += (*cur);
			cur++;
		}
		
		//得到文件大小
		stringstream strin;
		strin << tmp;
		strin >> _file_size;

		//_file_size = atoi(tmp.c_str());   atoi的返回值是int  范围有限
		return true;
	}
	else
	{
		if (strstr(_response.c_str(), "Transfer-Encoding: chunked"))
		{
			cout << "这是带有chunked字段的文件" << endl;
		}
		return false;
	}

}

//根据文件大小，设置线程数，文件块数，每块大小
void Http_Down_Load::Init()
{
	//关于线程数量
	if (_file_size < 50 * 1024)//50k
	{
		_thread_count = 1;
	}
	else if (_file_size < 500 * 1024)//500k
	{
		_thread_count = 3;
	}
	else
	{
		_thread_count = 5;
	}

	//关于块数(==线程数量)
	//int _record_blocks_all[BLOCKS];//记录每一块要下载的大小(即每一块的大小)
	_all_blocks = _thread_count;

	fs.resize(_all_blocks + 1);

	//关于每块大小(应该下载的大小)
	int i = 1;
	for (i = 1; i <= _all_blocks; i++)
	{
		_record_blocks_all[i] = _file_size / _all_blocks;
	}
	_record_blocks_all[_all_blocks] += _file_size % _all_blocks;//除不尽的加到最后一块上
}

//返回线程数量
int Http_Down_Load::get_threads()
{
	return _thread_count;
}


//根据响应信息获取响应头（赋值给成员变量_response,即除从响应首行到空行之间的）,传递的参数是接收到的响应信息
void Http_Down_Load::get_response_head(char buff[])
{
	string tmp;
	char* pos = strstr(buff, "\r\n\r\n");//在响应信息buff中找空行出现的位置
	char* begin = buff;//begin指向响应信息起始位置
	while (begin != pos)//满足条件说明不到空行
	{
		tmp += (*begin);
		begin++;
	}

	_response = tmp;
}


////根据响应头获取响应状态码
//bool Http_Down_Load::get_stat_okcode()
//{
//	//响应头是_response[9]开头的数字
//	if (_response[9] == '2')
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}


void Http_Down_Load::Sum()
{
	vector<ifstream> fileTmp(7);

	ofstream fileMarge;
	//获取下载文件的名字以及格式
	string _dataFile = _dir + _name;         //_dataFile是保存文件的绝对路径

	string _tmpFile = _dir + _name;         //临时文件地址


	int i = 1;
	while (i <= 6)
	{
		fileMarge.open(_dataFile, ios::app | ios::out | ios::binary); // 最后要保存的文件

		//cout << "打开文件" + _tmpFile + to_string(i) << endl;
		fileTmp[i].open(_tmpFile + to_string(i), ios::in | ios::binary);
		//cout << "文件打开成功！" + _tmpFile + to_string(i) << endl;
		while (together(fileMarge, fileTmp[i]) == false)
		{
			//cout << "合并第"<<i<<"个文件失败" << endl;
			;
		}
		cout << "合并第" << i << "个文件成功" << endl;

		fileMarge.close();
		fileTmp[i].close();
		while (remove((_tmpFile + to_string(i)).c_str()) != 0)
		{
			//cout << "删除第"<<i<<"个文件失败" << endl;;
		}
		//remove((_tmpFile + to_string(i)).c_str());
		cout << "删除第" << i << "个文件" << endl;
		++i;
	}

	if (rename((_dataFile).c_str(), (_dir + _name).c_str()) == 0)
	{
		cout << "文件改名成功" << endl;
	}
	else
	{
		cout << "文件改名失败" << endl;
	}

}

bool Http_Down_Load::together(ofstream& dest, ifstream& str)
{
	if (!dest.is_open())
	{
		cout << "打开目标文件失败" << endl;
		return false;
	}
	if (!str.is_open())
	{
		cout << "打开源文件失败" << endl;
		return false;
	}

	char tmpFile[3000] = { 0 };
	while (!str.eof())
	{
		str.read(tmpFile, 3000);
		dest.write(tmpFile, str.gcount());
		//cout << "合并ing..." << endl;
	}
	return true;
}


//是否断点续传
bool Http_Down_Load::is_continue()
{
	string record_file = _dir + _name + "recode.txt";//总的记录文件名favicon.ico.record.txt
	ofstream fs;
	fs.open(record_file, ios::in | ios::out);//文件不存在不会创建
	if (!fs)//文件不存在(打开失败)
	{
		fs.open(record_file, ios::out);//创建文件
		fs.close();
		fs.open(record_file, ios::in | ios::out);//打开文件

		return false;
	}
	return true;//需要断点续传
}

//下载线程入口函数（当前线程需要下载的块号，从1开始）
void Http_Down_Load::thread_download(int has_blocks)
{
	
	//_has_blocks++;//当前线程要下载第几块(从第一块开始)
	//int has_blocks = (int)_has_blocks;   // 保存属于当前线程的块
	//_mutex.unlock();
	string has_size;//获取已经下载的大小
	lock_guard<mutex> _lock(_mutex1);
	//_mutex1.lock();
	string record_file = _record_dir + _name + "record" + to_string(has_blocks) + ".txt";//每个块记录文件绝对路径(文件名)
	//ifstream fs;//读
	fs[has_blocks].open(record_file, ios::in | ios::out);
	if (!fs[has_blocks])//记录文件不存在
	{
		fs[has_blocks].open(record_file, ios::out);//打开文件
		fs[has_blocks].close();
		fs[has_blocks].open(record_file, ios::in | ios::out);
	}
	else//文件存在
	{
		fs[has_blocks] >> has_size;//从记录文件中读取 已经下载大小 放在has_size中


		if (atoi(has_size.c_str()) == _record_blocks_all[has_blocks])//说明当前块已经下载完毕,结束掉本线程
		{
			//cout << "第" << _has_blocks << "块已经下载完成" << endl;
			//_file_hasdown += atoi(has_size.c_str());

			fs[has_blocks].close();
			return;
		}
		else
		{
			_record_blocks_has[has_blocks] += atoi(has_size.c_str());
			//_file_hasdown += atoi(has_size.c_str());
		}
	}

	fs[has_blocks].close();
	//_file_size += _record_blocks_all[has_blocks];
	//_mutex1.lock();
	
	//设置本快 这次要发送的起始/终止位置
	int begin = ((has_blocks - 1) * _record_blocks_all[1] + atoi(has_size.c_str()));
	int end = ((has_blocks - 1) * _record_blocks_all[1] + _record_blocks_all[has_blocks] - 1);
	//cout << "第" << has_blocks << "个线程" << begin << "-" << end << endl;

	//封装本块请求头
	string header = "GET " + _url + " HTTP/1.1\r\nHost: " + _host + \
		"\r\nRange:bytes=" + to_string(begin) + "-" + to_string(end) + "\r\nConnection:keep-alive\r\n\r\n";

	//第一个线程记录开始下载时间
	if (has_blocks == 1)
	{
		_time_start = clock();
	}

	Socket s;
	s.Create();//创建soket

	bool isconnect;
	int sendlen;
	do
	{
		isconnect = s.Connect(_host, _port);//建立连接
		sendlen = s.Send(header);
	} while (isconnect == false || sendlen <= 0);

	//cout << "线程的请求头" << endl;
	//cout << header << endl;


	char buff[LIMIT] = { 0 };
	int len = 0;
	do
	{
		len = s.Recv(buff);
		if (len == 0)
		{
			cout << "对端已经关闭" << endl;
			return;
		}
		//else if (len > 0)
		//{
		//	cout << "第" << has_blocks << "个线程recv： "<<len<<"字节" << endl;
		//}
	} while (len < 0);

	//cout << "第"<<has_blocks<< "线程的响应信息:" << endl;
	//cout << buff << endl;



	char* pos = strstr(buff, "\r\n\r\n");//在响应信息中找空行起始位置
	char* start = pos + strlen("\r\n\r\n");//正文开始位置
	int n = len - (start - buff);//响应头中的正文长度

	//最终的一个文件绝对路径(文件名)
	string name = _dir + _name;



	if (isCon == false)
	{
		// 不断点续传
		// 创建文件
		if (flag == -1) // 执行的第一个开始下载的线程需要创建文件
		{
			file.open(name, ios::out);   // 不存在则创建
			file.close();
			flag = 1;
		}
		file.open(name, ios::in | ios::out | ios::binary);  // 只打开文件，不创建文件
	}
	else
	{
		// 断点续传
		file.open(name, ios::in | ios::out | ios::binary);
	}


	file.seekp(begin, ios::beg);//挪动文件指针

	file.write(start, n);//将正文写入文件(从start开始接收len长度数据)
	file.flush();

	_record_blocks_has[has_blocks] += n;//跟新当前块已经下载的大小
	_file_hasdown += n;//跟新当前已经下载的文件大小(所有)

	ofstream fs1;//写
	fs1.open(record_file, ios::trunc | ios::in | ios::out | ios::binary);
	fs1 << to_string(_record_blocks_has[has_blocks]) << endl;//将已经下载的大小写入记录文件
	fs1.close();


	//持续接收剩下的正文
	char buff1[LIMIT] = { 0 };
	//fs1.open(record_file, ios::trunc | ios::in | ios::out | ios::binary);

	time_t t_start = clock();
	time_t t_end;
	int time_interval = 0;
	int count = 1;
	do
	{
		t_end = clock();
		n = s.Recv(buff1);  // 接收数据
		if (n > 0)
		{
			//Sleep(1000); 

			file.write(buff1, n);//将接收到的正文写入文件
			file.flush();

			_record_blocks_has[has_blocks] += n;//更新当前块已经下载的大小
			_file_hasdown += n;


			//限速
			// 用时<1S， 并且下载量>LIMIT，则需要休眠 1S - 用时（t_end-t_start）
			if ((1000 + time_interval) >= (t_end - t_start) && _record_blocks_has[has_blocks] >= (LIMIT * count))
			{
				time_interval += 1000;
				count += 1;
				Sleep((1000 + time_interval) - (t_end - t_start));
			}


			//ofstream fs1;//写记录文件
			fs1.open(record_file, ios::trunc | ios::in | ios::out | ios::binary);
			fs1 << to_string(_record_blocks_has[has_blocks]) << endl;//将y写入记录文件
			fs1.close();
		}
		else if (n < 0 && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
		{
			continue;
		}

	} while (_record_blocks_has[has_blocks] < _record_blocks_all[has_blocks]);

	s.Close();
	file.close();
	//_mutex1.unlock();
}

//监控线程入口函数
void Http_Down_Load::thread_monitor()
{
	if (_file_hasdown == _file_size)
	{
		cout << "已经下载完成，无需再次下载" << endl;
		return;
	}
	else if(isCon == true)
	{
		 // 是断点续传，需要打印上次的进度
		printf("文件总大小：%lld  ", _file_size);
		printf("上次已经下载大小：%lld  ", (long long)_file_hasdown);
		double percent = (_file_hasdown * 1.0 / _file_size) * 100.0;//当前下载的百分比
		printf("上次下载量：%lf %%  \n", percent);
		printf("继续下载");
		printf("\n");
	}

	bool out = false;//判断是否下载完成
	bool isDown = false;
	while (!isDown)
	{
		isDown = true;
		double x = 0.001;
		int i = 1;//从第一块开始
		while (i <= _all_blocks)
		{
			if (_record_blocks_has[i] < _record_blocks_all[i])//说明本块没下载完
			{
				isDown = false;
			}
			
			++i;

			time_t _time_end = clock();//当前时间
			time_t diff = (_time_end - _time_start) * 1.0 / 1000;//到目前已经用时s
			if (diff == 0)
				diff = 1;

			double speed = (((long long)_file_hasdown- _last_file_hasdown) / 1024.0 / (diff)) * 1.0; //当前平均速度kb/s
			if (!(speed > -x && speed < x))
			{
				//cout << "当前平均速度" << speed << "kb/s" << "   ";
				printf("当前速度：%lf kb/s  ", speed);
			}
			//cout << "文件总大小" << _file_size << "  当前已经下载大小" << _file_hasdown << "   ";
			//cout << "当前下载量<<" << percent << "%" << endl;

			printf("文件总大小：%lld  ", _file_size);
			printf("当前已下载大小：%lld  ", (long long)_file_hasdown);
			double percent = (_file_hasdown * 1.0 / _file_size) * 100.0;//当前下载的百分比

			printf("当前下载量：%lf %%  ", percent);
			printf("耗时：%4d s", (int)diff);
			printf("\r");

			if ((int)_file_hasdown == _file_size)
			{
				printf("当前速度：%lf kb/s  ", speed);
				printf("文件总大小：%lld  ", _file_size);
				printf("当前已下载大小：%lld  ", (long long)_file_hasdown);
				double percent = (_file_hasdown * 1.0 / _file_size) * 100.0;//当前下载的百分比

				printf("当前下载量：%lf %%  ", percent);
				printf("耗时：%4d s", (int)diff);
				printf("\r");
				out = true;
				break;
			}
		}

		// 写完了
		if (out == true)
			break;
	}
	//来到这，说明每块下载已完成
	printf("\n");
	cout << "所有线程的下载量已完成" << endl;
}



const char* Http_Down_Load::my_strstr(char* str1, const char* str2)
{
	//assert(str1);
	//assert(str2);

	//str1指向每一次比较的起始位置，src遍历str1字符串
	//str2一直指向子串的起始位置，sub遍历str2字符串
	const char* src = str1;
	const char* sub = str2;

	while (*str1 != '\0')
	{
		src = str1;
		sub = str2;

		while (*src != '\0' && *sub != '\0' && *src == *sub)
		{
			src++;
			sub++;
		}
		//来到这有三种可能
		//1.*src=='\0' 2.*sub=='\0' 3.*src!=*sub
		//如果是第2种，说明匹配成功，第1和3种，需要进行下一次匹配
		if (*sub == '\0')
		{
			return str1;
		}
		str1++;
	}
	//来到这说明匹配失败
	return NULL;
}


//接收带有chunked字段的正文内容
void Http_Down_Load::loop()
{
	Socket s;
	s.Create();//创建socet
	s.Connect(_host, _port);//建立连接

	//先再次发送请求，再接收响应

	//封装本块请求头,并发送
	string header = "GET " + _url + " HTTP/1.1\r\nHost: " + _host + \
		"\r\n+Connection:keep-alive\r\n\r\n";
	s.Send(header);

	string name = _dir + _name;
	file.open(name, ios::out);//打开文件，不存在则创建
	file.close();
	file.open(name, ios::in | ios::out | ios::binary);
	if (!file)
	{
		perror("open file error");
		return;
	}

	Sleep(2000);
	int len = 0;
	char buff[LIMIT] = { 0 };
	do
	{
		len = s.Recv(buff);//接收响应信息
		//if (len == 0)
		//{
		//	cout << "对端已关闭" << endl;
		//	return;
		//}
		//else if (len < 0)
		//{
		//	//perror("recv error");
		//	cout << WSAGetLastError() << endl;
		//	cout << endl;
		//}
		//else
		//{
		//	cout << "接受到了"<<len<<"字节数据" << endl;
		//}
	} while (len < 0);

	Sleep(3000);
	
	int alen = 0;
	int seclen = 0;
	const char* endstr = "0\r\n\r\n";
	while ((my_strstr(buff+len+alen- 20,endstr)) == NULL)
	{
		char buff1[LIMIT] = { 0 };
		alen = s.Recv(buff1);
		if (alen > 0)
		{
			memcpy(buff + len + seclen, buff1, alen);

			seclen += alen;
		}

	}
	
	//显示接受到的消息放在buff中(接受到长度为len的消息)
	//cout << buff << endl;


	char* begin = strstr(buff, "\r\n\r\n");
	char* pos = begin + strlen("\r\n\r\n");//指向第一块的块大小
	int sum = 0;
	char* tmp1 = nullptr;
	char* tmp2 = nullptr;

	while ( !((*pos == '0') && (*(pos + 1) == '\r') && (*(pos + 2) == '\n')) )//只要pos指向的大小不是0   
	{
		
		string size;//提取每一块的大小
		while (*pos != '\r')
		{
			size += (*pos);
			pos++;
		}
		tmp1 = pos + 2;//pos指向正文
		int slen = convert_16_to_10(size);//拿到十进制的块大小,slen是实际接收的大小
		sum += slen;
		//file.write(pos, slen);//将当前块写入文件
		file.write(tmp1, slen);//将当前块写入文件
		file.flush();

		_file_hasdown += slen;
		Sleep(1000);

		//printf("已经下载的文件总量:%d ", sum);
		//printf("\r");


		tmp2 = tmp1 + slen + 2;//pos应该指向下一块大小（*pos只要不是0,就说明指向下一个大小）
		
		pos = tmp2;

		//pos = pos + slen;//pos指向\r
		/*if (pos == buff + len)
		{
			len = s.Recv(buff);
		}*/

	}
	//cout << "总共接收了" << sum << endl;

	complete = true;
	file.close();
}

//只对服务端域名进行重新解析
void Http_Down_Load::host_relation()
{
	string tmpurl = string(_url);
	auto begin = tmpurl.find("//");
	auto end = tmpurl.find('/', begin + 2);
	_host = tmpurl.substr(begin + 2, end - begin - 2);//域名
}

//将重定向后的url赋值给_url
void Http_Down_Load::Location(char buff[])
{
	//先从响应信息中拿到Location字段
	char* pos = strstr(buff, "Location: ");
	string tmp;//保存重定向的url
	pos = pos + strlen("Location: ");
	while (*pos != '\r')
	{
		tmp += (*pos);
		pos++;
	}
	//来到这，tmp保存了重定向后的url
	cout << "新的url:" << tmp << endl;

	_url = tmp;
	host_relation();
	//download();//设置号url后，再次调用
}

void Http_Down_Load::loopthread_monitor()
{
	while (complete == false)
	{
		printf("已经下载量：%d", (int)_file_hasdown);
		printf("\r");
	}
	printf("\n");
	printf("下载完成！\n");
}



//主线程调用，下载函数
void Http_Down_Load::download()
{
	//封装请求头,给_header
	pakage_head();

	//创建socket
	Socket s;
	s.Create();//创建socet

	s.Connect(_host, _port);//建立连接
	s.Send(_header);//发送请求头
	cout << "请求头:" << endl;
	cout << _header << endl;

	int len = 0;
	char buff[LIMIT] = { 0 };
	do
	{
		len = s.Recv(buff);//接收响应信息
		if (len == 0)
		{
			cout << "对端已关闭" << endl;
			return;
		}
	} while (len < 0);

	
	get_response_head(buff);//提取出响应信息中的头信息(置_response)

	cout << "响应头:" << endl;
	cout << _response << endl;

	char code = get_stat_code(buff);//获取响应状态
	if (code == '3')
	{
		cout << "需要重定向" << endl;
		Location(buff);//将接收到的响应信息传过去,解析出location字段，赋给_url,往下走
		s.Close();
		this->download();//重新回调自己
		return;
	}
	else if (code == '4' || code=='5')
	{
		char* pos = strstr(buff, "\r\n");//首行结束位置
		string tmp;
		char* begin = buff;
		while (begin != pos)
		{
			tmp += (*begin);
			begin++;
		}
		cout << tmp << endl;//显示相应首行
		return;
	}
	
	if (get_size() == false)//置文件总大小_file_size,返回false,说明有chunked字段
	{
		s.Close();
		thread loopthread(&Http_Down_Load::loopthread_monitor, this);
		loop();//循环接收正文信息
		//cout << endl;
		loopthread.join();
		return;
	}
	//来到这说明有大小
	if (_file_size / 1024 / 1024 / 1024 > getDiakFreeBytes(1))//当磁盘剩余空间<要下载的文件大小时
	{

		cout << "磁盘剩余空间不足" << endl;
		return;
		//getchar();
	}

	Init();//置线程数，文件块数，每块文件应该下载大小


	char tmpfilesize[512];
	// 循环判断每个块是否存在记录文件，如果存在，则说明要断点续传；否则不断点续传
	for (int i = 1; i <= _thread_count; i++)
	{
		string tmp_file = _record_dir + _name + "record" + to_string(i) + ".txt";//每个块记录文件绝对路径(文件名)
		ifstream ifs;
		ifs.open(tmp_file, ios::in | ios::out);
		if (ifs)  // 如果记录文件存在
		{
			isCon = true;  // 要断点续传
			flag = 1;
			ifs >> tmpfilesize;   // 从记录文件中读取大小，放在tmpfilesize中
			_file_hasdown += atoi(tmpfilesize);   // 重置已下载大小
			_last_file_hasdown += atoi(tmpfilesize);
		}
	}

	//创建监控线程
	thread monitor(&Http_Down_Load::thread_monitor, this);

	//创建下载线程
	vector<thread> v;
	for (int i = 0; i < _thread_count; i++)
	{
		v.push_back(thread(&Http_Down_Load::thread_download, this, i + 1));
		//this->thread_download();
	}

	//线程等待
	for (auto& th : v)
	{
		th.join();
	}
	monitor.join();
	_time_start = 0;
}

//获取磁盘剩余空间
double Http_Down_Load::getDiakFreeBytes(int index)
{
	vector<double> v(26);

	static LPCSTR Drive[] = { "B", "C:", "D:", "E:", "F:", "G:", "H:", "I:", "J:", "K:", "L:", "M:", "N:",
		"O:", "P:", "Q:", "R:", "S:", "T:", "U:", "V:", "W:", "X:", "Y:", "Z:" };
	ULARGE_INTEGER uliDiskFreeSize = { 0 };

	GetDiskFreeSpaceEx(NULL, NULL, NULL, &uliDiskFreeSize);

	DWORD dwMask = GetLogicalDrives();
	DWORD dwIndex = 0;
	dwMask >>= 1;
	double sum = 0;
	while ((dwMask >>= 1) > 0)
	{
		dwIndex++;
		if ((dwMask & 0x00000001) > 0)
		{
			GetDiskFreeSpaceEx(Drive[dwIndex], NULL, NULL, &uliDiskFreeSize);
			// 获取对应盘符的磁盘剩余空间 B从0开始
			//v.push_back(GB(uliDiskFreeSize));
			v[dwIndex] = GB(uliDiskFreeSize);
			sum += GB(uliDiskFreeSize);
		}
	}
	//return sum;
	return v[index];

}


