#ifndef __DISPATCHER_HPP__
#define __DISPATCHER_HPP__

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "server.hpp"
#include "callback_handle.hpp"
#include "SystemCmdMessage.pb.h"
#include "mcsf_refined_log.h"

namespace dispatcher {

    class address_managment
    {
    public:
        address_managment(netbasecore::connection_manager &mng) : client_mng_(mng) {}

        void add(const std::string &name, Mcsf::McsfRegisterContaineeRequest& mac_ip_port)
        {
            boost::mutex::scoped_lock l(mux);
            if (name2mac_addr_map_.find(name) != name2mac_addr_map_.end()) {
                DSP_LOG_INFO("replace " << name);
            }
            name2mac_addr_map_[name] = mac_ip_port;
            DSP_LOG_INFO(name << " mac_ip_port=" << mac_ip_port.name() << mac_ip_port.ip_addr() << " added to map. capacity=" << name2mac_addr_map_.size());
        }

        void erase(const std::string &name)
        {
            boost::mutex::scoped_lock(mux);
            name2mac_addr_map_.erase(name);
        }

        netbasecore::util::host_port_t get_adrress(const std::string &name, const std::string & sender) {
            boost::mutex::scoped_lock(mux);

            Mcsf::McsfRegisterContaineeRequest sender_addr_info;
            auto my = name2mac_addr_map_.find(sender);
            if (my == name2mac_addr_map_.end()) {
                DSP_LOG_WARN("dispatcher has no information of sender=" << sender);
                return netbasecore::util::host_port_t("");
            }

            sender_addr_info = my->second;

            auto it = name2mac_addr_map_.find(name);
            if (it == name2mac_addr_map_.end()) {
                return netbasecore::util::host_port_t("");
            }

            Mcsf::McsfRegisterContaineeRequest target_addr_info = it->second;
            netbasecore::util::host_port_t addr(target_addr_info.ip_addr());
            // sender mac if equals the target mac
            if (is_same_pc(sender_addr_info.name(), target_addr_info.name())) {
                addr.host = boost::asio::ip::address_v4::loopback().to_string();
                return addr;
            }

            // different machine
            netbasecore::connection_ptr c = client_mng_.get_connection(name);
            if (c) {
                std::string ip_find = c->get_remote_addr();
                if (boost::asio::ip::address_v4::from_string(ip_find).is_loopback()) {
                    //the target is same machine to dispatcher
                    netbasecore::util::host_port_t addrs_sender(sender_addr_info.ip_addr());
                    ip_find = try_best_find_match_ip(addrs_sender, addr);
                }

                addr.host = ip_find;
                return addr;
            }
            else {
                DSP_LOG_WARN("dispatcher has no connection with target=" << name);
                return netbasecore::util::host_port_t("");
            }
        }

    private:
        static std::string find_same_segment_ip(const std::set<std::string> &setIP_sender,
            const std::set<std::string> &setIP_target, const std::string &net_segment)
        {
            auto netmask = boost::asio::ip::address_v4::from_string(net_segment);
            BOOST_FOREACH(const std::string& ip, setIP_sender) {
                auto ip_sender = boost::asio::ip::address_v4::from_string(ip);

                BOOST_FOREACH(const std::string& ip2, setIP_target) {
                    auto ip_target = boost::asio::ip::address_v4::from_string(ip2);
                    if ((ip_sender.to_ulong() & netmask.to_ulong()) == (ip_target.to_ulong() & netmask.to_ulong())) {
                        return ip2;
                    }
                }
            }
            return "";
        }

        static std::string try_best_find_match_ip(netbasecore::util::host_port_t &sender_addrs, netbasecore::util::host_port_t &target_addrs)
        {
            std::set<std::string> setIP_sender;
            boost::algorithm::split(setIP_sender, sender_addrs.host, boost::is_any_of("&"));
            setIP_sender.erase(boost::asio::ip::address_v4::loopback().to_string());
            std::set<std::string> setIP_target;
            boost::algorithm::split(setIP_target, target_addrs.host, boost::is_any_of("&"));
            setIP_target.erase(boost::asio::ip::address_v4::loopback().to_string());

            std::string tmp = find_same_segment_ip(setIP_sender, setIP_target, "255.255.255.0");
            if (tmp.empty()) {
                tmp = find_same_segment_ip(setIP_sender, setIP_target, "255.255.0.0");
                if (tmp.empty()) {
                    tmp = find_same_segment_ip(setIP_sender, setIP_target, "255.0.0.0");
                }
            }
            return tmp;
        }

        static bool is_same_pc(const std::string &sMac1, const std::string &sMac2) {
            if (sMac2 == sMac1) {
                return true;
            }

            std::set<std::string> set1;
            boost::algorithm::split(set1, sMac1, boost::is_any_of("|"));

            std::set<std::string> set2;
            boost::algorithm::split(set2, sMac2, boost::is_any_of("|"));

            BOOST_FOREACH(const std::string& mac, set1) {
                if (set2.end() != set2.find(mac)) {
                    return true;
                }
            }

            return false;
        }

    private:
        std::map<std::string, Mcsf::McsfRegisterContaineeRequest> name2mac_addr_map_;
        boost::mutex mux;
        netbasecore::connection_manager &client_mng_;
    };

    class register_command_handler : public Mcsf::ICommandHandlerEx
    {
    public:
        register_command_handler(address_managment & addr_mng)
            : addr_mng_(addr_mng) {}

