#include "stdafx.h"

#ifdef ACE_WIN32
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

namespace Mcsf {

    TcpServer::TcpServer(ClientManager* pClientMgr) 
        : m_pCB(NULL)
        , m_pAcceptor(NULL)
        ,m_pClientMgr(pClientMgr)
    {
        CErrorHandler::RegisterToDump(this);
        NTB_TRACE_INFO_WITH_THIS("TcpServer Constructor enter.");
        this->msg_queue()->high_water_mark(1024*1024*16);
        this->msg_queue()->low_water_mark(1024*1024*16);
    }
    
    TcpServer::~TcpServer()
    {
        CErrorHandler::UnRegisterToDump(this);
    }
    
    int TcpServer::Init(TcpServer_Ptr pServer, int iPoolSize)
    {
        NTB_LOG_INFO_WITH_THIS("TcpServer::Init.");
        int tmpRtn = this->activate( THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, iPoolSize );
        if (0 != tmpRtn)
        {
            NTB_LOG_ERROR_WITH_THIS("TcpServer allocate thread failed! tmpRtn=" << tmpRtn);
            return tmpRtn;
        }
        //don't delete, it will delete itself in handle_close
        m_pAcceptor = new Acceptor( pServer, m_pClientMgr);
        CHECK_NULL_PTR(m_pAcceptor);
        return m_pAcceptor->open( m_ListenAddr );
    }

    int TcpServer::Fini()
    {
        NTB_LOG_INFO_WITH_THIS("TcpServer::Fini.");
        //if server process not exit, the messages have already been in queue
        //will be execute until MB_STOP message has been received
        int iRet = McsfUndefineError;
        if (m_pAcceptor)
        {
            // 1. stop listen thread
            iRet = m_pAcceptor->handle_close();
        }

        // 2. stop svc tread
        ACE_Message_Block *mb=NULL;
        mb = new ACE_Message_Block();
        CHECK_NULL_PTR(mb);
        mb->msg_type( ACE_Message_Block::MB_STOP );
        this->putq( mb );
        if (0 != this->wait())
        {
            NTB_LOG_ERROR_WITH_THIS("wait thread exit failed."<<" m_pCB="<<m_pCB
                <<" m_pAcceptor="<<m_pAcceptor<<" m_pClientMgr="<<m_pClientMgr);
        }
        ClearAllFinishedAsyncSvrCmdProcess();
        DEL_PTR(m_pAcceptor);
        return iRet;
    }

    void TcpServer::PreHandle(const std::string &sender, TcpChannel *pTcp)
    {
        //sender and it's connection address
        std::pair<std::string, std::string> pairSenderIpAddr;
        pairSenderIpAddr.first = sender;
        pairSenderIpAddr.second = pTcp->GetRemoteAddr();

        NTB_TRACE_INFO_WITH_THIS("sender="<<pairSenderIpAddr.first<<" ip_addr="<<pairSenderIpAddr.second);
        {
            boost::mutex::scoped_lock lock(m_mxMap);
            m_mapChannel2Name[pTcp] = pairSenderIpAddr;
        }
        {
            boost::mutex::scoped_lock lock(m_cbMutex);
            EVENTTYPE2CALLBACKMAP::iterator itr = m_fpc.find(EVENTTYPE_CONNECT_ADDR);
            if (itr != m_fpc.end())
            {
                // call back function of SD to record sender and it's connection address
                itr->second((void*)&(pairSenderIpAddr));
            }
        }
    }

