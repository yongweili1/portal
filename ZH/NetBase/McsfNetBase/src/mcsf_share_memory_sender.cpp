#include "stdafx.h"
namespace Mcsf
{
    SMFileSender::SMFileSender(  
        )  : m_sSHMKey( "" )
    {
        CErrorHandler::RegisterToDump(this);
        m_bIsClosed = false;
        m_pshmServer = NULL;
    }

    SMFileSender::~SMFileSender()
    {
        CErrorHandler::UnRegisterToDump(this);
        NTB_TRACE_INFO_WITH_THIS("SMFileSender::~SMFileSender");        
        DestoryRawData();
    }

    bool SMFileSender::SendRawData( boost::shared_array<char> pBuffer, size_t iLen )
    {
        if( pBuffer == NULL || iLen > MAX_SHARE_MEMORY_CAPCITY )
        {
            return false;
        }

        if( m_pshmServer != NULL )
        {
            DestoryRawData();
        }

        //Is there problem if buffer address is not start multiple of 64kb ?? Is Yes, we have 
        //to do a memcpy.
        char* tempFile = ACE_OS::tempnam();
        m_sSHMKey = tempFile;
        free( tempFile );

        NTB_TRACE_INFO_WITH_THIS( "Start Create Shared Memory. filename is " << m_sSHMKey.c_str() );
        m_pshmServer = new ACE_Shared_Memory_MM( m_sSHMKey.c_str(), iLen );
        CHECK_NULL_PTR(m_pshmServer);
        if( m_pshmServer->malloc() == NULL )
        {
            NTB_LOG_ERROR_WITH_THIS("create shared memory failed.m_sSHMKey="<<m_sSHMKey<<" m_bIsClosed="<<m_bIsClosed);
            return false;
        }
        memcpy( m_pshmServer->malloc(), pBuffer.get(), iLen );
        NTB_TRACE_INFO_WITH_THIS( "End Create Shared Memory." );

        return true;
    }

    void SMFileSender::CloseDataFile()
    {
        NTB_TRACE_INFO_WITH_THIS( "Destory Shared Memory." );
        int iRet = 0;
        if( m_pshmServer != NULL )
        {
            NTB_TRACE_INFO_WITH_THIS("file name="<<m_pshmServer->filename());
            
            iRet = m_pshmServer->close();
            m_bIsClosed = true;
            NTB_TRACE_INFO_WITH_THIS("iret="<<iRet);            
        }
    }

    void SMFileSender::DestoryRawData( )
    {
        NTB_TRACE_INFO_WITH_THIS( "Destory Shared Memory." );
        int iRet = 0;
        if( m_pshmServer != NULL )
        {
            NTB_TRACE_INFO_WITH_THIS("file name="<<m_pshmServer->filename());
            if (false == m_bIsClosed)
            {
                iRet = m_pshmServer->close();
                NTB_TRACE_INFO_WITH_THIS("iret="<<iRet);
            }
            iRet = m_pshmServer->remove();
            NTB_TRACE_INFO_WITH_THIS("iret="<<iRet);

            DEL_PTR(m_pshmServer);
        }
    }

    void SMFileSender::SetTimeOut( int iMillSecond )
    {
        ACE_UNUSED_ARG( iMillSecond );
    }

    std::string SMFileSender::Dump()
    {
        MCSF_TRY
            MCSF_DUMP(m_sDumpInfo, "(void)*", "this:", this);
            MCSF_DUMP(m_sDumpInfo, "string", "m_sSHMKey:", m_sSHMKey);
            MCSF_DUMP(m_sDumpInfo, "(ACE_Shared_Memory_MM)*", "m_pshmServer:", m_pshmServer );
            MCSF_DUMP(m_sDumpInfo, "bool", "m_bIsClosed:", m_bIsClosed );
            return m_sDumpInfo;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Dump occurs exception again.");
            return ""; 
        MCSF_TRY_CATCH_END

    }
}

