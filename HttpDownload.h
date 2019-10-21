#pragma once
#include "Common.h"
#include "Communation.h"
using namespace std;

#define BLOCKS 50 //文件最大块数
using namespace std;


class Http_Down_Load
{
private:
	string _url;//url
	string _host;//域名
	short _port;//端口号
	string _name;//请求资源路径(文件名)
	string _dir;//保存文件路径

	string _record_dir; // 记录文件保存路径
	long long _file_size;//文件总大小
	atomic_llong _file_hasdown;//文件已经下载的总大小

	int _all_blocks;//文件总块数
	atomic_int _has_blocks;//文件已经下载的块数
	int _record_blocks_has[BLOCKS];//记录每一块已经下载的大小（0下标不用，从1~_all_blocks,最大是BLOCKS）
	int _record_blocks_all[BLOCKS];//记录每一块要下载的大小(即每一块的大小)

	int _last_file_hasdown; // 保存断点续传前文件的大小
	int _thread_count;//线程数量
	string _header;//请求头
	string _response;//响应头   

	ofstream file;
	vector<ifstream> fs;
	mutex _mutex;
	mutex _mutex1;

	bool complete;

	bool isCon;  // 是否断点续传
	int flag; // 是否第一次创建保存的文件

	time_t _time_start;


public:
	//构造函数(给url)
	Http_Down_Load(string& url)
		:_file_size(0)
		, _file_hasdown(0)
		, _all_blocks(0)
		, _thread_count(0)
		, _has_blocks(0)
		, isCon(false)
		, flag(-1)
		, _last_file_hasdown(0)
		, complete(false)
	{
		_url = url;
		_dir = "C://my_download//";
		_record_dir = "C://recordfile//";
		memset(_record_blocks_has, 0, BLOCKS * sizeof(int));
		memset(_record_blocks_all, 0, BLOCKS * sizeof(int));

	}

	//url解析(解析出域名，端口号，文件名)
	void url_relation();

	//重新解析url(重定向)
	void host_relation();

	//封装http头(赋给成员变量_header)
	void pakage_head();

	//获取请求头
	string get_header();

	//获取响应头
	string get_response();

	const char* my_strstr(char* str1, const char* str2);

	//获取状态码起始几xx
	char get_stat_code(char buff[])
	{
		//char code = _response[9];
		char code = buff[9];
		return code;
	}

	//获取文件大小
	bool get_size();

	//根据文件大小，设置线程数，文件块数，每块大小
	void Init();

	//返回线程数量
	int get_threads();

	//根据响应信息获取响应头（赋值给成员变量_response,即除从响应首行到空行之间的）,传递的参数是接收到的响应信息
	void get_response_head(char buff[]);

	//是否断点续传
	bool is_continue();

	//下载线程入口函数
	void thread_download(int num);

	//监控线程入口函数
	void thread_monitor();

	// loop监控线程
	void loopthread_monitor();

	//主线程调用，下载函数
	void download();

	//获取磁盘剩余空间
	double getDiakFreeBytes(int index);

	void Sum();

	bool together(ofstream& dest, ifstream& str);

	void Http_Down_Load::loop();//循环接收响应信息

	void Http_Down_Load::Location(char buff[]);//重定向后的处理

};




