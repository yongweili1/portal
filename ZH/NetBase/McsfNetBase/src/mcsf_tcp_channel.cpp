#include "stdafx.h"
#include "boost/crc.hpp"
#include "boost/timer.hpp"

namespace Mcsf {


    TcpChannel::TcpChannel( Task_Base_Ptr pTask, const std::string& sOpposite ) 
        : m_pTask (pTask)
        , m_sOpposite(sOpposite)
    {
        NTB_TRACE_INFO_WITH_THIS("TcpChannel class constructor enter.");
        CErrorHandler::RegisterToDump(this);
    }
    
    TcpChannel::~TcpChannel()
    {
        NTB_TRACE_INFO_WITH_THIS("TcpChannel class destructor enter.");
        CErrorHandler::UnRegisterToDump(this);
        //m_pTask.reset();
    }

    int TcpChannel::Send(ACE_Message_Block* pMb)
    {
        //synchronous method call to protect asynchronous server command 
        //process is correct
        boost::timed_mutex::scoped_lock lock(m_Mutex, boost::posix_time::milliseconds(kHeaderTimeout.msec()));
        if (lock.owns_lock())
        {
            if (m_peerStream.send_n(pMb) == -1)
            {
                NTB_LOG_ERROR_WITH_THIS("send_n error.m_sOpposite=" << m_sOpposite << ":" << GetRemoteAddr());
                return -1;
            }

            return 0;
        }
        else
        {
            NTB_LOG_ERROR_WITH_THIS("send_n can't get lock.m_sOpposite = " << m_sOpposite << ":" << GetRemoteAddr());
            return -1;
        }
    }

    int TcpChannel::Send(RpcMsgPackPtr pPack, const ACE_Time_Value *tv)
    {
       // NTB_TRACE_INFO_WITH_THIS("[package]Channel send package enter.");

        SentBuff tupBuff = pPack->GetSentBuff();
        boost::shared_array<char> pSequenceBuff = tupBuff.get<0>();
        if (NULL == pSequenceBuff)
        {
            NTB_LOG_ERROR_WITH_THIS( "[package]Send error. pSequenceBuff==Null");
            return -1;
        }
        boost::timed_mutex::scoped_lock lock(m_Mutex, boost::posix_time::milliseconds(tv->msec()));
        if (lock.owns_lock())
        { 
            // ACE_GUARD_RETURN( ACE_Thread_Mutex, localObj, m_Mutex , -1 );
            if (m_peerStream.send_n(pSequenceBuff.get(), tupBuff.get<1>(), tv) == -1)
            {
                NTB_LOG_ERROR_WITH_THIS("[package]send sequence buff error.m_sOpposite=" << m_sOpposite << " last error=" << ACE_OS::last_error());
                return -1;
            }

            const std::string *pRawBuff = pPack->GetRawBody();
            if (pRawBuff)
            {
                if (m_peerStream.send_n(&*pRawBuff->begin(), pRawBuff->length(), tv) == -1)
                {
                    NTB_LOG_ERROR_WITH_THIS("[package]send raw buff error.m_sOpposite=" << m_sOpposite << " last error=" << ACE_OS::last_error());
                    return -1;
                }
            }

            return 0;
        }
        else
        {
            NTB_LOG_ERROR_WITH_THIS("[package]send sequence buff error.m_sOpposite=" << m_sOpposite << " last error=can't get lock in " << tv->msec());
            return -1;
        }
    }

    int TcpChannel::SendACK(int msgType)
    {
        const ACE_Time_Value tv(0, 100 * 1000);
        RpcMsgPackPtr pck(new RpcMsgPackage);
        pck->SetSentFields(0, 0, "", true, msgType);
        return Send(pck, &tv);
    }

