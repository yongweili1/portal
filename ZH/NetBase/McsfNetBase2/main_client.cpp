#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "client.hpp"
#include "io_service_pool.hpp"
#include "connection_manager.hpp"
#include "callback_handle.hpp"

#include "id_def.h"

const std::string dispat = "SystemDispatcher";



int syncSendCommand(netbasecore::connection_ptr conn,
    netbasecore::io_service_pool &pool, 
    netbasecore::callback_handle &callbackHandlers, int uid, int id, const std::string &d)
{
    netbasecore::util::RpcMsgPackagePtr p(
        new netbasecore::util::RpcMsgPackage(netbasecore::util::msg_type_cmd, "me"));
    p->m_header.uid = uid;
    p->m_header.id = id;
    p->setData(d);

    if (!conn->push_message(p))
    {
        printf("push message error! uid=%d,c=%d\n", p->m_header.uid, p->m_header.id);
        return -1;
    }

    netbasecore::pending_call_ptr call(
        new netbasecore::pending_call(dispat, pool.get_io_service(), true, nullptr, 0));
    callbackHandlers.register_pending_call(p->m_header.uid, call);

    if (call->wait(uid, &callbackHandlers))
    {
        std::string rtn = call->getReplyResult()->m_sData;
        printf("recv rtn=%s\n", rtn.c_str());
    }
    else {
        printf("timeout!!!\n");
    }

    return 0;
}

int sendEvent(netbasecore::connection_ptr conn,
    netbasecore::io_service_pool &pool,
    netbasecore::callback_handle &callbackHandlers, int uid, int id, const std::string &d)
{
    netbasecore::util::RpcMsgPackagePtr p(
        new netbasecore::util::RpcMsgPackage(netbasecore::util::msg_type_evt, "me"));
    p->m_header.uid = uid;
    p->m_header.id = id;
    p->setData(d);

    if (!conn->push_message(p))
    {
        printf("push message error! uid=%d,c=%d\n", p->m_header.uid, p->m_header.id);
        return -1;
    }
    return 0;
}


class testEventHandler : public Mcsf::IEventHandler
{
    virtual int HandleEvent(const std::string& sSender, int iChannelId, int iEventId, const std::string& sEvent)
    {
        printf("revc event : %s\n", sEvent.c_str());
        return 0;
    }
};

int main(int argc, char* argv[])
{
    try
    {
        // Check command line arguments.
//         if (argc != 3)
//         {
//             std::cerr << "Usage: dispatcher_client <address> <port> \n";
//             std::cerr << "  For IPv4, try:\n";
//             std::cerr << "    receiver 0.0.0.0 80\n";
//             std::cerr << "  For IPv6, try:\n";
//             std::cerr << "    receiver 0::0 80\n";
//             return 1;
//         }


        std::string t1 = "123";
        const std::string &t2 = t1;
        std::string t3 = std::move(t2);
        printf("t1= %s\n", t1.c_str());
        printf("t1= %s\n", t2.c_str());
        printf("t1= %s\n", t3.c_str());




       netbasecore::io_service_pool pool(1,1);
       netbasecore::connection_manager mgr;
       netbasecore::callback_handle callbackHandlers("me");
       callbackHandlers.register_event(broad_cast_id, new testEventHandler);
       netbasecore::client c(pool, callbackHandlers, mgr);
       mgr.set_reconnect_handler(boost::bind(&netbasecore::client::handle_reconnect_dispatcher, boost::ref(c), _1));

        // Run the server until stopped.
        boost::thread poll_run(boost::bind(&netbasecore::io_service_pool::run, boost::ref(pool)));
        //pool.run();

       netbasecore::connection_ptr conn = mgr.get_connection(dispat);
         // Initialise the server.
        if (conn == nullptr)
        {
            if (c.Connect( boost::asio::ip::host_name(), "10000", 0))
            {
                conn = mgr.get_connection(dispat);
            }
            else
                return -1;
        }

        std::string message1 = boost::asio::ip::host_name() + ":1200";

        syncSendCommand(conn, pool, callbackHandlers, 0, register_id, message1);
        syncSendCommand(conn, pool, callbackHandlers, 1, find_id, "me");
        sendEvent(conn, pool, callbackHandlers, 2, broad_cast_id, "broadcast message");

        poll_run.join();

        //shutdown
        c.stop();
        mgr.stop_all();
        pool.stop();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    return 0;
}