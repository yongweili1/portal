////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_acceptor.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/01
///
/// \brief  An acceptor pattern to accept socket and registe it to reactor
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_ACCEPTOR_H
#define _MCSF_ACCEPTOR_H

#include <string>

#include "ace/Event_Handler.h"
#include "ace/INET_Addr.h"
#include "ace/Reactor.h"
#include "ace/SOCK_Acceptor.h"

#include "mcsf_ichannel.h"
#include "McsfErrorHandler/mcsf_error_handler.h"
class ACE_Task_Base;

namespace Mcsf {
     class ClientManager;
    /// \class Acceptor
    /// \brief a class base on ACE Accept pattern
    class Acceptor : public ACE_Event_Handler, public IDumper
    {
    public:
        explicit Acceptor(Task_Base_Ptr pServer, ClientManager* pClientMgr);
        virtual ~Acceptor();
         
        /// open an address and listen on it
        /// 
        /// \param   local_addr   address to listen on    
        /// \return  int          return zero if success
        virtual int open ( const ACE_INET_Addr &local_addr );
         
        /// virtual function override from ACE_Event_Handler, 
        /// Called when input events occur
        ///
        /// \param   an handler represent underly socket         
        /// \return  int  return 0 success , if you return -1 will unregister from reactor
        virtual int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE);
         
        /// Called when a handle_*() method returns -1 or when the
        /// remove_handler() method is called on an ACE_Reactor.  The
        /// \a close_mask indicates which event has triggered the
        /// handle_close() method callback on a particular \a handle.
        virtual int handle_close (ACE_HANDLE = ACE_INVALID_HANDLE,
            ACE_Reactor_Mask close_mask= 0);

        ///Get the I/O handle. 
        virtual ACE_HANDLE get_handle () const {
            return m_acceptor.get_handle();
        }

        /// get the listen ip address
        std::string GetListenAddr() const;

        /// get listen port
        std::string GetListenPort() const;
        
        std::string Dump();
    protected:
        ACE_SOCK_Acceptor m_acceptor;       /// ACE Acceptor object
        Task_Base_Ptr m_pServer;           /// the thread task that associate
        ClientManager* m_pClientMgr;
    };
}

#endif