    int TcpServer::svc()
    {
        NTB_LOG_INFO_WITH_THIS("server thread start.");
        AsycServiceCmdProcess* pCmdProcess = NULL;
        TcpChannel* pChannel = NULL;
        MCSF_TRY
            while ( true )
            {
                ClearFinishedAsyncSvrCmdProcess();
                ACE_Message_Block *mb = NULL;
                if( -1 == getq( mb ) )
                {
                    NTB_LOG_ERROR_WITH_THIS("Get queue error, server svc will exit.");
                    return 0;
                }
                CHECK_NULL_PTR(mb);

                if( mb->msg_type() == ACE_Message_Block::MB_STOP )
                {
                    mb->release();
                    break;
                }

                memcpy( &pChannel, mb->rd_ptr(), sizeof( pChannel ) );
                if (NULL == pChannel)
                {
                    NTB_LOG_ERROR_WITH_THIS("pChannel is null.");
                    mb->release();
                    continue;
                }
                mb->rd_ptr(sizeof(pChannel));

                RpcMsgPackage *pRevPack = NULL;
                memcpy(&pRevPack, mb->rd_ptr(), sizeof(pRevPack));
                if (NULL == pRevPack)
                {
                    NTB_LOG_ERROR_WITH_THIS("pRevPack is null.");
                    mb->release();
                    pChannel->Release();
                    continue;
                }
                //release ACE_Message_Block memory
                mb->release();

                RpcMsgPackPtr pRpcMsg(pRevPack);

                RpcMsgHeader *pHeader = pRpcMsg->GetHeader();
                if (message_type_cmd == pHeader->iMsgType)
                {
                    // check if is asynchronous call  
                    if( pHeader->bRsrv )
                    {
                        pCmdProcess = new AsycServiceCmdProcess( pRpcMsg, pChannel, m_pCB, this);
                        CHECK_NULL_PTR(pCmdProcess);
                        AddAsyncSvrCmdProcess(pCmdProcess);
                        pCmdProcess->Start();
                        continue;
                    }

                    const uint32_t &cmdevtid = pHeader->id;
                    const uint32_t &uid = pHeader->uid;
                    const std::string &sender = pRpcMsg->GetSender();

                    if (cmdevtid == SYSTEM_COMMAND_ID_REGIST_CONTAINEE)
                        PreHandle(sender, pChannel);

                    if (SYSTEM_COMMAND_ID_SYS_MANAGER_GET_PROCESS != cmdevtid)
                    {
                        NTB_LOG_INFO("begin c=" << cmdevtid << " u=" << uid << " s=" << sender);
                    }

                    std::string sResponse;
                    int iRtn = m_pCB->HandleRpcCommand(pRpcMsg, &sResponse, pChannel);

                    if (SYSTEM_COMMAND_ID_SYS_MANAGER_GET_PROCESS != cmdevtid)
                    {
                        NTB_LOG_INFO("end c=" << cmdevtid << " u=" << uid << " s=" << sender);
                    }

                    if ( IServerProcessCB::Not_Handler == iRtn  || IServerProcessCB::Have_Exception == iRtn)
                    {
                        sResponse.clear();

                        RpcMsgPackPtr pResMsg(new RpcMsgPackage);

                        if (cmdevtid == SYSTEM_COMMAND_ID_SHAREMEMORY_BIGDATA_TRANSFER)
                        {
                            // release share memory here?
                            McsfSendDataRequest request;
                            ReceivedBuff pData = pResMsg->GetReceivedBuff();
                            request.ParseFromString(pData ? *pData : "") ;
                            if( request.type() == McsfSendDataRequest_SendType_SHARE_MEMORY )
                            {
                                McsfSendDataResponse response;
                                response.set_is_success(false);
                                response.SerializeToString(&sResponse);

                                pResMsg->SetSentFields(uid, cmdevtid, sResponse, true);

                                NTB_TRACE_INFO_WITH_THIS("begin to send.channel="<<pChannel);
                                pChannel->Send( pResMsg );
                                NTB_TRACE_INFO_WITH_THIS("end to send.channel="<<pChannel);
                            }

                            pChannel->Release();
                            continue;
                        }

                        pResMsg->SetSentFields(uid, cmdevtid, sResponse, true, 0, "" , false, 
                            IServerProcessCB::Not_Handler == iRtn
                            ? McsfNotRegisterCommandHandler : McsfHandlerHasException);
                        pChannel->Send(pResMsg);
                    }
                    else if ( IServerProcessCB::Success == iRtn )
                    {
                        NTB_TRACE_INFO_WITH_THIS("server begin to send result to " << sender
                            <<" cmd_id="<<cmdevtid << " uid=" << uid);

                        RpcMsgPackPtr pResMsg(new RpcMsgPackage);
                        pResMsg->SetSentFields(uid, cmdevtid, sResponse, true);

                        pChannel->Send( pResMsg );

                        NTB_TRACE_INFO_WITH_THIS("server end to send result to "<<sender
                            <<" cmd_id="<<cmdevtid << " uid=" << uid);
                    }
                    else //CommandEx, do not release channel.
                    {
                        continue;
                    }//end return type
                }//end data_type_cmd
                else if (message_type_event == pHeader->iMsgType)
                {
                    m_pCB->HandleRpcEvent(pRpcMsg);
                }

                NTB_TRACE_INFO_WITH_THIS("TcpServer::svc release channel="<<pChannel);
                pChannel->Release();
            }
            NTB_LOG_INFO_WITH_THIS( "TcpServer Thread will exit." );
            return McsfSuccess;
        
        MCSF_CATCH(Mcsf::Exception)
            DEL_PTR(pCmdProcess);
            if (NULL != pChannel)
            {
                pChannel->Release();
            }

            NTB_LOG_ERROR_WITH_THIS(e.what()<<" m_pCB="<<m_pCB
                <<" m_pAcceptor="<<m_pAcceptor<<" m_pClientMgr="<<m_pClientMgr);
        MCSF_CATCH_ALL
            DEL_PTR(pCmdProcess);
            if (NULL != pChannel)
            {
                pChannel->Release();
            }

            NTB_LOG_ERROR_WITH_THIS("TcpServer::svc occrus exception."<<"m_pCB="<<m_pCB
                <<" m_pAcceptor="<<m_pAcceptor<<" m_pClientMgr="<<m_pClientMgr);
        MCSF_TRY_CATCH_END

        return McsfUndefineError;
    }

