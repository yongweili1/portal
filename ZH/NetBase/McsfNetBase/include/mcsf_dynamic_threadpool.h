////////////////////////////////////////////////////////////////////////////
///
/// Copyright, (c) Shanghai United-imaging Healthcare Inc., 2011
/// All rights reserved
///
/// \file   mcsf_dynamic_threadpool.h
/// \author Ruifei.liu <ruifei.liu@united-imaging.com>
///
/// \date   2011/09/05
///
/// \brief  a dynamic thread pool ( not totaly implement)
///
////////////////////////////////////////////////////////////////////////////

#ifndef _MCSF_DYNAMICTHREAD_POOL_H
#define _MCSF_DYNAMICTHREAD_POOL_H

#include "ace/Task.h"
#include "ace/Thread_Mutex.h"
#include "ace/Thread_Semaphore.h"

namespace Mcsf {

    class DynamicThreadPool : public ACE_Task<ACE_MT_SYNCH>, public IDumper
    {
    public:
        DynamicThreadPool();
        virtual ~DynamicThreadPool();

        int StartPool( int iMinNum, int iMaxNum );

        int AddTask( ACE_Message_Block* pBlock );

        virtual int ServiceFunc( ACE_Message_Block* mb ) = 0;
        virtual int svc(void);
        virtual std::string Dump();
    private:
        int m_iMaxNum;
        int m_iMinNum;

        ACE_Thread_Semaphore *m_pFreeThread;
        long m_iFlag;

    };

}

#endif