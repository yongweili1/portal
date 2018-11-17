#include "../include/McsfNetBase/mcsf_communication_proxy.h"
using namespace Mcsf;
int main()
{
	CommunicationProxy *proxy = new CommunicationProxy();
	proxy->SetName("me");
	proxy->CheckCastToSystemDispatcher("10.9.19.150:10000");
	proxy->StartListener("");
	
	sleep(1000*1000);
	
}