    RpcMsgPackage *TcpChannel::Receive()
    {
        RpcMsgPackage *pRecvRtn(new RpcMsgPackage);

        //receive header
        RpcMsgHeaderPtr pRecvHeader(new RpcMsgHeader);
        ssize_t iReadLen = sizeof(RpcMsgHeader);

        ssize_t iBytesTrans = m_peerStream.recv_n(pRecvHeader.get(), iReadLen, &kHeaderTimeout);
        if (iBytesTrans != iReadLen)
        {
            NTB_LOG_ERROR_WITH_THIS( "recv header error. iBytesTrans="<<iBytesTrans
                <<" last error="<<ACE_OS::last_error()<<" m_sOpposite="<<m_sOpposite);
            DEL_PTR(pRecvRtn);
            return NULL;
        }

        ReceivedBuff pRecvBuff;
        iReadLen = pRecvHeader->uDataLen + pRecvHeader->uSenderLen;
        if (iReadLen)
        {
            pRecvBuff.reset(new std::string);
            pRecvBuff->reserve(iReadLen);
            pRecvBuff->resize(iReadLen);

            char *paddr = &*pRecvBuff->begin();

            if(pRecvHeader->bSyncBig)
            {
                iBytesTrans = m_peerStream.recv_n(paddr + pRecvHeader->uDataLen, pRecvHeader->uSenderLen, &kContentTimeout);

                if (iBytesTrans != pRecvHeader->uSenderLen)
                {
                    NTB_LOG_ERROR_WITH_THIS( "recv sender error. iBytesTrans="<<iBytesTrans
                        <<" last error="<<ACE_OS::last_error()<<" m_sOpposite="<<m_sOpposite);
                    DEL_PTR(pRecvRtn);
                    return NULL;
                }

                iReadLen = pRecvHeader->uDataLen;
            }

            iBytesTrans = m_peerStream.recv_n(paddr, iReadLen, &kContentTimeout);
            if (iBytesTrans != iReadLen)
            {
                NTB_LOG_ERROR_WITH_THIS( "recv sender / data error. iBytesTrans="<<iBytesTrans
                    <<" last error="<<ACE_OS::last_error()<<" m_sOpposite="<<m_sOpposite);
                DEL_PTR(pRecvRtn);
                return NULL;
            }
        }

        pRecvRtn->SetReceivedFields(pRecvHeader, pRecvBuff);

        return pRecvRtn;
    }

    TcpChannel* TcpChannel::CreateTcpChannel( Task_Base_Ptr pTask , const std::string& sOpposite)
    {
        NTB_TRACE_INFO("TcpChannel::CreateTcpChannel enter.");
        if( boost::dynamic_pointer_cast< TcpServer >(pTask) != NULL )
        {
            return new ServerTcpChannel(pTask, sOpposite);
        } 

        return new ClientTcpChannel(pTask, sOpposite);
    }

    std::string TcpChannel::GetRemoteAddr(void) const 
    {
        NTB_TRACE_INFO_WITH_THIS("GetRemoteAddr enter.");
        static const int iIPSize = 16;
        ACE_INET_Addr remoteAddr;
        m_peerStream.get_remote_addr( remoteAddr );
        char sIPTemp[iIPSize];
        memset(sIPTemp, '\0', iIPSize);
        std::string sIp = remoteAddr.get_host_addr(sIPTemp, iIPSize);
        if (sIp.empty() )
        {
            NTB_LOG_ERROR_WITH_THIS("GetRemoteAddr get_host_addr failed.m_sOpposite="<<m_sOpposite);
            return "";
        }

        std::string sPort = boost::lexical_cast<std::string>(remoteAddr.get_port_number());
        NTB_TRACE_INFO_WITH_THIS("GetRemoteAddr sIP="<<sIp<<" sPort="<<sPort);
        return sIp + ":" + sPort;
    }

    std::string TcpChannel::Dump()
    {
        MCSF_TRY
            MCSF_DUMP(m_sDumpInfo, "(void)*", "this:", this);
            MCSF_DUMP(m_sDumpInfo,"Task_Base_Ptr", "m_pTask:", m_pTask);
            MCSF_DUMP(m_sDumpInfo, "std::string", "m_sOpposite:", m_sOpposite);
            return m_sDumpInfo;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Dump occurs exception again.");
            return "";
        MCSF_TRY_CATCH_END
    }

