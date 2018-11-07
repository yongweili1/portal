#ifndef _MCSF_ICHANNEL_H
#define _MCSF_ICHANNEL_H

class ACE_Task_Base;

namespace Mcsf {

    class TcpClient;
    class TcpServer;
    class Acceptor;
    typedef boost::shared_ptr<ACE_Task_Base> Task_Base_Ptr;
    typedef boost::shared_ptr<TcpClient>     TcpClient_Ptr;
    typedef boost::shared_ptr<TcpServer>     TcpServer_Ptr;
    typedef boost::shared_ptr<Acceptor>      Acceptor_Ptr;
}

#endif