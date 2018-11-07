#include "stdafx.h"

namespace Mcsf {


    DynamicThreadPool::DynamicThreadPool()
    {
        CErrorHandler::RegisterToDump(this);
        m_iFlag = THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED;
        m_pFreeThread = NULL;
        this->msg_queue()->high_water_mark(1024*1024*16);
        this->msg_queue()->low_water_mark(1024*1024*16);
    }

    DynamicThreadPool::~DynamicThreadPool()
    {
        CErrorHandler::UnRegisterToDump(this);
        DEL_PTR( m_pFreeThread );
    }


    int DynamicThreadPool::StartPool( int iMinNum, int iMaxNum )
    {
        m_iMinNum = iMinNum;
        m_iMaxNum = iMinNum;

        m_pFreeThread = new ACE_Thread_Semaphore(0, NULL, NULL, iMaxNum);  
        CHECK_NULL_PTR(m_pFreeThread);
        return this->activate( m_iFlag, iMinNum );
    }


    int DynamicThreadPool::AddTask( ACE_Message_Block* pBlock )
    {
        if( m_pFreeThread->tryacquire() == -1 )
        {
            //can not acquire semaphore, need enlarge thread pool size
            this->activate( m_iFlag, 1, 1 );
        }else
        {
            m_pFreeThread->release();
        }

        return this->putq( pBlock );
    }

    int DynamicThreadPool::svc( void )
    {
        m_pFreeThread->release();

        while(true)
        {
            ACE_Message_Block *mb = NULL;

            if( -1 == this->getq( mb ) )
            {
                return 0;
            }

            //now when i am busy i decrease one count of semaphore 
            ACE_GUARD_RETURN( ACE_Thread_Semaphore, guard, *m_pFreeThread, -1 );
            this->ServiceFunc( mb );
        }
    }

    std::string DynamicThreadPool::Dump()
    {
        MCSF_TRY
            MCSF_DUMP(m_sDumpInfo, "(void)*", "this:", this );
            MCSF_DUMP(m_sDumpInfo, "int", "m_iMaxNum:", m_iMaxNum);
            MCSF_DUMP(m_sDumpInfo, "int", "m_iMinNum:", m_iMinNum);
            MCSF_DUMP(m_sDumpInfo, "ACE_Thread_Semaphore *", "m_pFreeThread:", m_pFreeThread);
            MCSF_DUMP(m_sDumpInfo, "long", "m_iFlag:", m_iFlag);
            return m_sDumpInfo;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Dump occurs exception again.");
            return ""; 
        MCSF_TRY_CATCH_END

    }
}