    //////////////////////////////////////////////////////////////////////////
    // ServerTcpChannel
    //////////////////////////////////////////////////////////////////////////
    ServerTcpChannel::ServerTcpChannel( Task_Base_Ptr pTask , const std::string& sOpposite)
        : TcpChannel( pTask, sOpposite )
        , m_bValidConnection(false)
    {
        NTB_TRACE_INFO_WITH_THIS("ServerTcpChannel::ServerTcpChannel enter.");
    }

    bool ServerTcpChannel::ReadVeriInfo()
    {
        NTB_TRACE_INFO_WITH_THIS("Server ReadVeriInfo enter.");
        const static int iSize=64;
        char buf[iSize+1] = {0};
        ACE_Time_Value tm(10);//1s
        ACE_INT32 iLength = m_peerStream.recv_n(buf, iSize, &tm);

        if (iLength != iSize)
        {
            ACE_INT32 nErr = ACE_OS::last_error();
            NTB_LOG_ERROR_WITH_THIS("the verification info is wrong.iLength="<<iLength<<" last_error="<<nErr);
            return false;
        }
        //verify
        boost::crc_optimal<8, 0x1021> crc_8bit;
        crc_8bit.process_bytes(buf, iSize-1);
        char cTemp = crc_8bit.checksum()+'0';
        if (cTemp != buf[iSize-1])
        {
            NTB_LOG_ERROR_WITH_THIS("the verification info is wrong.iLength="<<iLength);
            return false;
        }
        NTB_TRACE_INFO_WITH_THIS("Server ReadVeriInfo succeed.");

        return true;
    }

    int ServerTcpChannel::HandleRead()
    {
 //       NTB_TRACE_INFO_WITH_THIS("Server HandleRead enter.");
        ACE_Message_Block* mbTcpChannel = NULL;
        RpcMsgPackage *pRevPack = NULL;
        MCSF_TRY
            if (!m_bValidConnection)
            {
                m_bValidConnection = ReadVeriInfo();
                if (!m_bValidConnection)
                {
                    // std::cout<<"wrong conn"<<std::endl;
                    // invalid connection
                    NTB_LOG_ERROR_WITH_THIS("the connection is invliad.");

                    return -1;
                } 
                return McsfSuccess;
            }

            pRevPack = Receive();
            if (NULL == pRevPack)
                return McsfUndefineError;

            RpcMsgHeader *ph = pRevPack->GetHeader();
            if (ph->iMsgType == message_type_ack_request)
            {
                DEL_PTR(pRevPack);
                return 0;
            }

            ServerTcpChannel *pTcpChannel = this;
            mbTcpChannel = new ACE_Message_Block(sizeof(mbTcpChannel) + sizeof(pRevPack));
            memcpy( mbTcpChannel->wr_ptr(), &pTcpChannel, sizeof(mbTcpChannel) );
            mbTcpChannel->wr_ptr( sizeof(mbTcpChannel) );
            memcpy( mbTcpChannel->wr_ptr(), &pRevPack, sizeof(pRevPack));
            mbTcpChannel->wr_ptr( sizeof(pRevPack) );

            TcpServer_Ptr pTask = boost::dynamic_pointer_cast< TcpServer >(m_pTask);
            if( pTask == NULL )
            {
                NTB_LOG_ERROR_WITH_THIS( "pTask is NULL error.m_sOpposite="<<m_sOpposite);
                mbTcpChannel->release();
                DEL_PTR(pRevPack);
                return McsfUndefineError;
            }

            NTB_TRACE_INFO_WITH_THIS( "receive some data from " << GetRemoteAddr() << " for request. ") ;
            pTcpChannel->Duplicate();
            pTask->putq( mbTcpChannel );

            return McsfSuccess;
        
        MCSF_CATCH(Mcsf::Exception)
            NTB_LOG_ERROR_WITH_THIS(e.what()<<" m_sOpposite="<<m_sOpposite);
            if(mbTcpChannel) mbTcpChannel->release();
            DEL_PTR(pRevPack);
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("ServerTcpChannel::HandleRead occurs exception.m_sOpposite="<<m_sOpposite);
            if(mbTcpChannel) mbTcpChannel->release();
            DEL_PTR(pRevPack);
        MCSF_TRY_CATCH_END

        return McsfUndefineError;
    }

