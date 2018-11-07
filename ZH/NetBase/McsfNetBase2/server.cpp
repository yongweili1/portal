#include "server.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "id_def.h"

#ifdef _MSC_VER
#include <WinSock2.h>
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib") //ÐèÒªÌí¼ÓIphlpapi.lib¿â
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
#else
#include <stdio.h> 
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netpacket/packet.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#endif

namespace netbasecore{
    server::server(const std::string &port,
        io_service_pool &pool, connection_manager &cm, callback_handle &callbak)
        : io_service_pool_(pool), 
        connection_manager_(cm),
        callback_handlers_(callbak),
        acceptor_(io_service_pool_.get_io_service())
    {
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), atoi(port.c_str()));
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(callbak.getMyName() != dispatcher_name));
        acceptor_.bind(endpoint);
        acceptor_.listen();

        start_accept();
    }

    std::string server::getMacPort()
    {
        return getMacs() + ":" + boost::lexical_cast<std::string>(acceptor_.local_endpoint().port());
    }

    std::string server::getIPInterface4() {
        std::set<std::string> setIPInterfaces;
#ifdef _MSC_VER
        // WSAIoctl for obtaining IPv6 address info.
        PIP_ADAPTER_ADDRESSES AdapterAddresses = 0;
        ULONG OutBufferLength = 0;
        ULONG RetVal = 0;
        unsigned char *octet_buffer = 0;

        RetVal = GetAdaptersAddresses(AF_INET, 0, 0, AdapterAddresses, &OutBufferLength);

        if (RetVal != ERROR_BUFFER_OVERFLOW)
        {
            return "";
        }

        octet_buffer = new unsigned char[OutBufferLength];
        AdapterAddresses = (IP_ADAPTER_ADDRESSES *)octet_buffer;

        RetVal = GetAdaptersAddresses(AF_INET, 0, 0, AdapterAddresses, &OutBufferLength);

        if (RetVal != NO_ERROR)
        {
            delete[] octet_buffer;
            return "";
        }

        // If successful, output some information from the data we received
        PIP_ADAPTER_ADDRESSES AdapterList = AdapterAddresses;

        for (AdapterList = AdapterAddresses; AdapterList != 0; AdapterList = AdapterList->Next) {
            if (AdapterList->OperStatus != IfOperStatusUp)
                continue;

            IP_ADAPTER_UNICAST_ADDRESS *uni = 0;
            if (AdapterList->IfIndex != 0)
                for (uni = AdapterList->FirstUnicastAddress; uni != 0; uni = uni->Next)
                {
                    SOCKET_ADDRESS *sa_addr = &uni->Address;
                    if (sa_addr->lpSockaddr->sa_family == AF_INET)
                    {
                        sockaddr_in addr;
                        int len = sa_addr->iSockaddrLength;
                        int maxlen = static_cast<int> (sizeof(sockaddr_in));
                        if (sa_addr->iSockaddrLength > maxlen)
                            len = maxlen;
                        memcpy(&addr, sa_addr->lpSockaddr, len);
                        setIPInterfaces.insert(inet_ntoa(addr.sin_addr));
                        break;
                    }
                }
        }

        delete[] octet_buffer;
        octet_buffer = nullptr;

#else
        // Take advantage of the BSD getifaddrs function that simplifies
        // access to connected interfaces.
        struct ifaddrs *ifap = 0;
        struct ifaddrs *p_if = 0;

        if (::getifaddrs(&ifap) != 0)
            return "";

        for (p_if = ifap; p_if != 0; p_if = p_if->ifa_next) {
            if (p_if->ifa_addr && p_if->ifa_addr->sa_family == AF_INET) {
                struct sockaddr_in *addr =
                    reinterpret_cast<sockaddr_in *> (p_if->ifa_addr);

                // Sometimes the kernel returns 0.0.0.0 as the interface
                // address, skip those...
                if (addr->sin_addr.s_addr != INADDR_ANY){
                    setIPInterfaces.insert(inet_ntoa(addr->sin_addr));
                }
            }
        }

        ::freeifaddrs(ifap);

#endif

        std::string sRtn;
        for (std::set<std::string>::iterator it= setIPInterfaces.begin(); it != setIPInterfaces.end(); ++it)
        {
            sRtn += (*it) + "&";
        }

        return sRtn.empty() ? "" : sRtn.substr(0, sRtn.size() - 1);
    }

    std::string server::getIPsPort()
    {
        return getIPInterface4() + ":" + boost::lexical_cast<std::string>(acceptor_.local_endpoint().port());
    }

    std::string server::getMacs(){
        std::set<std::string> setMac;

#ifdef _MSC_VER
        unsigned long stSize = sizeof(IP_ADAPTER_INFO);
        IP_ADAPTER_INFO *pIpAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(stSize);
        int nRel = GetAdaptersInfo(pIpAdapterInfo,&stSize);
        if (ERROR_BUFFER_OVERFLOW == nRel)
        {
            FREE(pIpAdapterInfo);

            pIpAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(stSize);
            nRel = GetAdaptersInfo(pIpAdapterInfo,&stSize);
        }

        if (ERROR_SUCCESS != nRel)
        {
            FREE(pIpAdapterInfo);
            return "";
        }

        IP_ADAPTER_INFO *pAdapter = pIpAdapterInfo;
        while(pAdapter)
        {
            if (MIB_IF_TYPE_ETHERNET != pAdapter->Type)
            {
                pAdapter = pAdapter->Next;
                continue;
            }

            char mac[32] = {0};
            for (UINT i = 0; i < pAdapter->AddressLength; i++)
            {
                sprintf_s(mac+3*i, 4, "%02x-", (unsigned char)pAdapter->Address[i]);
            }
            mac[strlen(mac)-1] = '\0';

            setMac.insert(mac);
            pAdapter = pAdapter->Next;
        }

        if (pIpAdapterInfo)
        {
            FREE(pIpAdapterInfo);
        }
#else
        struct ifaddrs *ifaddr = NULL;
        if (getifaddrs(&ifaddr) == -1)
        {
            return "";
        }

        struct ifaddrs *ifa = NULL;
        for (ifa = ifaddr; ifa; ifa = ifa->ifa_next)  
        {
            if (ifa->ifa_addr == NULL)
                continue;

            int family = ifa->ifa_addr->sa_family;

            switch (family)
            {
            case AF_PACKET:
                {
                    struct sockaddr_ll *s = (struct sockaddr_ll*) ifa->ifa_addr;
                    if (s->sll_hatype == ARPHRD_ETHER)
                    {
                        char mac[32] = {0};
                        for (int i = 0; i < 6; i++)
                        {
                            sprintf(mac+3*i, "%02x-", (unsigned char)s->sll_addr[i]);
                        }
                        mac[strlen(mac)-1] = '\0';
                        setMac.insert(std::string(mac));
                    }
                    break;
                }

            default:
                {
                    break;
                }
            }
        }

        freeifaddrs(ifaddr);

#endif

        std::string sRtn;
        for (std::set<std::string>::iterator it
            = setMac.begin(); it != setMac.end(); ++it)
        {
            sRtn += (*it) + "|";
        }

        return sRtn.empty() ? "" : sRtn.substr(0, sRtn.size()-1);
    }

    void server::start_accept()
    {
        connection_ptr new_connection_(new connection(
            io_service_pool_.get_io_service(),
            io_service_pool_.get_msg_handle_service(),
            io_service_pool_.get_reply_handle_service(), callback_handlers_, connection_manager_));

        acceptor_.async_accept(new_connection_->socket(),
            boost::bind(&server::handle_accept, this, new_connection_,
            boost::asio::placeholders::error));
    }

    void server::handle_accept(connection_ptr pConnection, const boost::system::error_code& e)
    {
        if (!acceptor_.is_open())
        {
            return;
        }

        if (!e)
        {
            pConnection->start();
        }

        start_accept();
    }

    void server::stop()
    {
        boost::system::error_code egnore;
        acceptor_.close(egnore);
    } 

}