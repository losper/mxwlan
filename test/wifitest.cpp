#include "stdio.h"
#include "../sdk/dpz/xtest.hpp"
#include "../src/wifiwork.hpp"
using namespace dpz;

TestAuto ta("/nand/report.html",2000,"PMU丢包测试报告","刘文军");

typedef int (*FUNxCTEST)(BYTE* data,int len);
struct testdata{
	LPTSTR ssid;
	LPTSTR pwd;
};
TEST(test1,test2){
	
	TEST_INFO(
		本测试程序用来测试WIFI的可连接性
		);
	testdata td[]={
		{L"8515_FreeWiFi",L"85158888"},
		{L"STAR_2FC813",L""},
		{L"刘文军",L"12345678"},
		{L"世界上只有一个中国",L"12345678"},
		{L"8515_FreeWiFi",L"85158888"},
		{L"8515_FreeWiFi",L"85158888"},
		{L"STAR_2FC813",L""},
		{L"刘文军",L"12345678"},
		{L"世界上只有一个中国",L"12345678"},
		{L"8515_FreeWiFi",L"85158888"},
		{L"8515_FreeWiFi",L"85158888"},
		{L"STAR_2FC813",L""},
		{L"刘文军",L"12345678"},
		{L"世界上只有一个中国",L"12345678"},
		{L"8515_FreeWiFi",L"85158888"},
		{L"8515_FreeWiFi",L"85158888"},
		{L"STAR_2FC813",L""},
		{L"刘文军",L"12345678"},
		{L"世界上只有一个中国",L"12345678"},
		{L"8515_FreeWiFi",L"85158888"},
		{L"8515_FreeWiFi",L"85158888"},
		{L"STAR_2FC813",L""},
		{L"刘文军",L"12345678"},
		{L"世界上只有一个中国",L"12345678"},
		{L"8515_FreeWiFi",L"85158888"},
	};
	xwifi<void>::get_inst().scan();
	TCHAR info[200];
	char tips[200];
	int ret=0;
	for(int i=0;i<25;i++){
		ret=xwifi<void>::get_inst().connect(td[i].ssid,td[i].pwd);
		if (ret)
		{
			_stprintf(info,L"连接<em style=\"color:blue\">%s</p>失败",td[i].ssid);
		}else{
			_stprintf(info,L"连接<em style=\"color:blue\">%s</p>成功",td[i].ssid);
		}
		UnicodeToMulti(info,CP_ACP,tips,200);
		CHECKTIPS(!ret,tips);
	}
	tag_NetWork tag;
	xwifi<void>::get_inst().getWnetwork(tag,2);
	xwifi<void>::get_inst().getCurNetwork();
}


//TEST(copy1,tgroup){
//	FILE* f=fopen("/nand/in.data","rb");
//	FILE* f1=fopen("/nand/out.data","w+");
//	BYTE buf[200];
//	int len=0;
//	for (int i=0;i<100;i++)
//	{
//		while(len=fread(buf,1,20,f)){
//			fseek(f1,0,SEEK_END);
//			fwrite(buf,1,len,f1);
//		}
//		fseek(f,0,SEEK_SET);
//	}
//	fclose(f);
//	fclose(f1);
//}