    int ServerTcpChannel::HandleClose()
    {
        NTB_TRACE_INFO_WITH_THIS( "ServerTcpChannel::HandleClose enter.one client connection is closed.");
        TcpServer_Ptr pTask = boost::dynamic_pointer_cast< TcpServer >(m_pTask);
        if( pTask == NULL )
        {
            NTB_LOG_ERROR_WITH_THIS( "pTask is NULL error.m_sOpposite="<<m_sOpposite);
            return McsfUndefineError;
        }
        pTask->RemoveServer(this);
        pTask->UnloadContaineeInDispt(this);
        return 0;
    }

    //////////////////////////////////////////////////////////////////////////
    //ClientTcpChannel
    //////////////////////////////////////////////////////////////////////////

    ClientTcpChannel::ClientTcpChannel( Task_Base_Ptr pTask , const std::string& sOpposite) 
        : TcpChannel( pTask , sOpposite)
    {
        NTB_TRACE_INFO_WITH_THIS("ClientTcpChannel::ClientTcpChannel enter.");
    }

    int ClientTcpChannel::HandleRead()
    {
  //      NTB_TRACE_INFO_WITH_THIS("Client HandleRead enter.");
        MCSF_TRY
            RpcMsgPackPtr pClientPck(Receive());

            if (NULL == pClientPck)
                return McsfUndefineError;

            RpcMsgHeader *ph = pClientPck->GetHeader();
            if (ph->iMsgType == message_type_ack_response)
                return McsfSuccess;

            //boost::dynamic_pointer_cast, pointer cast up from base class to subclass
            TcpClient_Ptr pTask = boost::dynamic_pointer_cast< TcpClient >(m_pTask);

            pTask->ReceiveResponse(pClientPck);

            NTB_TRACE_INFO_WITH_THIS( "receive some data from " << GetRemoteAddr() << " for response" );
            return McsfSuccess;
        
        MCSF_CATCH(Mcsf::Exception)
            NTB_LOG_ERROR_WITH_THIS(e.what());
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("ClientTcpChannel::HandleRead occurs exception.m_sOpposite="<<m_sOpposite);
        MCSF_TRY_CATCH_END

        return McsfUndefineError;
    }

    int ClientTcpChannel::HandleClose()
    {
        int iRtn = McsfSuccess;
        NTB_LOG_INFO_WITH_THIS("ClientTcpChannel::HandleClose enter" );
        TcpClient_Ptr pClient = boost::dynamic_pointer_cast< TcpClient >(m_pTask);
        if( pClient == NULL )
        {
            NTB_LOG_ERROR_WITH_THIS( "a critical error happen.m_sOpposite="<<m_sOpposite);
            return McsfUndefineError;
        }
        ACE_Message_Block *mb=NULL;
        MCSF_TRY
            //Send an message to queue to exit thread.
            mb = new ACE_Message_Block();
            CHECK_NULL_PTR(mb);
            mb->msg_type( ACE_Message_Block::MB_STOP );
            pClient->putq( mb );
            if (0 != pClient->wait())
            {
                NTB_LOG_ERROR_WITH_THIS("wait thread exit failed.m_sOpposite="<<m_sOpposite);
                iRtn = McsfUndefineError;
            }
            else
            {
                pClient->GetClientManager()->UnRegisterClient(m_sOpposite);
                NTB_LOG_INFO_WITH_THIS("UnRegisterClient OK.m_sOpposite="<<m_sOpposite);
                if (m_sOpposite == sSystemDispatchName)
                {
                    NTB_TRACE_INFO_WITH_THIS("lost connecttion with system dispatcher,try to reconnect it.");
                    pClient->GetClientManager()->TrigReconnection();
                }
                
                pClient->ClearDataFile();
            }

            return iRtn;

        MCSF_CATCH(Mcsf::Exception)
            NTB_LOG_ERROR_WITH_THIS(e.what()<<" m_sOpposite="<<m_sOpposite);
        
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("ClientTcpChannel::HandleClose occurs exception.m_sOpposite="<<m_sOpposite);
        MCSF_TRY_CATCH_END
        
        return McsfUndefineError;
    }


}