    int TcpServer::RegisterCallBack( IServerProcessCB* pCB )
    {
//        NTB_TRACE_INFO_WITH_THIS("register callback in server enter.");
        m_pCB = pCB;
        return 0;
    }

    std::string TcpServer::GetListenPort()
    {
//        NTB_TRACE_INFO_WITH_THIS("TcpServer::GetListenAddr enter.");
        return NULL == m_pAcceptor ? "" : m_pAcceptor->GetListenPort();
    }

    std::string TcpServer::GetListenAddr()
    {
//        NTB_TRACE_INFO_WITH_THIS("TcpServer::GetListenAddr enter.");
        return NULL == m_pAcceptor ? "" : m_pAcceptor->GetListenAddr();
    }

    void TcpServer::SetListenAddr(const std::string& sIpAddr) 
    {
 //       NTB_TRACE_INFO_WITH_THIS("set listen address enter.sIpAddr="<<sIpAddr);
        // listen all ip address
        std::string sLocalIpAddr = "0";

        // if not : we assume it is string represent ip 
        if( sIpAddr.find( ":" ) == std::string::npos )
        {
            sLocalIpAddr += ":0";
        }
        else
        {
            sLocalIpAddr += sIpAddr.substr(sIpAddr.find(":"));
        }

        m_ListenAddr.set( sLocalIpAddr.c_str() );
    }

    //TO how to deal with multi network ip address
    std::string TcpServer::GetIpInterface()
    {
//        NTB_TRACE_INFO("TcpServer::GetIpInterface enter.");
        static const std::string sLocalHost = "127.0.0.1";
        ACE_INET_Addr* addr_array; 
        size_t count = 0; 

        if (ACE::get_ip_interfaces(count, addr_array) != 0)
        {
            return ""; 
        }

        std::string sAddress;
        std::string sTemp;
        ACE_INET_Addr* addr_array2 = addr_array;
        for( size_t iIndex = 0; iIndex < count; ++iIndex )
        {
            sTemp = addr_array2->get_host_addr();
            if (!sTemp.empty() && sTemp.compare(sLocalHost))
            {
                sAddress += sTemp;
                sAddress += "&";
            }
            ++addr_array2;
            sTemp.clear();
        }
        if (!sAddress.empty())
        {
            sAddress = sAddress.substr(0, sAddress.length()-1);
        }
        else
        {
            sAddress = sLocalHost;
        }
        delete[] addr_array;
//        NTB_TRACE_INFO("TcpServer::GetIpInterface sAddress="<<sAddress);
        return sAddress;        
    }

    void TcpServer::SetCallBack( EVENTTYPE eEvtType, boost::function1<void,void*> ufpc )
    {
        ACE_UNUSED_ARG(eEvtType);
        boost::mutex::scoped_lock lock(m_cbMutex);
        m_fpc[eEvtType] = ufpc;
    }

