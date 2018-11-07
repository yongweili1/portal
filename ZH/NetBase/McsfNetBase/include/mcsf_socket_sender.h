////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_socket_sender.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/14
///
/// \brief  
///
////////////////////////////////////////////////////////////////////////////

#ifndef MCSF_SOCKET_SENDER_H_
#define MCSF_SOCKET_SENDER_H_

#include "ace/Task.h"
#include "ace/SOCK_Acceptor.h"

#include "mcsf_blobdata_interface.h"

namespace Mcsf { 

    class SocketSender : public ACE_Task_Base , public IDataSender, public IDumper
    {
    public:
        SocketSender();
        virtual ~SocketSender();

        bool SendRawData( boost::shared_array<char> pBuffer, size_t iLen ); 

        virtual void SetTimeOut( int iMillSecond )
        {
            m_iTimeout = iMillSecond;
        }

        void SetName(const std::string& sName) 
        {
            m_sIpAddr = sName;
        }

        std::string GetName() const 
        {
            return m_sIpAddr;
        }

        virtual McsfSendDataRequest_SendType GetSendType() const 
        {
            return McsfSendDataRequest_SendType_SOCKET;
        }

        virtual int svc(void);
        void DestoryRawData( );
        void CloseDataFile();
        std::string Dump();
    private:

        std::string m_sIpAddr;
        boost::shared_array<char> m_pBuffer;
        size_t m_iLen;
        int m_iTimeout;

        ACE_SOCK_Acceptor m_acceptor;
    };
}

#endif // MCSF_SOCKET_SENDER_H_
