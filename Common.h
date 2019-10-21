#pragma once

#pragma comment(lib,"WS2_32.lib")

#define _CRT_SECURE_NO_WARNINGS
#define LIMIT 1024*100//100k

#include <regex>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <time.h>
#include <winsock2.h>
#include <windows.h>
#include <mutex>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <atomic>
#include <mutex>
#include <sstream>



//定位错误
//void Search_Bug()
//{
//	printf("File    Fame: %s  ", __FILE__);      //文件名
//	printf("Present Line: %d  ", __LINE__);      //所在行
//	printf("Present Function: %s\n", __func__);  //函数名
//}
