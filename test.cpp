#include "Common.h"
#include "Communation.h"
#include "HttpDownload.h"

using namespace std;

//检查url是否合法
bool check_url(const string creditCard)
{
	bool rs = false;
	string pattern = { "http(s)?://([\\w-]+\\.)+[\\w-]+(/[\\w- ./?%&=]*)?" };
	regex re(pattern);
	rs = regex_match(creditCard, re);

	if (rs)
	{
		return true;
	}
	return false;
}

int main()
{
	string url;
	cout << "请输入url:";
	getline(cin, url);
	if (check_url(url))
	{
		Http_Down_Load d(url);//创建d对象
		d.url_relation();//url解析22
		d.download();//下载
	}
	else
	{
		cout << "这是一个不合法的url" << endl;
	}
	system("pause");
	return 0;
}

//void menu()
//{
//	cout << "************** 1.download *****************" << endl;
//	cout << "************** 2. exit    *****************" << endl;
//}
//
//int main()
//{
//	//string url = "http://down.sandai.net/thunderx/XunLeiWebSetup10.1.17.484gw.exe";
//
//	//string url = "http://cr1.jc9559.com/vmwareworkstationjjchsazzcb.zip";//vm 220MB
//	//string url = "http://cr1.197946.com/vmwareworkstationjjchsazzcb.zip";
//
//	//string url="http://cr1.197946.com/vmwareworkstationjjchsazzcb.zip";
//	//string url = "http://releases.llvm.org/3.3/clang-tools-extra-3.3.src.tar.gz";  //107k
//	
//	// 120MB
//	//string url = "https://store-g1.seewo.com/seewo-mis-desktop%2F1ccd68b80e3a488aa88a7eef50d03420";
//	//string url = "http://cr1.jc9559.com/vmwareworkstationjjchsazzcb.zip";
//	
//	//string url = "http://www.httpwatch.com/httpgallery/chunked/chunkedimage.aspx";//chunked
//
//	// centos 4.27GB
//	//string url = "http://mirrors.aliyun.com/centos/7.6.1810/isos/x86_64/CentOS-7-x86_64-DVD-1810.iso";
//
//	//
//	int input = 0;
//	do
//	{
//		menu();
//		string url;
//
//		cout << "请选择：";
//		cin >> input;
//		cin.get();
//
//		if (input == 1)
//		{
//			cout << "请输入url:";
//			getline(cin, url);
//			if (check_url(url))
//			{
//				Http_Down_Load d(url);//创建d对象
//				d.url_relation();//url解析22
//				d.download();//下载
//			}
//			else
//			{
//				cout << "这是一个不合法的url" << endl;
//			}
//		}
//		else if (input == 2)
//		{
//			cout << "退出程序" << endl;
//			break;
//		}
//		else
//		{
//			cout << "输入有误，请输入1/2" << endl;
//			//cin.sync();
//			fflush(stdin);
//			continue;
//		}
//	} while (1);
//	system("pause");
//	return 0;
//}

