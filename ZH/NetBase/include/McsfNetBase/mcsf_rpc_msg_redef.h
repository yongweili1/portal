#ifndef __MCSF_RCP_MSG_REDEF_H__
#define __MCSF_RCP_MSG_REDEF_H__

#include <string.h>

#include "boost/cstdint.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/shared_array.hpp"
#include "boost/tuple/tuple.hpp"

#define small_buffer_upper 10240

namespace Mcsf{

    enum
    {
        message_type_cmd,
        message_type_event,

        message_type_ack_request,

        message_type_ack_response,
    };

    struct RpcMsgHeader
    {
        RpcMsgHeader()
            : uDataLen(0)
            , uid(0)
            , id(0)
            , iRsrv(0)
            , uSenderLen(0)
            , iMsgType(0)
            , bRsrv(false)
            , bSyncBig(false)
        {}

        uint32_t uDataLen;

        uint32_t uid;

        uint32_t id;

        uint32_t iRsrv;

        uint16_t uSenderLen;

        int8_t iMsgType;

        bool bRsrv;

        bool bSyncBig;
    };

    typedef boost::shared_ptr<RpcMsgHeader> RpcMsgHeaderPtr;
    typedef boost::tuple<boost::shared_array<char>, uint32_t> SentBuff;
    typedef boost::shared_ptr<std::string> ReceivedBuff;

    class RpcMsgPackage
    {
    public:

        RpcMsgPackage()
            : m_pRawBody(NULL)
            , m_uSentLen(0)
        {}

        void SetSentFields(
            uint32_t uid,
            uint32_t id, 
            const std::string &data,
            bool bSync = false,
            int8_t msgType = 0,
            const std::string &sender = "",
            bool bRsrv = false,
            uint32_t iRsrv = 0)
        {
            uint32_t uDataLen = (uint32_t)data.length();
            uint16_t uSenderLen = (uint16_t)sender.length();
            uint16_t uHeaderLen = (uint16_t)sizeof(RpcMsgHeader);

            if (bSync && uDataLen > small_buffer_upper)
            {
                m_uSentLen = uHeaderLen + uSenderLen;
                m_pSentBuff.reset(new char[m_uSentLen]);

                char *pHeadAddr = m_pSentBuff.get();

                FillHeader((RpcMsgHeader *)pHeadAddr, uDataLen, uid, id, iRsrv, uSenderLen, msgType, bRsrv, true);

                if (uSenderLen)
                    memcpy(pHeadAddr + uHeaderLen, &*sender.begin(), uSenderLen );

                m_pRawBody = &data;
            }
            else
            {
                m_uSentLen = uHeaderLen + uSenderLen + uDataLen;
                m_pSentBuff.reset(new char[m_uSentLen]);

                char *pHeadAddr = m_pSentBuff.get();

                FillHeader((RpcMsgHeader *)pHeadAddr, uDataLen, uid, id, iRsrv, uSenderLen, msgType, bRsrv, false);

                // first data then sender for parse efficiency when data is big.
                if (uDataLen)
                    memcpy(pHeadAddr + uHeaderLen , &*data.begin(), uDataLen);

                if (uSenderLen)
                    memcpy(pHeadAddr + uHeaderLen + uDataLen, &*sender.begin(), uSenderLen );
            }
        }

        RpcMsgHeader *GetHeader()
        {
            return m_pHeader ? m_pHeader.get() : (RpcMsgHeader *)(m_pSentBuff.get());
        }

        SentBuff GetSentBuff()
        {
            return SentBuff(m_pSentBuff, m_uSentLen);
        }

        ReceivedBuff GetReceivedBuff() const
        {
            return m_pReceivedBuff;
        }

        const std::string *GetRawBody() const
        {
            return m_pRawBody;
        }

        const std::string &GetSender() const
        {
            return m_sSender;
        }

        void SetReceivedFields(RpcMsgHeaderPtr header, ReceivedBuff buff)
        {
            // header not null forever.
            m_pHeader = header;
            m_pReceivedBuff = buff;

            if (m_pHeader->uSenderLen)
            {
                // buff now not null
                m_sSender = m_pReceivedBuff->substr(m_pHeader->uDataLen);
                //attention memmove if first sender then data.
                m_pReceivedBuff->erase(m_pHeader->uDataLen);
            }
        }

    private:

        void FillHeader(RpcMsgHeader *pHeader, uint32_t &uDataLen, uint32_t &uid, 
            uint32_t &id, uint32_t &iRsrv, uint16_t uSenderLen, int8_t iMsgType, bool bRsrv, bool tag)
        {
            pHeader->bRsrv = bRsrv;
            pHeader->id = id;
            pHeader->uDataLen = uDataLen;
            pHeader->uid = uid;
            pHeader->uSenderLen = uSenderLen;
            pHeader->iRsrv = iRsrv;
            pHeader->iMsgType = iMsgType;
            pHeader->bSyncBig = tag;
        }

    private:

        ReceivedBuff m_pReceivedBuff;

        RpcMsgHeaderPtr m_pHeader;

        std::string m_sSender;

        const std::string *m_pRawBody;

        boost::shared_array<char> m_pSentBuff;
        uint32_t m_uSentLen;
    };

    typedef boost::shared_ptr<RpcMsgPackage> RpcMsgPackPtr;
}
#endif