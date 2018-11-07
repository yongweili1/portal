#include "mcsf_share_memory_sender.h"

#include <stdio.h>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "mcsf_refined_log.h"

namespace Mcsf
{
    SMFileSender::SMFileSender() : m_sSHMKey(""), m_bIsClosed(false) {
    }

    SMFileSender::~SMFileSender()
    {
        DestoryRawData();
    }

    bool SMFileSender::SendRawData( boost::shared_array<char> pBuffer, size_t iLen )
    {
        if( pBuffer == NULL || iLen > MAX_SHARE_MEMORY_CAPCITY )
        {
            NTB_LOG_ERROR("[blob data]invalid parameters! pBuffer=" << pBuffer.get() << " ilen=" << iLen);
            return false;
        }

        char tempFile[L_tmpnam] = { 0 };
        tmpnam(tempFile);
        m_sSHMKey = tempFile;

        boost::interprocess::shared_memory_object shm(
            boost::interprocess::create_only, m_sSHMKey.c_str(), boost::interprocess::read_write);
        shm.truncate(iLen);
        boost::interprocess::mapped_region region(shm, boost::interprocess::read_write);
        memcpy(region.get_address(), pBuffer.get(), iLen);

        return true;
    }

    void SMFileSender::CloseDataFile()
    {
    }

    void SMFileSender::DestoryRawData( )
    {
        if (!m_sSHMKey.empty())
        {
            boost::interprocess::shared_memory_object::remove(m_sSHMKey.c_str());
            m_bIsClosed = true;
        }
    }

    void SMFileSender::SetTimeOut( int iMillSecond )
    {
    }

}

