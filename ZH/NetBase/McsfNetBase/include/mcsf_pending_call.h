////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_pending_call.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/05
///
/// \brief  a request call object to process sync and async command request
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_PENDINGCALL_H_
#define _MCSF_PENDINGCALL_H_

#include "ace/Event.h"
#include "ace/Task.h"
#include "boost/thread.hpp"

#include "mcsf_rpc_msg_redef.h"

namespace Mcsf {
    class ICommandCallbackHandler;
    class AsyncCallTimeout;
    class IDataSender;
    class AsyncCallBackThread;

    boost::uint64_t GetCurrentUTCTime();

    class CommandCall
    {
    public:
        CommandCall(int iId, const std::string &sSender, const std::string &sRecv,
            bool bSync, boost::shared_ptr<AsyncCallBackThread> pActive);

        virtual ~CommandCall(); 

        int Wait(unsigned int iWaitTime);

        void SetResult(RpcMsgPackPtr pReplyPck, bool bTimeout = false);
        RpcMsgPackPtr GetResult() const {return m_pReplyPck;}
        int GetId() const { return m_iId; }
        void SetCallBack( ICommandCallbackHandler* pCallback );
        void SetCallBack( IDataSender *pSendData);
        void CancelCallback();
        bool IsCompleted(){return m_bCompleted;}
        void ClearDataFile(bool bIsSyncCmd);
        void TransDataTimeOut(bool value) {m_bTimeOutForTransBigData = value;}
        bool IsTransDataTimeOut() {return m_bTimeOutForTransBigData;}
        bool IsSyncCmd(){return m_bIsSyncCmd;}
        std::string GetSender() const { return m_sender; }
        std::string GetReceiver() const { return m_recver; }

    private:
        int m_iId;
        bool m_bCompleted;
        bool m_bTimeOutForTransBigData;
        ACE_Event m_Event;
        ICommandCallbackHandler* m_pCallback;
        IDataSender* m_pSendData;
        boost::mutex m_mutexCallBack;
        bool m_bIsSyncCmd;
        RpcMsgPackPtr m_pReplyPck;
        boost::shared_ptr<AsyncCallBackThread> m_pClientCbActivObj;
        std::string m_sender;
        std::string m_recver;
    };

    typedef boost::shared_ptr<CommandCall>  CommandCallPtr;

    class AsyncCallTimeout : public ACE_Event_Handler
    {
    public:
        AsyncCallTimeout() {m_bShareMemData = false;}
        virtual int handle_timeout(const ACE_Time_Value &current_time, 
            const void *act );
        virtual int handle_close(ACE_HANDLE handle, 
            ACE_Reactor_Mask close_mask);
        void SetCommandCall(CommandCallPtr p) {m_pCall = p;}
        void SetIsShareMemData(bool value) {m_bShareMemData = value;}       
    private:
        bool m_bShareMemData;
        CommandCallPtr m_pCall;
    };

    class AsyncResult;
    class AsyncCallBackThread : public ACE_Task<ACE_MT_SYNCH>
    {
    public:
        AsyncCallBackThread();
        virtual ~AsyncCallBackThread();

        //static AsyncCallBackThread* GetInstance();

        int Start();
        int svc(void);
        int Stop();
        void DoHandleReply( AsyncResult *pResult );
        ACE_Thread_Mutex m_Mutex;
    };
}

#endif