        virtual void HandleCommand(boost::shared_ptr < Mcsf::ICommandContextEx > pContext)
        {
            Mcsf::McsfRegisterContaineeRequest regQuest;
            if (regQuest.ParseFromString(pContext->GetSerializeObject())) {
                addr_mng_.add(pContext->GetSender(), regQuest);
                pContext->Reply(boost::lexical_cast<std::string>(int(true)));
            }
            else {
                pContext->Reply(boost::lexical_cast<std::string>(int(false)));
            }
        }

    private:
        address_managment &addr_mng_;
    };

    class find_command_handler : public Mcsf::ICommandHandlerEx
    {
    public:
        find_command_handler(address_managment & addr_mng)
            : addr_mng_(addr_mng) {}

        virtual void HandleCommand(boost::shared_ptr < Mcsf::ICommandContextEx > pContext)
        {
            std::string queryIP = addr_mng_.get_adrress(pContext->GetSerializeObject(), pContext->GetSender()).to_string();
            DSP_LOG_INFO("query [" << pContext->GetSerializeObject() << "] get " << queryIP);
            pContext->Reply(queryIP);
        }

    private:
        address_managment &addr_mng_;
    };

    class broadcast_event_handler : public Mcsf::IEventHandler
    {
    public:
        broadcast_event_handler(netbasecore::connection_manager &conn_mng)
            : conn_mng_(conn_mng) {}

        virtual int HandleEvent(const std::string& sSender, int iChannelId, int iEventId, const std::string& sEvent)
        {
            netbasecore::util::RpcMsgPackagePtr p(new netbasecore::util::RpcMsgPackage(
                netbasecore::util::msg_type_evt));
            p->m_header.id = iEventId;
            p->m_header.iRsrv = iChannelId;
            p->m_header.uSenderLen = sSender.size();
            p->setData(sEvent + sSender);

            std::vector<netbasecore::connection_ptr> vecOnline = conn_mng_.get_online();

            for (auto it = vecOnline.begin(); it != vecOnline.end(); ++it) {
                (*it)->push_message(p);
            }
            return 0;
        }

    private:
        netbasecore::connection_manager &conn_mng_;
    };

    class subs_event_mngment {

    public:
        void add(const std::string &who, const std::set<int> &eventIDs) {
            boost::mutex::scoped_lock lock(tx_);
            m_mapWho2SubsId[who].insert(eventIDs.begin(), eventIDs.end());
        }

        void erase(const std::string &name) {
            boost::mutex::scoped_lock lock(tx_);
            m_mapWho2SubsId.erase(name);
        }

        std::set<std::string> find_who_subs(int eventID) {
            std::set<std::string> setRtn;
            boost::mutex::scoped_lock lock(tx_);
            for (auto it = m_mapWho2SubsId.begin(); it != m_mapWho2SubsId.end(); ++it)
            {
                if (it->second.find(eventID) != it->second.end())
                    setRtn.insert(it->first);
            }

            return setRtn;
        }
    private:
        boost::mutex tx_;
        std::map<std::string, std::set<int> > m_mapWho2SubsId;
    };

    class subs_command_handler : public Mcsf::ICommandHandlerEx
    {
    public:
        subs_command_handler(subs_event_mngment & mng) : sub_mngment_(mng) {}

        virtual void HandleCommand(boost::shared_ptr<Mcsf::ICommandContextEx> pContext) {
            std::string sContent = pContext->GetSerializeObject();
            std::set<std::string> strIDs;
            boost::split(strIDs, sContent, boost::is_any_of(","));

            std::set<int> IDs;
            BOOST_FOREACH(const std::string &s, strIDs) {
                IDs.insert(atoi(s.c_str()));
            }
            sub_mngment_.add(pContext->GetSender(), IDs);
            pContext->Reply("");
        }

    private:
        subs_event_mngment &sub_mngment_;
    };

    class proxy_leave_clean_wrapper
    {
    public:
        proxy_leave_clean_wrapper(subs_event_mngment &sub_mng, address_managment &addr_mng)
            : sub_mng_(sub_mng), addr_mng_(addr_mng) {}

        void clean(const std::string& name) {
            addr_mng_.erase(name);
            sub_mng_.erase(name);
        }

    private:
        subs_event_mngment &sub_mng_;
        address_managment &addr_mng_;
    };

    class send_event_handler : public Mcsf::IEventHandler
    {
    public:
        send_event_handler(netbasecore::connection_manager &conn_mng, subs_event_mngment &sub_mng)
            : conn_mng_(conn_mng), sub_mngment_(sub_mng) {}

        virtual int HandleEvent(const std::string& sSender, int iChannelId, int iEventId, const std::string& sEvent)
        {
            netbasecore::util::RpcMsgPackagePtr p(new netbasecore::util::RpcMsgPackage(
                netbasecore::util::msg_type_evt));
            p->m_header.id = iEventId;
            p->m_header.iRsrv = iChannelId;
            p->m_header.uSenderLen = sSender.size();
            p->setData(sEvent + sSender);

            std::set<std::string> Subscribers = sub_mngment_.find_who_subs(iEventId);

            for (auto it = Subscribers.begin(); it != Subscribers.end(); ++it) {
                netbasecore::connection_ptr conn = conn_mng_.get_connection(*it);
                if (conn)
                    conn->push_message(p);
            }
            return 0;
        }

    private:
        netbasecore::connection_manager &conn_mng_;
        subs_event_mngment &sub_mngment_;
    };

    class echoCommandHandler : public Mcsf::ICommandHandlerEx
    {
        virtual void HandleCommand(boost::shared_ptr<Mcsf::ICommandContextEx> pContext) {
            pContext->Reply(pContext->GetSerializeObject());
        }
    };

}
#endif
