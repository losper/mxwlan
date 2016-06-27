#include "wifiwork.hpp"
BOOL InitPMUInstance(HINSTANCE hInstance, int nCmdShow);
void loadwifi(){
	(0,0);
	//wifihelper::get_inst().disconnect();
	
	//xwifi<void>::get_inst().scan();
	/*xwifi<void>::get_inst().disconnect();
	xwifi<void>::get_inst().connect(L"8515_FreeWiFi",L"85158888");*/
	/*tag_NetWork wlist;
	int lsize;	
	wifihelper::get_inst().getWnetwork(wlist,0);
	wifihelper::get_inst().scan();
	wifihelper::get_inst().getWnetwork(wlist,3);
	wifihelper::get_inst().scan();
	wifihelper::get_inst().getWnetwork(wlist,0);
	wifihelper::get_inst().getWlistSize();
	lsize=wifihelper::get_inst().connect(L"8515_FreeWiFi",L"85158888");
	Sleep(10000);
	wifihelper::get_inst().disconnect();*/

}
//void showlist(){
//	tag_NetWork wlist;
//	int lsize=0;
//	wifihelper::get_inst().scan();
//	lsize=wifihelper::get_inst().getWlistSize();
//	for (int i=0;i<lsize;i++)
//	{
//		wifihelper::get_inst().getWnetwork(wlist,i);
//		addlist(wlist.ucSSID,wlist.ulSingleQuality);
//	}
//}