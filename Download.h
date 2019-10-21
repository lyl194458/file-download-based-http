#include "Common.h"
using namespace std;

#define BLOCKS 50 //文件最大块数

class Http_Down_Load
{
private:
	string _url;//url
	string _host;//域名
	short _port;//端口号
	string _name;//请求资源路径(文件名)

	int _file_size;//文件总大小
	int _file_hasdown;//文件已经下载的总大小

	int _all_blocks;//文件总块数
	int _has_blocks;//文件已经下载的块数
	int _record_blocks_has[BLOCKS];//记录每一块已经下载的大小（0下标不用，从1~_all_blocks）
	int _record_blocks_all[BLOCKS];//记录每一块要下载的大小

	int _thread_count;//线程数量

public:
	//构造函数(给url)
	Http_Down_Load(string& url)
	{
		_url = url;
	}

	//url解析(解析出域名，端口号，文件名)
	void url_relation()
	{
		string tmpurl = string(url);
		auto begin = tmpurl.find("//");
		auto end = tmpurl.find('/', begin + 2);
		_host = tmpurl.substr(begin + 2, end - begin - 2);//域名

		_port = 80;//端口号
		//_port=443;

		// /fio.con
		auto pos = tmpurl.rfind('/');
		_name = tmpurl.substr(pos);//请求资源路径(文件名)
	}
};