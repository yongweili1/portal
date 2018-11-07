#include "stdafx.h"


namespace
{
    const int MAX_HOSTNAME_LONG = 256;
}

namespace Mcsf {

    Acceptor::Acceptor( Task_Base_Ptr pServer, ClientManager* pClientMgr) 
        : ACE_Event_Handler(pClientMgr->GetReactor()), m_pServer(pServer),m_pClientMgr(pClientMgr)
    {
        CErrorHandler::RegisterToDump(this);
        NTB_TRACE_INFO_WITH_THIS("Acceptor constructor.");
    }

    Acceptor::~Acceptor()
    {
        CErrorHandler::UnRegisterToDump(this);
    }
    int Acceptor::open( const ACE_INET_Addr &local_addr )
    {
        NTB_LOG_INFO_WITH_THIS("Acceptor open enter.local addr = "<<local_addr.get_addr());
//        boost::mutex::scoped_lock lock(m_mxAcceptor);
        if ( m_acceptor.open( local_addr, 1 ) == -1 )
        {
            return McsfSocketListenFailed;
        }

        return m_pClientMgr->GetReactor()->register_handler( this, ACE_Event_Handler::ACCEPT_MASK ) == -1 ? 
            McsfRegisterReactorFailed : McsfSuccess;
    }

    int Acceptor::handle_input( ACE_HANDLE )
    {
        TcpHandler* peer_handler = NULL;
        ACE_INET_Addr remoteAddr;

        MCSF_TRY
            NTB_LOG_INFO_WITH_THIS("new connection comes in. ");

            peer_handler =  new TcpHandler( m_pClientMgr->GetReactor(), m_pServer );
            CHECK_NULL_PTR(peer_handler);
            m_pClientMgr->AddSvrTcpHandler(peer_handler->GetChannel(), peer_handler);

            if( m_acceptor.accept( peer_handler->peer() ) == -1 )
            {
                peer_handler->peer().get_remote_addr( remoteAddr );
                NTB_LOG_ERROR_WITH_THIS( "accept " << remoteAddr.get_host_addr()<<" failed.");
                peer_handler->handle_close();
                return McsfReactorRemoveHandler;
            }
            else if ( peer_handler->open() == -1 )
            {
                peer_handler->handle_close();
                NTB_LOG_ERROR_WITH_THIS( "peer_handler->open() == -1 ");
                //peer_handler->Release();
                return McsfReactorRemoveHandler;
            }
            peer_handler->peer().get_remote_addr( remoteAddr );

            NTB_LOG_INFO_WITH_THIS( "accept " << remoteAddr.get_host_addr()<<":"
                <<remoteAddr.get_port_number()<<" succeed.");
            return 0;
        MCSF_CATCH (Mcsf::Exception)
            NTB_LOG_ERROR_WITH_THIS(e.what()<<" m_pServer="<<m_pServer<<" m_pClientMgr="<<m_pClientMgr);
            //peer_handler->Release();
            peer_handler->handle_close();
            return McsfReactorRemoveHandler;
        MCSF_CATCH_ALL        
            NTB_LOG_ERROR_WITH_THIS("Acceptor handle_input occur exception."<<" m_pServer="<<m_pServer<<" m_pClientMgr="<<m_pClientMgr);
            //peer_handler->Release();
            peer_handler->handle_close();
            return McsfReactorRemoveHandler;        
        MCSF_TRY_CATCH_END
    }

    int Acceptor::handle_close( ACE_HANDLE , ACE_Reactor_Mask mask )
    {
        NTB_LOG_INFO_WITH_THIS("Acceptor handle_close enter.");
//        boost::mutex::scoped_lock lock(m_mxAcceptor);
        if( mask == 0 )
        {
            mask = ACE_Event_Handler::ALL_EVENTS_MASK | 
                ACE_Event_Handler::DONT_CALL;
            m_pClientMgr->GetReactor()->remove_handler( this, mask );
        }
        m_acceptor.close();

        return 0;
    }

    std::string Acceptor::GetListenAddr() const
    {
        ACE_INET_Addr listenAddr;
        m_acceptor.get_local_addr( listenAddr );

        if ( listenAddr.is_any() )
        {
            std::string sLocalAddr = TcpServer::GetIpInterface() + ":"
                + boost::lexical_cast<std::string>(listenAddr.get_port_number());
            return sLocalAddr;
        }

        ACE_TCHAR buffer[MAX_HOSTNAME_LONG] = { 0 };
        listenAddr.addr_to_string( buffer, MAX_HOSTNAME_LONG );
        return buffer;
    }

    std::string Acceptor::GetListenPort() const
    {
        ACE_INET_Addr listenAddr;
        m_acceptor.get_local_addr( listenAddr );
        std::string sListenPort = boost::lexical_cast<std::string>(listenAddr.get_port_number());
        return sListenPort;
    }
    
    std::string Acceptor::Dump()
    {
        MCSF_TRY
            MCSF_DUMP(m_sDumpInfo, "(void)*", "this: ", this );
            MCSF_DUMP(m_sDumpInfo, "Task_Base*", "m_pServer:", m_pServer);
            MCSF_DUMP(m_sDumpInfo, "ClientManager*", "m_pClientMgr:", m_pClientMgr);
            return m_sDumpInfo;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Dump occurs exception again.");
            return ""; 
        MCSF_TRY_CATCH_END

    }
}