    void TcpServer::CancelCallBack( EVENTTYPE eEvtType )
    {
        ACE_UNUSED_ARG(eEvtType);
        boost::mutex::scoped_lock lock(m_cbMutex);
        m_fpc.erase(eEvtType);
    }

    void TcpServer::UnloadContaineeInDispt( TcpChannel* pChannel )
    {
        std::pair<std::string, std::string> sNameIPPort;
        map2pair::iterator itfind;
        {
            boost::mutex::scoped_lock lock(m_mxMap);
            itfind = m_mapChannel2Name.find(pChannel);
            if (itfind == m_mapChannel2Name.end())
            {
                return ;
            }
            sNameIPPort= itfind->second;
            m_mapChannel2Name.erase(itfind);
        }

        NTB_LOG_INFO_WITH_THIS( sNameIPPort.first << " unload containee. pChannel="<<pChannel);
        {
            boost::mutex::scoped_lock lock(m_cbMutex);
            EVENTTYPE2CALLBACKMAP::iterator itr = m_fpc.find(EVENTTYPE_DISCONNECT);
            if (itr != m_fpc.end())
            {
                itr->second((void*)&(sNameIPPort));
            }
        }
    }

    void TcpServer::RemoveServer(TcpChannel* pChannel)
    {
        m_pClientMgr->RemoveServer(pChannel);
    }

    std::string TcpServer::Dump()
    {
        MCSF_TRY
            ostringstream om;
            char* dot = ", ";
            om << "this:" << this << endl;
            om << " server listen address:" << m_ListenAddr.get_ip_address() << endl;
            om << " m_pCB: " << m_pCB << endl;
            om << " m_pClientMgr:" << m_pClientMgr << endl;

            boost::mutex::scoped_lock lock(m_mxMap);
            for( map2pair::iterator it = m_mapChannel2Name.begin();
                it != m_mapChannel2Name.end(); ++it )
            {
                om <<  it->first << dot << it->second.first<<dot<<it->second.second << std::endl;
            }

            m_sDumpInfo = om.str();
            return m_sDumpInfo;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Dump occurs exception again.");
            return "";
        MCSF_TRY_CATCH_END
    }


    void TcpServer::LoopEraseFinishedSvr()
    {
        for (std::map<AsycServiceCmdProcess*, bool>::iterator it =
            m_mapAsynSvrCmd.begin(); it != m_mapAsynSvrCmd.end();)
        {
            if (it->second)
            {
                it->first->Stop();
                delete it->first;
                m_mapAsynSvrCmd.erase(it++);
            }
            else
            {
                ++it;
            }
        }
    }

    void TcpServer::ClearFinishedAsyncSvrCmdProcess()
    {
        boost::mutex::scoped_lock lock(m_svrMapMutex);
        LoopEraseFinishedSvr();
    }

    void TcpServer::UpdateFinishedSvr( AsycServiceCmdProcess *pAsyncSvrCmdProcess )
    {
        boost::mutex::scoped_lock lock(m_svrMapMutex);

        std::map<AsycServiceCmdProcess*, bool>::iterator itFind = 
            m_mapAsynSvrCmd.find(pAsyncSvrCmdProcess);

        if (m_mapAsynSvrCmd.end() != itFind)
        {
            itFind->second = true;
            m_svrCond.notify_one();
        }
    }

    void TcpServer::AddAsyncSvrCmdProcess( AsycServiceCmdProcess *pAsyncSvrCmdProcess )
    {
        boost::mutex::scoped_lock lock(m_svrMapMutex);
        m_mapAsynSvrCmd[pAsyncSvrCmdProcess] = false;
    }

    void TcpServer::ClearAllFinishedAsyncSvrCmdProcess()
    {
        boost::mutex::scoped_lock lock(m_svrMapMutex);

        LoopEraseFinishedSvr();
        while (!m_mapAsynSvrCmd.empty())
        {
            m_svrCond.wait(lock);
            LoopEraseFinishedSvr();
        }
    }

    std::string TcpServer::GetMacInterface()
    {
        std::set<std::string> setMac;

#ifdef ACE_WIN32
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

}