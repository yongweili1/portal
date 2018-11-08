#ifndef RPC_HEADER_HPP
#define RPC_HEADER_HPP

#include <string>
#include <boost/cstdint.hpp>
#include "boost/shared_ptr.hpp"
#include "boost/shared_array.hpp"
#include <list>
#include <vector>

#include "boost/assign.hpp"
#include <boost/asio.hpp>
#include <boost/function.hpp>

namespace netbasecore {

    namespace util
    {
        enum {
            msg_type_first = 1,
            msg_type_cmd,
            msg_type_cmd_reply,
            msg_type_evt,
            msg_type_heartbeat,
        };

#define package_check_number (0xAE)

        struct RpcMsgHeader
        {
            RpcMsgHeader()
                : uDataLen(0)
                , uid(0)
                , id(0)
                , iRsrv(0)
                , uSenderLen(0)
                , iMsgType(0)
                , iCheckByte(package_check_number)
            {}

            uint8_t iMsgType;

            uint8_t iCheckByte;

            uint16_t uSenderLen;

            uint32_t uDataLen;

            uint32_t uid;

            uint32_t id;

            uint32_t iRsrv;
        };

#define util_header_len (sizeof(netbasecore::util::RpcMsgHeader))

        struct RpcMsgPackage
        {
            RpcMsgPackage() {}
            RpcMsgPackage(int8_t msgType) {
                this->m_header.iMsgType = msgType;
            }

            void setData(const std::string &data) {
                this->m_sData = std::move(data); //unavoidable copy
                this->m_header.uDataLen = (uint32_t)m_sData.size();
            }

            void setSender(const std::string &sender) {
                this->m_sSender = sender;
            }

            std::vector<boost::asio::const_buffer> to_buffers()
            {
                std::vector<boost::asio::const_buffer> buffers;
                buffers.push_back(boost::asio::buffer(&m_header, util_header_len));
                buffers.push_back(boost::asio::buffer(m_sData));
                return buffers;
            }

            void swapData() {
                this->m_sData = std::move(std::string(m_RecvBuff.get(), m_header.uDataLen));
                m_RecvBuff.reset();
            }

            std::string m_sSender;
            std::string m_sData;
            RpcMsgHeader m_header;
            boost::shared_array<char> m_RecvBuff;
        };

        typedef boost::shared_ptr<RpcMsgPackage> RpcMsgPackagePtr;

        struct host_port_t {
            host_port_t()
                : parsed_ok(false) { }

            host_port_t(const std::string& blok)
                : parsed_ok(false) {
                size_t pos = blok.find(':');
                if (pos != std::string::npos) {
                    host = blok.substr(0, pos);
                    port = blok.substr(pos + 1);
                    parsed_ok = true;
                }
            }

            std::string host;
            std::string port;
            bool parsed_ok;

            std::string to_string() {
                if (host.empty() || port.empty())
                    return "";
                return host + ":" + port;
            }
        };
    }
}


#endif RPC_HEADER_HPP