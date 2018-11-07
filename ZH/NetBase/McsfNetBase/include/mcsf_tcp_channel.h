////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_tcp_channel.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/04
///
/// \brief  Server Tcp communication channel and client tcp communication 
///         channel.
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_TCPCHANNEL_H
#define _MCSF_TCPCHANNEL_H

#include <string>

#include "ace/SOCK_Stream.h"
#include "ace/Thread_Mutex.h"
#include "ace/Task.h"
#include "boost/lexical_cast.hpp"
#include "boost/array.hpp"
#include "boost/thread.hpp"

#include "mcsf_ichannel.h"
#include "mcsf_irefcount.h"
#include "mcsf_tcphandler.h"
#include "mcsf_rpc_msg_redef.h"
#include "McsfErrorHandler/mcsf_error_handler.h"

namespace Mcsf 
{
    const ACE_Time_Value kHeaderTimeout(1);
    const ACE_Time_Value kContentTimeout(30);

    /// \class TcpChannel
    /// \brief receive and send data by socket
    class TcpChannel : public RefCounted<TcpChannel>, public IDumper
    {
    public:
        explicit TcpChannel(Task_Base_Ptr pTask, const std::string& sOpposite);

        ACE_SOCK_Stream &peer () { 
            return m_peerStream;
        }

        /// factory function to create channel base on \a pTask dynamic type
        static TcpChannel* CreateTcpChannel( Task_Base_Ptr pTask, const std::string& sOpposite);
        /// handler when io read happen
        virtual int HandleRead() = 0;
        /// handler when io close signal happen
        virtual int HandleClose() = 0;

        /// send data in \a pMb by socket
        int Send( ACE_Message_Block* pMb );

        int Send(RpcMsgPackPtr pPack, const ACE_Time_Value *tv = &kContentTimeout);

        int SendACK(int msgType);

        RpcMsgPackage *Receive();

        std::string GetRemoteAddr(void) const ;
        std::string Dump();
    protected:
        friend class RefCounted<TcpChannel>;
        virtual ~TcpChannel();

        ACE_SOCK_Stream m_peerStream;
        Task_Base_Ptr m_pTask;
        boost::timed_mutex m_Mutex;
        std::string m_sOpposite;
    };

    ///  \class ServerTcpChannel
    ///  \brief a tcp channel in server side, when receive an data will put 
    ///  to the queue of TcpServer
    class ServerTcpChannel : public TcpChannel
    {
    public:
        explicit ServerTcpChannel(Task_Base_Ptr pTask, const std::string& sOpposite);
        /// handler when io read happen
        virtual int HandleRead();
        /// handler when io close signal happen
        virtual int HandleClose();

        virtual bool ReadVeriInfo();
    private:
        bool m_bValidConnection;
    };

    ///    \class ClientTcpChannel
    ///  \brief a tcp channel of client, when receive an data will be process
    ///  by TcpClient
    class ClientTcpChannel : public TcpChannel
    {
    public:
        explicit ClientTcpChannel(Task_Base_Ptr pTask, const std::string& sOpposite);
        /// handler when io read happen
        virtual int HandleRead();
        /// handler when io close signal happen
        virtual int HandleClose();
    };

}
#endif
