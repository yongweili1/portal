#include "stdafx.h"

namespace Mcsf {

    SMFileReceiver::SMFileReceiver( const std::string& sName
        ) : m_sSHMKey( sName )
    {
        CErrorHandler::RegisterToDump(this);
        m_pSHMClient = NULL;
    }    
    
    SMFileReceiver::~SMFileReceiver()
    {
        CErrorHandler::UnRegisterToDump(this);
    }
    boost::shared_array<char> SMFileReceiver::GetRawData( size_t& size , bool bIsSyncCmd)
    {
        if( m_pSHMClient != NULL )
        {
            Destory(false);
        }    

        m_pSHMClient = new ACE_Shared_Memory_MM( m_sSHMKey.c_str() );    
        NTB_TRACE_INFO_WITH_THIS( "Get the share memory data filename = " << m_sSHMKey.c_str() );
        size = m_pSHMClient->get_segment_size();
        boost::shared_array<char> pData(new char[size]);        
        memcpy(pData.get(), m_pSHMClient->malloc(), size);        
        // receiver is responsible for release the underlying file
        Destory(bIsSyncCmd);        
        return pData;
    }

    void SMFileReceiver::Destory(bool bIsSyncCmd)
    {
        if( m_pSHMClient != NULL )
        {
            int iRet = 0;
            NTB_TRACE_INFO_WITH_THIS("file name="<<m_pSHMClient->filename());
            iRet = m_pSHMClient->close();
            if (bIsSyncCmd)
            {                                
                iRet = m_pSHMClient->remove();                
                NTB_TRACE_INFO_WITH_THIS("remove data file with iret="<<iRet);
            }
            DEL_PTR(m_pSHMClient);
        }    
    }

    std::string SMFileReceiver::Dump()
    {
        MCSF_TRY
            MCSF_DUMP(m_sDumpInfo, "(void)*", "this:", this);
            MCSF_DUMP(m_sDumpInfo, "string", "m_sSHMKey:", m_sSHMKey);
            MCSF_DUMP(m_sDumpInfo, "(ACE_Shared_Memory_MM)*", "m_pSHMClient:", m_pSHMClient );
            return m_sDumpInfo;
        MCSF_CATCH_ALL
            NTB_LOG_ERROR_WITH_THIS("Dump occurs exception again.");
            return ""; 
        MCSF_TRY_CATCH_END

    }
}

