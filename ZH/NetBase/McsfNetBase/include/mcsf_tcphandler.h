////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_tcphandler.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/05
///
/// \brief  a tcp handler class register in ace reactor
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_TCPHANDLER_H
#define _MCSF_TCPHANDLER_H

#include "ace/Event_Handler.h"
#include "ace/Reactor.h"
#include "ace/SOCK_Stream.h"
#include "boost/shared_ptr.hpp"
#include "boost/thread.hpp"
#include "mcsf_tcp_channel.h"
#include "mcsf_ichannel.h"
#include "mcsf_irefcount.h"
#include "McsfErrorHandler/mcsf_error_handler.h"

namespace Mcsf {

    /*
     *  \class TcpHandler
     *  \brief a tcp handler class register in ace reactor
     */
    class TcpHandler : public ACE_Event_Handler , public RefCounted<TcpHandler>, public IDumper
    {
    public:
        TcpHandler( ACE_Reactor *reactor, Task_Base_Ptr pTask , const std::string& sOpposite="");
        virtual ~TcpHandler();

        // Activate the object.
        virtual int open ();
        /// hook function when input event happen 
        virtual int handle_input (ACE_HANDLE fd= ACE_INVALID_HANDLE);
        /// called when a handler_*() return -1
        virtual int handle_close (ACE_HANDLE fd= ACE_INVALID_HANDLE,
            ACE_Reactor_Mask = 0);

        virtual int handle_timeout(const ACE_Time_Value &current_time, 
            const void *act );

        virtual ACE_HANDLE get_handle (void) const {
            TcpChannel* channel = const_cast<TcpChannel*>(m_pTcpChannel);
            return channel->peer().get_handle();
        };

         ACE_SOCK_Stream &peer () {
             return m_pTcpChannel->peer();
         }

         TcpChannel* GetChannel() {
             return m_pTcpChannel;
         }

         void ClearResource();

         std::string Dump();

         void SetSendFailedMask(bool b) { m_bSendfailedMask = b; }

    private:
        ///relative tcp channel
        TcpChannel* m_pTcpChannel;
        std::string m_sOpposite;
        ACE_Reactor *m_pReactor;
        int m_iTimeOutCount ;
        bool m_bIsServer;
        bool m_bSendfailedMask;
        bool m_bCleared;
    };


}

#endif